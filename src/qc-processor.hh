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


namespace qcpp
{

class ReadProcessor
{
public:
    ReadProcessor                   (const std::string &name);

    virtual void
    process_read                    (Read              &the_read) = 0;

    virtual void
    process_read_pair               (ReadPair          &the_read_pair) = 0;

    virtual std::string
    report                          () = 0;

protected:
    std::atomic_ullong      _num_reads;
    const std::string       _name;
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
        //_pipeline.push_back(std::make_unique<ReadProcType>(args...));
        _pipeline.push_back(
                std::unique_ptr<ReadProcType>(new ReadProcType(args...)));
    }

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

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


} // namespace qcpp

#endif /* QC_PROCESSOR_HH */
