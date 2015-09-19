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
ReadProcessor(const std::string &name, const QualityEncoding &encoding)
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

void
ReadProcessorPipeline::
add_stats_from(ReadProcessorPipeline &other)
{
    for (size_t i = 0; i < _pipeline.size(); i++) {
        _pipeline[i]->add_stats_from(other._pipeline[i].get());
    }
}

std::string
ReadProcessorPipeline::
report()
{
    std::ostringstream ss;
    ss << global_report_yaml_header();
    for (auto &proc: _pipeline) {
        ss << proc->yaml_report();
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

/////////////////////////////  ThreadedQCProcessor ////////////////////////////

ThreadedQCProcessor::
ThreadedQCProcessor(std::string &input, std::ostream *output,
                    size_t worker_threads)
    : _num_reads(0)
    , _output(output)
    , _num_threads(worker_threads)
    , _input_complete(false)
    , _output_complete(0)
    , _chunksize(8192)
{
    _input.open(input);
    for (size_t i = 0; i < _num_threads; i++) {
        _pipelines.emplace_back();
    }
}

void
ThreadedQCProcessor::
writer(ThreadedQCProcessor *self)
{
    while (true) {
        std::unique_lock<std::mutex> lock(self->_out_mutex);
        while (self->_out_queue.empty()) {
            if (self->_output_complete == self->_num_threads) {
                return;
            }
            self->_out_cv.wait_for(lock, std::chrono::microseconds(1));
        }
        ReadChunk chunk = self->_out_queue.front();
        self->_out_queue.pop();

        lock.unlock();

        for (ReadPair &rp: chunk) {
            (*self->_output) <<  rp.str();
        }
        self->_num_reads += chunk.size();
        if (self->_progress_cb) {
            self->_progress_cb(self->_num_reads);
        }
    }
}

void
ThreadedQCProcessor::
worker(ThreadedQCProcessor *self, size_t thread_id)
{
    ReadProcessorPipeline &pipeline = self->_pipelines[thread_id];
    while (true) {
        std::unique_lock<std::mutex> lock(self->_in_mutex);
        while (self->_in_queue.empty()) {
            if (self->_input_complete) {
                self->_output_complete++;
                return;
            }
            self->_out_cv.wait_for(lock, std::chrono::microseconds(1));
        }
        ReadChunk chunk(self->_in_queue.front());
        self->_in_queue.pop();

        lock.unlock();

        for (ReadPair &rp: chunk) {
            pipeline.process_read_pair(rp);
        }

        {
            std_mutex_lock lg(self->_out_mutex);
            self->_out_queue.emplace(chunk);
        }
        self->_out_cv.notify_one();
    }
}

void
ThreadedQCProcessor::
reader(ThreadedQCProcessor *self)
{
    while (true) {
        ReadChunk   chunk;
        while (chunk.size() < self->_chunksize) {
            ReadPair    rp;
            if (!self->_input.parse_read_pair(rp))  {
                self->_input_complete = true;
                break;
            }
            chunk.emplace_back(rp);
        }
        {
            std_mutex_lock lg(self->_in_mutex);
            self->_in_queue.emplace(chunk);
        }
        self->_in_cv.notify_one();
        while (self->_in_queue.size() > 2 * self->_num_threads) {
            // Avoid reader racing ahead of workers.
            std::this_thread::sleep_for(std::chrono::microseconds(1));
        }
        if (self->_input_complete) break;
    }
}

size_t
ThreadedQCProcessor::
run()
{
    std::thread rdr(ThreadedQCProcessor::reader, this);
    std::thread wtr(ThreadedQCProcessor::writer, this);
    std::vector<std::thread> workers;

    for (size_t i = 0; i < _num_threads; i++) {
        workers.emplace_back(ThreadedQCProcessor::worker, this, i);
    }

    rdr.join();
    for (auto &thr: workers) {
        thr.join();
    }
    wtr.join();

    for (size_t i = 1; i < _num_threads; i++) {
        _pipelines[0].add_stats_from(_pipelines[i]);
    }
    return _num_reads;
}

void
ThreadedQCProcessor::
set_progress_callback(std::function<void(size_t)> func)
{
    _progress_cb = func;
}

std::string
ThreadedQCProcessor::
report()
{
    return _pipelines[0].report();
}


} // namespace qcpp
