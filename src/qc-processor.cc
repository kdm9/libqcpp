/*
 * ============================================================================
 *
 *       Filename:  qc-processor.cc
 *    Description:  Base read processing and aggregation
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

/* Copyright (c) 2015 Kevin Murray
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "qc-processor.hh"

namespace qcpp
{

ReadProcessor::
ReadProcessor(const std::string &name, QualityEncoding encoding)
    : _name(name)
    , _num_reads(0)
    , _encoding(encoding)
{
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
    ss << global_report_yaml_header();
    for (auto &proc: _pipeline) {
        ss << proc->yaml_report(proc->report());
    }
    return ss.str();
}

//////////////////////////////  ProcessedReadStream ///////////////////////////

ProcessedReadStream::
ProcessedReadStream()
{
}

ProcessedReadStream::
ProcessedReadStream(const std::string &filename)
{
    _parser.open(filename);
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
        if (our_read.size() == 0) {
            continue;
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

    while (our_pair.first.size() == 0 &&
            our_pair.second.size() == 0) {
        if (!_parser.parse_read_pair(our_pair)) {
            return false;
        }
        if (our_pair.first.size() == 0) {
            continue;
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
