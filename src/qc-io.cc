/*
 * ============================================================================
 *
 *       Filename:  qc-io.cc
 *    Description:  Read I/O Operations. Adapted from Khmer
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */


#include "qc-io.hh"

#include <seqan/sequence.h>
#include <seqan/seq_io.h>
#include <seqan/stream.h>

namespace qcpp
{

/*****************************************************************************
 *                               SeqAn Wrapper
 *****************************************************************************/

struct SeqAnReadWrapper
{
    seqan::SeqFileIn stream;
    uint32_t seqan_spin_lock;

    SeqAnReadWrapper()
    {
        seqan_spin_lock = 0;
    }

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
        __asm__ __volatile__ ("" ::: "memory");
        seqan_spin_lock = 0;
    }
};

struct SeqAnWriteWrapper
{
    seqan::SeqFileOut stream;
    uint32_t seqan_spin_lock;

    SeqAnWriteWrapper()
    {
        seqan_spin_lock = 0;
    }

    ~SeqAnWriteWrapper()
    {
        seqan::close(stream);
    }

    void open(const char *filename)
    {
        if (!seqan::open(stream, filename)) {
            std::string message = "Could not open '";
            message = message + filename + "' for writing.";
            throw IOError(message);
        }
        __asm__ __volatile__ ("" ::: "memory");
        seqan_spin_lock = 0;
    }
};


/*****************************************************************************
 *                                BASE CLASS
 *****************************************************************************/

template<typename SeqAnWrapper>
ReadIO<SeqAnWrapper>::ReadIO()
{
    _private = new SeqAnWrapper();
}

template<typename SeqAnWrapper>
ReadIO<SeqAnWrapper>::~ReadIO()
{
    delete _private;
}

template<typename SeqAnWrapper>
void
ReadIO<SeqAnWrapper>::open(const std::string &filename)
{
    _private->open(filename.c_str());
}

template<typename SeqAnWrapper>
void
ReadIO<SeqAnWrapper>::open(const char *filename)
{
    _private->open(filename);
}

template<typename SeqAnWrapper>
size_t
ReadIO<SeqAnWrapper>::
get_num_reads()
{
    return _num_reads;
}


/*****************************************************************************
 *                                 READERS
 *****************************************************************************/

bool ReadParser::parse_read(Read &the_read)
{
    the_read.clear();
    const char *exception = NULL;
    while (!__sync_bool_compare_and_swap(&_private->seqan_spin_lock, 0, 1));
    bool atEnd = seqan::atEnd(_private->stream);
    if (!atEnd) {
        try {
            seqan::readRecord(the_read.name, the_read.sequence,
                              the_read.quality, _private->stream);
            _num_reads++;
        } catch (seqan::IOError &err) {
            exception = err.what();
        } catch (seqan::ParseError &err) {
            exception = err.what();
        }
    }
    __asm__ __volatile__ ("" ::: "memory");
    _private->seqan_spin_lock = 0;
    // Throw any error in the read, even if we're at the end
    if (exception != NULL) {
        throw IOError(exception);
    }
    if (atEnd) {
        return false;
    }
    return true;
}

bool ReadParser::parse_read_pair(ReadPair &the_read_pair)
{
    bool first = parse_read(the_read_pair.first);
    bool second = parse_read(the_read_pair.second);
    if (!first || !second) {
        the_read_pair.first.clear();
        the_read_pair.second.clear();
        return false;
    }
    return true;
}

void
ReadInterleaver::open(const char *r1_filename, const char *r2_filename)
{
    r1_parser.open(r1_filename);
    r2_parser.open(r2_filename);
}

void
ReadInterleaver::open(const std::string &r1_filename,
                      const std::string &r2_filename)
{
    r1_parser.open(r1_filename);
    r2_parser.open(r2_filename);
}

bool ReadInterleaver::parse_read_pair(ReadPair &the_read_pair)
{
    bool first = r1_parser.parse_read(the_read_pair.first);
    bool second = r2_parser.parse_read(the_read_pair.second);
    if (!first || !second) {
        the_read_pair.first.clear();
        the_read_pair.second.clear();
        return false;
    }
    return true;
}

/*****************************************************************************
 *                                 WRITERS
 *****************************************************************************/


void ReadWriter::write_read(Read &the_read)
{
    the_read.clear();
    const char *exception = NULL;
    while (!__sync_bool_compare_and_swap(&_private->seqan_spin_lock, 0, 1));
    try {
        seqan::writeRecord(_private->stream, the_read.name,
                           the_read.sequence, the_read.quality);
        _num_reads++;
    } catch (seqan::IOError &err) {
        exception = err.what();
    } catch (seqan::ParseError &err) {
        exception = err.what();
    }
    __asm__ __volatile__ ("" ::: "memory");
    _private->seqan_spin_lock = 0;
    // Throw any error in the read, even if we're at the end
    if (exception != NULL) {
        throw IOError(exception);
    }
}

void ReadWriter::write_read_pair(ReadPair &the_read_pair)
{
    write_read(the_read_pair.first);
    write_read(the_read_pair.second);
}

void
ReadDeInterleaver::open(const char *r1_filename, const char *r2_filename)
{
    r1_writer.open(r1_filename);
    r2_writer.open(r2_filename);
}

void
ReadDeInterleaver::open(const std::string &r1_filename,
                      const std::string &r2_filename)
{
    r1_writer.open(r1_filename);
    r2_writer.open(r2_filename);
}

void ReadDeInterleaver::write_read_pair(ReadPair &the_read_pair)
{
    r1_writer.write_read(the_read_pair.first);
    r2_writer.write_read(the_read_pair.second);
}

template class ReadIO<SeqAnReadWrapper>;
template class ReadIO<SeqAnWriteWrapper>;

} // namespace qcpp
