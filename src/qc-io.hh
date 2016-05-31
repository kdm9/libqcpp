/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_IO_HH
#define QC_IO_HH

#include "qc-config.hh"
#include "qc-ds.hh"

namespace qcpp
{

/*******************************************************************************
*                                 Exceptions                                  *
*******************************************************************************/

class IOError : public std::runtime_error
{
public:
    explicit IOError            (const std::string &msg) :
        std::runtime_error(msg) {}

    explicit IOError            (const char        *msg) :
        std::runtime_error(msg) {}
};


/*******************************************************************************
*                              ReadIO Base Class                              *
*******************************************************************************/
// Declare wrappers from the source. We keep these in obfuscated structs to
// avoid having to install the SeqAn headers, or compile them in every source
// file.
struct SeqAnReadWrapper;
struct SeqAnWriteWrapper;

template<typename SeqAnWrapper>
class ReadIO
{
public:
    ReadIO                      ();
    ~ReadIO                     ();

    void
    open                        (const char        *filename);

    void
    open                        (const std::string &filename);

    void
    close                       ();

    size_t
    get_num_reads               ();

protected:

    SeqAnWrapper           *_private;
    uint64_t                _num_reads;
    bool                    _has_qual;
};


/*******************************************************************************
*                         ReadInputStream base class                          *
*******************************************************************************/
class ReadInputStream
{
public:
    virtual bool
    parse_read                  (Read              &the_read) = 0;

    virtual bool
    parse_read_pair             (ReadPair          &the_read_pair) = 0;

    bool
    at_end                      ();

protected:
    bool                    _at_end=false;
};


class ReadOutputStream
{
public:
    virtual void
    write_read                  (Read              &the_read) = 0;

    virtual void
    write_read_pair             (ReadPair          &the_read_pair) = 0;
};


class ReadParser: public ReadInputStream, public ReadIO<SeqAnReadWrapper>
{
public:
    bool
    parse_read                  (Read              &the_read);

    bool
    parse_read_pair             (ReadPair          &the_read_pair);

};


/*******************************************************************************
*                                 ReadWriter                                  *
*******************************************************************************/
class ReadWriter: public ReadOutputStream, public ReadIO<SeqAnWriteWrapper>
{
public:
    void
    write_read                  (Read              &the_read);

    void
    write_read_pair             (ReadPair          &the_read_pair);

};


/*******************************************************************************
*                               ReadInterleaver                               *
*******************************************************************************/
class ReadInterleaver : public ReadInputStream
{
public:
    void
    open                        (const char        *r1_filename,
                                 const char        *r2_filename);

    void
    open                        (const std::string &r1_filename,
                                 const std::string &r2_filename);

    bool
    parse_read_pair             (ReadPair          &the_read_pair);

    bool
    parse_read                  (Read              &the_read);

    size_t
    get_num_reads               ();


private:
    ReadParser          r1_parser;
    ReadParser          r2_parser;
    bool                _last_was_r1=false;
};


/*******************************************************************************
*                              ReadDeInterleaver                              *
*******************************************************************************/
class ReadDeInterleaver : public ReadOutputStream
{

public:
    ReadDeInterleaver           ();

    void
    open                        (const char        *r1_filename,
                                 const char        *r2_filename);

    void
    open                        (const std::string &r1_filename,
                                 const std::string &r2_filename);
    void
    write_read_pair             (ReadPair          &the_read_pair);

    void
    write_read                  (Read              &the_read);

    size_t
    get_num_reads               ();

private:
    ReadWriter          r1_writer;
    ReadWriter          r2_writer;
    bool                _last_was_r1=false;
};


} // namespace qcpp

#endif /* QC_IO_HH */
