/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#include "qc-io.hh"

#include <seqan/sequence.h>
#include <seqan/seq_io.h>
#include <seqan/stream.h>

namespace qcpp
{

/*******************************************************************************
*                                Seqan Wrapper                                *
*******************************************************************************/
struct SeqAnReadWrapper
{
    seqan::SeqFileIn stream;
    //std::mutex _mutex;

    ~SeqAnReadWrapper()
    {
        seqan::close(stream);
    }

    void open(const char *filename)
    {
        if (!seqan::open(stream, filename)) {
            std::string message = "Could not open '";
            message = message + filename + "' for reading.";
            throw IOError(message);
        } else if (seqan::atEnd(stream)) {
            std::string message = "File '";
            message = message + filename + "' does not contain any sequences!";
            throw IOError(message);
        }
    }

    void close()
    {
        seqan::close(stream);
    }
};


struct SeqAnWriteWrapper
{
    seqan::SeqFileOut stream;
    //std::mutex _mutex;

    ~SeqAnWriteWrapper()
    {
        this->close();
    }

    void open(const char *filename)
    {
        using seqan::OPEN_CREATE;
        using seqan::OPEN_WRONLY;
        if (!seqan::open(stream, filename, OPEN_CREATE | OPEN_WRONLY)) {

            std::string message = "Could not open '";
            message = message + filename + "' for writing. asdfs";
            throw IOError(message);
        }
    }

    void close()
    {
        seqan::close(stream);
    }
};


/*****************************************************************************
 *                                BASE CLASS
 *****************************************************************************/
template<typename SeqAnWrapper>
ReadIO<SeqAnWrapper>::
ReadIO()
{
    _private = new SeqAnWrapper();
    _num_reads = 0;
    _has_qual = false;
}

template<typename SeqAnWrapper>
ReadIO<SeqAnWrapper>::
~ReadIO()
{
    delete _private;
}

template<typename SeqAnWrapper>
void
ReadIO<SeqAnWrapper>::
open(const std::string &filename)
{
    _private->open(filename.c_str());
}

template<typename SeqAnWrapper>
void
ReadIO<SeqAnWrapper>::
open(const char *filename)
{
    _private->open(filename);
}

template<typename SeqAnWrapper>
void
ReadIO<SeqAnWrapper>::
close()
{
    _private->close();
}

template<typename SeqAnWrapper>
size_t
ReadIO<SeqAnWrapper>::
get_num_reads()
{
    return _num_reads;
}

template class ReadIO<SeqAnReadWrapper>;
template class ReadIO<SeqAnWriteWrapper>;


/*******************************************************************************
*                                   Readers                                   *
*******************************************************************************/
bool
ReadInputStream::
at_end()
{
    return _at_end;
}


bool
ReadParser::
parse_read(Read &the_read)
{
    the_read.clear();
    const char *exception = NULL;
    bool atEnd;

    // Non-threadsafe block
    {
        //std::lock_guard<std::mutex> lock(_private->_mutex);
        atEnd = seqan::atEnd(_private->stream);
        if (!atEnd) {
            try {
                seqan::readRecord(the_read.name, the_read.sequence,
                                  the_read.quality, _private->stream);
                if (_num_reads == 0 && the_read.quality.size() != 0) {
                    _has_qual = true;
                }
            } catch (seqan::IOError &err) {
                exception = err.what();
            } catch (seqan::ParseError &err) {
                exception = err.what();
            }
        }
    }
    // Check that the lengths are the same
    if (_has_qual && the_read.sequence.size() != the_read.quality.size()) {
        // For some reason this error isn't caught by SeqAn
        exception = "Sequence and Quality lengths differ";
    }
    // Throw any error in the read, even if we're at the end
    if (exception != NULL) {
        throw IOError(exception);
    }
    if (atEnd) {
        return false;
    }
    _num_reads++;
    return true;
}

bool
ReadParser::
parse_read_pair(ReadPair &the_read_pair)
{
    bool first, second;
    {
        //std::lock_guard<std::mutex> lock(_pair_mutex);
        first = parse_read(the_read_pair.first);
        second = parse_read(the_read_pair.second);
    }
    if (!first || !second) {
        the_read_pair.first.clear();
        the_read_pair.second.clear();
        return false;
    }
    return true;
}


/*******************************************************************************
*                               ReadInterleaver                               *
*******************************************************************************/
void
ReadInterleaver::
open(const char *r1_filename, const char *r2_filename)
{
    r1_parser.open(r1_filename);
    r2_parser.open(r2_filename);
}

void
ReadInterleaver::
open(const std::string &r1_filename, const std::string &r2_filename)
{
    r1_parser.open(r1_filename);
    r2_parser.open(r2_filename);
}

bool
ReadInterleaver::
parse_read_pair(ReadPair &the_read_pair)
{
    bool first = r1_parser.parse_read(the_read_pair.first);
    bool second = r2_parser.parse_read(the_read_pair.second);
    if (first && second) {
        return true;
    }
    the_read_pair.first.clear();
    the_read_pair.second.clear();
    return false;
}

bool
ReadInterleaver::
parse_read(Read &the_read)
{
    if (_last_was_r1) {
        _last_was_r1 = false;
        return r2_parser.parse_read(the_read);
    } else {
        _last_was_r1 = true;
        return r1_parser.parse_read(the_read);
    }
}

size_t
ReadInterleaver::
get_num_reads()
{
    return r1_parser.get_num_reads() + r2_parser.get_num_reads();
}


/*******************************************************************************
*                                 ReadWriter                                  *
*******************************************************************************/
void
ReadWriter::
write_read(Read &the_read)
{
    assert(_private != NULL);
    const char *exception = NULL;
    //_private->_mutex.lock();
    try {
        seqan::writeRecord(_private->stream, the_read.name,
                           the_read.sequence, the_read.quality);
        _num_reads++;
    } catch (seqan::IOError &err) {
        exception = err.what();
    } catch (seqan::ParseError &err) {
        exception = err.what();
    }
    //_private->_mutex.unlock();
    // Throw any error in the read, even if we're at the end
    if (exception != NULL) {
        throw IOError(exception);
    }
}

void
ReadWriter::
write_read_pair(ReadPair &the_read_pair)
{
    write_read(the_read_pair.first);
    write_read(the_read_pair.second);
}


/*******************************************************************************
*                              ReadDeInterleaver                              *
*******************************************************************************/
void
ReadDeInterleaver::
open(const char *r1_filename, const char *r2_filename)
{
    r1_writer.open(r1_filename);
    r2_writer.open(r2_filename);
}

void
ReadDeInterleaver::
open(const std::string &r1_filename, const std::string &r2_filename)
{
    r1_writer.open(r1_filename);
    r2_writer.open(r2_filename);
}

void
ReadDeInterleaver::
write_read_pair(ReadPair &the_read_pair)
{
    r1_writer.write_read(the_read_pair.first);
    r2_writer.write_read(the_read_pair.second);
}

void
ReadDeInterleaver::
write_read(Read &the_read)
{
    if (_last_was_r1) {
        _last_was_r1 = false;
        r2_writer.write_read(the_read);
    } else {
        _last_was_r1 = true;
        r1_writer.write_read(the_read);
    }
}

size_t
ReadDeInterleaver::
get_num_reads()
{
    return r1_writer.get_num_reads() + r2_writer.get_num_reads();
}

} // namespace qcpp
