/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */

#include "qc-ds.hh"

namespace qcpp
{

/*****************************************************************************
 *                                    Read
 *****************************************************************************/
Read::
Read()
{
}

Read::
Read(const std::string &name_, const std::string &sequence_,
     const std::string &quality_):
    name(name_),
    sequence(sequence_),
    quality(quality_)
{
}

void
Read::
clear()
{
    name.clear( );
    sequence.clear( );
    quality.clear( );
}

size_t
Read::
size() const
{
    return sequence.size();
}

std::string
Read::
str() const
{
    std::ostringstream oss;

    if (name.size() == 0 || sequence.size() == 0) {
        return "";
    }
    if (quality.size() > 0) {
        oss << "@";
    } else {
        oss << ">";
    }
    oss << name << "\n";
    oss << sequence << "\n";
    if (quality.size() > 0) {
        oss << "+\n";
        oss << quality << "\n";
    }

    return oss.str();
}

void
Read::
erase(size_t pos)
{
    sequence.erase(pos);
    quality.erase(pos);
}

void
Read::
erase(size_t pos, size_t count)
{
    sequence.erase(pos, count);
    quality.erase(pos, count);
}

bool
operator==(const Read &r1, const Read &r2)
{
    return r1.name == r2.name && \
           r1.sequence == r2.sequence && \
           r1.quality == r2.quality;
}

ReadPair::
ReadPair():
    std::pair<Read, Read>()
{
}

ReadPair::
ReadPair(const std::string &name1, const std::string &sequence1,
         const std::string &quality1, const std::string &name2,
         const std::string &sequence2, const std::string &quality2):
    first(name1, sequence1, quality1),
    second(name2, sequence2, quality2)
{
}

std::string
ReadPair::
str()
{
    std::ostringstream oss;
    bool fastq = first.quality.size() > 0 || second.quality.size() > 0;

    if (first.name.size() == 0 || second.name.size() == 0 ||
            (first.sequence.size() == 0 && second.quality.size() == 0)) {
        return "";
    }

    if (first.sequence.size() == 0) {
        // Make a fake record of a single N, to avoid breaking pairing.
        if (fastq) {
            oss << "@";
        } else {
            oss << ">";
        }
        oss << first.name << "\n";
        oss << "N\n";
        if (fastq) {
            oss << "+\n";
            // 'B' is the lowest quality score that is valid in all encodings.
            // See https://en.wikipedia.org/wiki/FASTQ_format#Encoding
            oss << "B\n";
        }
    } else {
        oss << first.str();
    }

    if (second.sequence.size() == 0) {
        // Make a fake record of a single N, to avoid breaking pairing.
        if (fastq) {
            oss << "@";
        } else {
            oss << ">";
        }
        oss << second.name << "\n";
        oss << "N\n";
        if (fastq) {
            oss << "+\n";
            // 'B' is the lowest quality score that is valid in all encodings.
            // See https://en.wikipedia.org/wiki/FASTQ_format#Encoding
            oss << "B\n";
        }
    } else {
        oss << second.str();
    }

    return oss.str();
}
bool
operator==(const ReadPair &r1, const ReadPair &r2)
{
    return r1.first == r2.first && r1.second == r2.second;
}

} /* end namespace qcpp */
