/*
 * ============================================================================
 *
 *       Filename:  qc-measure.hh
 *    Description:  Collect measures of read quality
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
    add_stats_from                  (PerBaseQuality    &other);

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
