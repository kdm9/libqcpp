/*
 * ============================================================================
 *
 *       Filename:  qc-io.hh
 *    Description:  Read I/O Operations. Adapted from Khmer
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#ifndef QC_IO_HH
#define QC_IO_HH

#include "qc-config.hh"

#include <exception>
#include <stdexcept>
#include <sstream>
#include <string>

namespace qcpp
{

class IOError : public std::runtime_error
{
public:
    explicit IOError           (const std::string &msg) :
        std::runtime_error(msg) {}

    explicit IOError           (const char *msg) :
        std::runtime_error(msg) {}
};

class Read
{
public:
    std:: string    name;
    std:: string    sequence;
    std:: string    quality;

    void clear();

    std::string str();
};
bool operator==(const Read &r1, const Read &r2);


typedef std::pair<Read, Read>   ReadPair;


struct SeqAnReadWrapper;
struct SeqAnWriteWrapper;

template<typename SeqAnWrapper>
class ReadIO
{
public:
    ReadIO                     ();
    ~ReadIO                    ();

    void
    open                       (const char         *filename);

    void
    open                       (const std::string  &filename);

    size_t
    get_num_reads              ();

protected:

    SeqAnWrapper *_private;
    size_t          _num_reads;
    bool            _has_qual;
};

class ReadInputStream
{
public:
    virtual bool
    parse_read                 (Read               &the_read) = 0;

    virtual bool
    parse_read_pair            (ReadPair           &the_read_pair) = 0;

    bool
    at_end                     ();

protected:
    bool _at_end;
};

class ReadParser: public ReadInputStream, public ReadIO<SeqAnReadWrapper>
{
public:
    bool
    parse_read                 (Read               &the_read);

    bool
    parse_read_pair            (ReadPair           &the_read_pair);

};

class ReadOutputStream
{
public:
    void
    write_read                 (Read               &the_read);

    void
    write_read_pair            (ReadPair           &the_read_pair);

};

class ReadWriter: public ReadOutputStream, public ReadIO<SeqAnWriteWrapper>
{
public:
    void
    write_read                 (Read               &the_read);

    void
    write_read_pair            (ReadPair           &the_read_pair);

};

class ReadInterleaver : public ReadInputStream
{

public:
    ReadInterleaver            ();

    void
    open                       (const char         *r1_filename,
                                const char         *r2_filename);

    void
    open                       (const std::string  &r1_filename,
                                const std::string  &r2_filename);
    bool
    parse_read                 (Read           &the_read) {return false;}

    bool
    parse_read_pair            (ReadPair           &the_read_pair);

    size_t
    get_num_reads              ();

    size_t
    get_num_pairs              ();

private:
    ReadParser      r1_parser;
    ReadParser      r2_parser;
    size_t _num_pairs;

};

class ReadDeInterleaver : public ReadOutputStream
{

public:
    ReadDeInterleaver          ();

    void
    open                       (const char         *r1_filename,
                                const char         *r2_filename);

    void
    open                       (const std::string  &r1_filename,
                                const std::string  &r2_filename);
    void
    write_read                 (Read               &the_read) {}

    void
    write_read_pair            (ReadPair           &the_read_pair);

    size_t
    get_num_reads              ();

    size_t
    get_num_pairs              ();

private:
    ReadWriter      r1_writer;
    ReadWriter      r2_writer;
    size_t          _num_pairs;
};


} // namespace qcpp

#endif /* QC_IO_HH */
