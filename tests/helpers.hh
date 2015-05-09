/*
 * ============================================================================
 *
 *       Filename:  helpers.hh
 *    Description:  Helpers for libqc++ tests.
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#ifndef HELPERS_HH
#define HELPERS_HH


#include <cstdlib>
#include <cstdio>
#include <sstream>
#include <string>

class TestConfig
{
public:
    TestConfig() :
        n_writable_files(0)
    {

    }

    ~TestConfig()
    {
        for (std::string &file: files_to_delete) {
            std::remove(file.c_str());
        }
    }

    std::string
    get_data_file(std::string file)
    {
        std::ostringstream fs;

        if (root.size() == 0) {
            _get_root();
        }

        fs << root << "/data/" << file;
        return fs.str();
    }

    std::string
    get_writable_file(bool keep=false)
    {
        std::ostringstream fs;

        if (root.size() == 0) {
            _get_root();
        }

        fs << root << "/data/" << ++n_writable_files;
        if (keep) {
            files_to_delete.push_back(fs.str());
        }
        return fs.str();
    }

    static TestConfig *
    get_config()
    {
        return &instance;
    }

protected:
    void
    _get_root()
    {
        const char *envroot = std::getenv("LIBQCPP_DATA_ROOT");

        if (envroot != NULL) {
            root = envroot;
            return;
        }

        // Use full path if we can
        envroot = std::getenv("PWD");
        if (envroot != NULL) {
            root = envroot;
            return;
        }

        // Fall back on './'
        root = ".";
    }

    std::string root;
    std::vector<std::string> files_to_delete;
    size_t n_writable_files;

    static TestConfig instance;
};

extern TestConfig _config;
#endif /* HELPERS_HH */
