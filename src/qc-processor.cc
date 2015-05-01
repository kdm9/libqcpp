/*
 * ============================================================================
 *
 *       Filename:  qc-processor.cc
 *    Description:  Base class that handles processing of reads and aggregation
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include "qc-processor.hh"

namespace qcpp
{

ReadProcessor::
ReadProcessor()
{
    _num_reads = 0;
}

ReadProcessorPipeline::
ReadProcessorPipeline()
{

}

ReadProcessorPipeline::
ReadProcessorPipeline(ReadProcessorPipeline &&other)
{
    _pipeline = std::move(other._pipeline);

}

void
ReadProcessorPipeline::
process_read(Read &the_read)
{
    for (auto &proc: _pipeline) {
        proc->process_read(the_read);
    }

}

void
ReadProcessorPipeline::
process_read_pair(ReadPair &the_read_pair)
{
    for (auto &proc: _pipeline) {
        proc->process_read_pair(the_read_pair);
    }
}

std::string
ReadProcessorPipeline::
report()
{
    std::ostringstream ss;
    for (auto &proc: _pipeline) {
        ss << proc->report();
    }
    return ss.str();
}

ProcessedReadStream::
ProcessedReadStream()
{
}

void
ProcessedReadStream::
open(const char *filename)
{
    _parser.open(filename);
}

void
ProcessedReadStream::
open(const std::string &filename)
{
    _parser.open(filename);
}

bool
ProcessedReadStream::
parse_read(Read &the_read)
{
    Read our_read;

    the_read.clear();

    while (the_read.size() == 0) {
        if (!_parser.parse_read(our_read))  {
            return false;
        }
        _pipeline.process_read(our_read);
        the_read = our_read;
    }
    return true;
}

bool
ProcessedReadStream::
parse_read_pair(ReadPair &the_read_pair)
{
    ReadPair our_pair;

    the_read_pair.first.clear();
    the_read_pair.second.clear();

    while (the_read_pair.first.size() == 0 &&
           the_read_pair.second.size() == 0) {
        if (! _parser.parse_read_pair(our_pair)) {
            return false;
        }
        _pipeline.process_read_pair(our_pair);
        the_read_pair = our_pair;
    }
    return true;
}


std::string
ProcessedReadStream::
report()
{
    return _pipeline.report();
}

} // namespace qcpp
