/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_QUALTRIM_HH
#define QC_QUALTRIM_HH

#include "qc-config.hh"
#include "qc-processor.hh"
#include "qc-quality.hh"

namespace qcpp
{


class WindowedQualTrim: public ReadProcessor
{
    // This class implements a similar algorithm to Nik Joshi's sickle tool
public:
    WindowedQualTrim                (const std::string &name,
                                     const QualityEncoding &encoding,
                                     int8_t             min_quality,
                                     size_t             min_length,
                                     size_t             window_size);
    WindowedQualTrim                (const std::string &name,
                                     const QualityEncoding &encoding,
                                     int8_t             min_quality,
                                     size_t             min_length);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessor     *other_ptr);

    std::string
    yaml_report                          ();

private:
    int8_t                  _min_quality;
    size_t                  _min_length;
    size_t                  _window_size;
    size_t                  _num_reads_trimmed;
    size_t                  _num_reads_dropped;
};


} // namespace qcpp


#endif /* QC_QUALTRIM_HH */
