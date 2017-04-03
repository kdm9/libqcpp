/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_LENGTH_HH
#define QC_LENGTH_HH

#include <map>
#include "qc-processor.hh"

namespace qcpp
{


class ReadLenCounter: public ReadProcessor
{
public:
    ReadLenCounter                  (const std::string &name,
                                     const QualityEncoding &encoding=SangerEncoding);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    virtual void
    add_stats_from                  (ReadProcessor     *other);

    std::string
    yaml_report                     ();

private:
    bool                    _have_r2;
    size_t                  _max_len;
    std::map<size_t, size_t> _len_map_r1;
    std::map<size_t, size_t> _len_map_r2;
};


class ReadLenFilter: public ReadProcessor
{
public:
    ReadLenFilter                   (const std::string &name,
                                     size_t             threshold=1,
                                     const QualityEncoding &encoding=SangerEncoding);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessor     *other_ptr);

    std::string
    yaml_report                     ();

private:
    size_t                  _num_r1_dropped;
    size_t                  _num_r2_dropped;
    size_t                  _num_pairs_dropped;
    size_t                  _threshold;
};

class ReadTruncator: public ReadProcessor
{
public:
    ReadTruncator                   (const std::string &name,
                                     size_t             threshold=64,
                                     const QualityEncoding &encoding=SangerEncoding);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessor     *other_ptr);

    std::string
    yaml_report                     ();

private:
    size_t                  _num_r1_dropped;
    size_t                  _num_r2_dropped;
    size_t                  _num_pairs_dropped;
    size_t                  _threshold;
};


} // end namespace qcpp

#endif /* QC_LENGTH_HH */
