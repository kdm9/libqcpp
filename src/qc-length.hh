/*
 * ============================================================================
 *
 *       Filename:  qc-length.hh
 *    Description:  Filter on and count read lengths
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

#ifndef QC_LENGTH_HH
#define QC_LENGTH_HH

#include <map>
#include "qc-processor.hh"

namespace qcpp
{


class ReadLenCounter: public ReadProcessor
{
public:
    ReadLenCounter                  (const std::string &name);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    bool                    _have_r2;
    size_t                  _max_len;
    std::map<size_t, std::atomic_ullong> _len_map_r1;
    std::map<size_t, std::atomic_ullong> _len_map_r2;
    std::mutex              _map_mutex;
};


class ReadLenFilter: public ReadProcessor
{
public:
    ReadLenFilter                   (const std::string &name,
                                     size_t             threshold = 1);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_r1_trimmed;
    std::atomic_ullong      _num_r2_trimmed;
    std::atomic_ullong      _num_pairs_trimmed;
    size_t                  _threshold;
};


} // end namespace qcpp

#endif /* QC_LENGTH_HH */
