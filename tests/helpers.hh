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
#include <iostream>
#include <string>
#include <fstream>


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

// returns true on identity, false otherwise
static inline bool
filecmp(const std::string &filename1,
        const std::string &filename2)
{
    std::ifstream fp1(filename1), fp2(filename2);

    if (!fp1 || !fp2) {
        return false;
    }

    if (std::equal(std::istreambuf_iterator<char>(fp1),
                   std::istreambuf_iterator<char>(),
                   std::istreambuf_iterator<char>(fp2))) {
        return true;
    }
    return false;
}
// returns true on identity, false otherwise
static inline bool
filecmp2(const std::string &filename1,
         const std::string &filename2)
{
    char chr1, chr2;
    std::ifstream fp1(filename1), fp2(filename2);

    if (!fp1 || !fp2) {
        return false;
    }

    while (fp1.good() && fp2.good()) {
        fp1.get(&chr1, 1);
        fp2.get(&chr2, 1);
        if (chr1 != chr2) {
            return false;
        }
    }
    if (fp1.eof() && fp2.eof()) {
        return true;
    }
    return false;
}

// returns true on identity, false otherwise
static inline bool
filestrcmp(const std::string &filename,
           const std::string &contents)
{
    std::ifstream fp(filename, std::ios_base::binary);
    size_t idx = 0;

    if (!fp) {
        return false;
    }

    while (fp.good() && idx < contents.size()) {
        char chr = fp.get();
        if (chr != contents[idx]) {
            return false;
        }
        idx++;
    }
    if (idx == contents.size() && fp.get() == EOF) {
        return true;
    }
    return false;
}

#endif /* HELPERS_HH */
