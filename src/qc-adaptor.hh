/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#ifndef QC_ADAPTOR_HH
#define QC_ADAPTOR_HH

#include "qc-processor.hh"
#include <tuple>

namespace qcpp
{

class AdaptorTrimPE: public ReadProcessor
{
public:
    AdaptorTrimPE                   (const std::string &name,
                                     int                min_overlap=10,
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
    {std::ignore = the_read;} // "Deleted" Pure Virtual function

};

} // namespace qcpp

#endif /* QC_ADAPTOR_HH */
