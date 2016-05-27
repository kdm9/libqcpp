/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_MEASURE_HH
#define QC_MEASURE_HH

#include <map>
#include <array>
#include "qc-processor.hh"

namespace qcpp
{

typedef std::map<int8_t, size_t> PhredHistogram;

class PerBaseQuality: public ReadProcessor
{
public:
    PerBaseQuality                  (const std::string &name,
                                     const QualityEncoding &encoding=SangerEncoding);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessor     *other);

    std::string
    yaml_report                     ();

private:
    bool                    _have_r2;
    size_t                  _max_len;
    std::vector<PhredHistogram> _qual_scores_r1;
    std::vector<PhredHistogram> _qual_scores_r2;
};


} // end namespace qcpp

#endif /* QC_MEASURE_HH */
