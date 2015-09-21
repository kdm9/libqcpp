/*
 * ============================================================================
 *
 *       Filename:  qc-processor.hh
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

#ifndef QC_PROCESSOR_HH
#define QC_PROCESSOR_HH

#include "qc-config.hh"
#include "qc-util.hh"
#include "qc-io.hh"
#include "qc-quality.hh"


#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>


namespace qcpp
{

class ReadProcessor
{
public:
    ReadProcessor                   (const std::string &name,
                                     const QualityEncoding &encoding);

    virtual void
    process_read                    (Read              &the_read) = 0;

    virtual void
    process_read_pair               (ReadPair          &the_read_pair) = 0;

    virtual void
    add_stats_from                  (ReadProcessor     *other) = 0;

    virtual std::string
    yaml_report                     () = 0;

protected:
    const std::string       _name;
    size_t                  _num_reads;
    const QualityEncoding   _encoding;
};


class ReadProcessorPipeline
{
public:
    ReadProcessorPipeline           ();
    ReadProcessorPipeline           (ReadProcessorPipeline &&other);

    template<typename ReadProcType, class ...  Args>
    void
    append_processor                (Args&&...          args)
    {
        _pipeline.push_back(
                std::unique_ptr<ReadProcType>(new ReadProcType(args...)));
    }

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessorPipeline &other);

    std::string
    report                          ();

protected:
    std::vector<std::unique_ptr<ReadProcessor>> _pipeline;
};


class ProcessedReadStream: public ReadInputStream
{
public:
    ProcessedReadStream             ();
    ProcessedReadStream             (const std::string &filename);

    void
    open                            (const char        *filename);

    void
    open                            (const std::string &filename);

    bool
    parse_read                      (Read              &the_read);

    bool
    parse_read_pair                 (ReadPair          &the_read_pair);

    template<typename ReadProcType, class ...  Args>
    void
    append_processor                (Args&&...          args)
    {
        _pipeline.append_processor<ReadProcType>(args...);
    }

    std::string
    report                          ();

protected:
    ReadParser              _parser;
    ReadProcessorPipeline   _pipeline;
};

/////////////////////////////  ThreadedQCProcessor ////////////////////////////

class ThreadedQCProcessor
{
    typedef std::vector<ReadPair> ReadChunk;
public:
    ThreadedQCProcessor             (std::string        &input,
                                     std::ostream       *output,
                                     size_t              worker_threads=1);

    template<typename ReadProcType, class ...  Args>
    void
    append_processor                (Args&&...          args)
    {
        for (auto &pipeline: _pipelines) {
            pipeline.append_processor<ReadProcType>(args...);
        }
    }

    void
    set_progress_callback           (std::function<void(size_t)> func);

    size_t
    run                             ();

    std::string
    report                          ();

    static void reader(ThreadedQCProcessor *self);
    static void worker(ThreadedQCProcessor *self, size_t thread_id);
    static void writer(ThreadedQCProcessor *self);

protected:
    size_t                  _num_reads;
    // One pipeline per thread
    std::vector<ReadProcessorPipeline> _pipelines;
    ReadParser              _input;
    std::ostream           *_output;
    std::condition_variable _in_cv;
    std::condition_variable _out_cv;
    std::mutex              _in_mutex;
    std::mutex              _out_mutex;
    std::queue<ReadChunk>   _in_queue;
    std::queue<ReadChunk>   _out_queue;
    size_t                  _num_threads;
    bool                    _input_complete;
    size_t                  _output_complete;
    std::function<void(size_t)> _progress_cb;

private:
    const size_t            _chunksize;
};

} // namespace qcpp

#endif /* QC_PROCESSOR_HH */
