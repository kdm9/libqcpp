/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */



#include "qc-processor.hh"

namespace qcpp
{

class GBSTrimPE: public ReadProcessor
{
public:
    GBSTrimPE                       (const std::string &name,
                                     const std::string &re_site,
                                     int                overhang_pos,
                                     const QualityEncoding &enc=SangerEncoding);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    void
    add_stats_from                  (GBSTrimPE         &other);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_pairs_trimmed;
    const std::string       _re_site;
    std::string             _re_key;
    const size_t            _overhang_pos;
    size_t                  _key_length;

    void
    process_read                    (Read              &the_read)
    {} // "Deleted" Pure Virtual function

};

} // namespace qcpp
