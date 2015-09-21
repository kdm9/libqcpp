/*
 * ============================================================================
 *
 *       Filename:  qc-adaptor.hh
 *    Description:  Trim adaptor read-through from reads
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

#ifndef QC_ADAPTOR_HH
#define QC_ADAPTOR_HH

#include "qc-processor.hh"

namespace qcpp
{

class AdaptorTrimPE: public ReadProcessor
{
public:
    AdaptorTrimPE                   (const std::string &name,
                                     int                min_overlap,
                                     const QualityEncoding &encoding=SangerEncoding);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (ReadProcessor     *other);

    std::string
    yaml_report                     ();

private:
    std::atomic_ullong      _num_pairs_trimmed;
    std::atomic_ullong      _num_pairs_joined;
    int                     _min_overlap;

    void
    process_read                    (Read              &the_read)
    {} // "Deleted" Pure Virtual function

};

} // namespace qcpp

#endif /* QC_ADAPTOR_HH */
