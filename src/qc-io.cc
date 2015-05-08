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
 *                                    Read
 *****************************************************************************/

void
Read::
clear()
{
    name.clear( );
    sequence.clear( );
    quality.clear( );
}

size_t
Read::
size() const
{
    return sequence.size();
}

std::string
Read::
str() const
{
    std::ostringstream oss;

    if (name.size() == 0 || sequence.size() == 0) {
        return "";
    }
    if (quality.size() > 0) {
        oss << "@";
    } else {
        oss << ">";
    }
    oss << name << "\n";
    oss << sequence << "\n";
    if (quality.size() > 0) {
        oss << "+\n";
        oss << quality << "\n";
    }

    return oss.str();
}

void
Read::
erase(size_t pos)
{
    sequence.erase(pos);
    quality.erase(pos);
}

bool
operator==(const Read &r1, const Read &r2)
{
    return r1.name == r2.name && \
           r1.sequence == r2.sequence && \
           r1.quality == r2.quality;
}

std::string
ReadPair::
str()
{
    std::ostringstream oss;
    bool fastq = first.quality.size() > 0 || second.quality.size() > 0;

    if (first.name.size() == 0 || second.name.size() == 0 ||
            (first.sequence.size() == 0 && second.quality.size() == 0)) {
        return "";
    }

    if (first.sequence.size() == 0) {
        // Make a fake record of a single N, to avoid breaking pairing.
        if (fastq) {
            oss << "@";
        } else {
            oss << ">";
        }
        oss << first.name << "\n";
        oss << "N\n";
        if (fastq) {
            oss << "+\n";
            // 'B' is the lowest quality score that is valid in all encodings.
            // See https://en.wikipedia.org/wiki/FASTQ_format#Encoding
            oss << "B\n";
        }
    } else {
        oss << first.str();
    }

    if (second.sequence.size() == 0) {
        // Make a fake record of a single N, to avoid breaking pairing.
        if (fastq) {
            oss << "@";
        } else {
            oss << ">";
        }
        oss << second.name << "\n";
        oss << "N\n";
        if (fastq) {
            oss << "+\n";
            // 'B' is the lowest quality score that is valid in all encodings.
            // See https://en.wikipedia.org/wiki/FASTQ_format#Encoding
            oss << "B\n";
        }
    } else {
        oss << second.str();
    }

    return oss.str();

}

bool
operator==(const ReadPair &r1, const ReadPair &r2)
{
    return r1.first == r2.first && r1.second == r2.second;
}

/*****************************************************************************
 *                               SeqAn Wrapper
 *****************************************************************************/

struct SeqAnReadWrapper
{
    seqan::SeqFileIn stream;
    std::mutex _mutex;

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
};

struct SeqAnWriteWrapper
{
    seqan::SeqFileOut stream;
    std::mutex _mutex;

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
size_t
ReadIO<SeqAnWrapper>::
get_num_reads()
{
    return _num_reads;
}


/*****************************************************************************
 *                                 READERS
 *****************************************************************************/

bool
ReadInputStream::
at_end()
{
    return _at_end;
}

ReadInputStream::
ReadInputStream()
{
    _at_end = false;
}

ReadInputStream::
ReadInputStream(const ReadInputStream &other)
{
    _at_end = other._at_end;
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
        std::lock_guard<std::mutex> lock(_private->_mutex);
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
        std::lock_guard<std::mutex> lock(_pair_mutex);
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

ReadInterleaver::
ReadInterleaver()
{
    _num_pairs = 0;
}

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
        _num_pairs++;
        return true;
    }
    the_read_pair.first.clear();
    the_read_pair.second.clear();
    return false;
}

size_t
ReadInterleaver::
get_num_reads()
{
    return _num_pairs * 2;
}

size_t
ReadInterleaver::
get_num_pairs()
{
    return _num_pairs;
}


/*****************************************************************************
 *                                 WRITERS
 *****************************************************************************/

ReadOutputStream::
ReadOutputStream()
{
}

ReadOutputStream::
ReadOutputStream(const ReadOutputStream &other)
{
}

void
ReadWriter::
write_read(Read &the_read)
{
    the_read.clear();
    const char *exception = NULL;
    {
        std::lock_guard<std::mutex> lg(_private->_mutex);
        try {
            seqan::writeRecord(_private->stream, the_read.name,
                               the_read.sequence, the_read.quality);
            _num_reads++;
        } catch (seqan::IOError &err) {
            exception = err.what();
        } catch (seqan::ParseError &err) {
            exception = err.what();
        }
    }
    // Throw any error in the read, even if we're at the end
    if (exception != NULL) {
        throw IOError(exception);
    }
}

void
ReadWriter::
write_read_pair(ReadPair &the_read_pair)
{
    std::lock_guard<std::mutex> lg(_pair_mutex);
    write_read(the_read_pair.first);
    write_read(the_read_pair.second);
}

ReadDeInterleaver::
ReadDeInterleaver()
{
    _num_pairs = 0;
}


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
    _num_pairs++;
}

size_t
ReadDeInterleaver::
get_num_reads()
{
    return _num_pairs * 2;
}

size_t
ReadDeInterleaver::
get_num_pairs()
{
    return _num_pairs;
}

template class ReadIO<SeqAnReadWrapper>;
template class ReadIO<SeqAnWriteWrapper>;

} // namespace qcpp
