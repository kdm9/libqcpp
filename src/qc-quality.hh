/*
 * ============================================================================
 *
 *       Filename:  qc-quality.hh
 *    Description:  Quality score niceties
 *        License:  LGPLv3+
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

#ifndef QC_QUALITY_HH
#define QC_QUALITY_HH

#include "qc-config.hh"

namespace qcpp
{

struct QualityEncoding {
    const std::string name;
    const int8_t offset;
    const int8_t start;
    const int8_t stop;

    inline int8_t
    p2q(char phred) const
    {
        return phred - offset;
    }
};

static const QualityEncoding SangerEncoding{"Sanger", 33, 0, 40};
static const QualityEncoding SolexaEncoding{"Solexa", 59, -5, 40};
static const QualityEncoding Illumina13Encoding{"Illumina 1.3+", 64, 0, 40};
static const QualityEncoding Illumina15Encoding{"Illumina 1.5+", 64, 2, 40};
static const QualityEncoding Illumina18Encoding{"Illumina 1.8+", 33, 0, 41};


static inline int8_t
qual_of_base                    (const Read &the_read,
                                 const size_t idx,
                                 QualityEncoding &encoding)
{
    return the_read.quality[idx] - encoding.offset;
}

} // end namespace qcpp

#endif /* QC_QUALITY_HH */
