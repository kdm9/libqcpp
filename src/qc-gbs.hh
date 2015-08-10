/*
 * ============================================================================
 *
 *       Filename:  qc-gbs.hh
 *    Description:  Trim reads from Genotyping-by-Sequcing protocols
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

class GBSTrimPE: public ReadProcessor
{
public:
    GBSTrimPE                       (const std::string &name,
                                     const std::string &re_site,
                                     int                overhang_pos);

    void
    process_read_pair               (ReadPair          &the_read_pair);

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
