/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
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
