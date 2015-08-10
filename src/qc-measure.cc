/*
 * ============================================================================
 *
 *       Filename:  qc-measure.cc
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

#include <yaml-cpp/yaml.h>

#include "qc-measure.hh"

namespace qcpp
{

/////////////////////////////////// BASE CLASS ////////////////////////////////
QCMeasure::
QCMeasure(const std::string &name, unsigned phred_offset):
    ReadProcessor(name),
    _have_r2(false),
    _phred_offset(phred_offset)
{
}


/////////////////////////////// PerBaseQuality /////////////////////////
PerBaseQuality::
PerBaseQuality(const std::string &name, unsigned phred_offset):
    QCMeasure(name, phred_offset),
    _max_len(0)
{
}

void
PerBaseQuality::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len > _max_len) {
        std::lock_guard<std::mutex> lg(_expansion_mutex);
        for (size_t i = _max_len; i <= read_len; i++) {
            _qual_scores_r1.emplace_back();
        }
        _max_len = read_len;
    }

    for (size_t i = 0; i < read_len; i++) {
        size_t qual_score = the_read.quality[i] - _phred_offset;
        // It's a kludge, but we have to use uint64_t and to sync_add here, as
        // std::atomic doesn't like being in a std::array
        __sync_add_and_fetch(&_qual_scores_r1[i][qual_score], 1);
    }
    _num_reads++;
}

void
PerBaseQuality::
process_read_pair(ReadPair &the_read_pair)
{
    size_t read_len1 = the_read_pair.first.size();
    size_t read_len2 = the_read_pair.second.size();
    const Read &r1 = the_read_pair.first;
    const Read &r2 = the_read_pair.second;
    size_t larger_len = read_len1 > read_len2 ? read_len1 : read_len2;

    _have_r2 = true;
    if (larger_len > _max_len) {
        std::lock_guard<std::mutex> lg(_expansion_mutex);
        for (size_t i = _max_len + 1; i <= larger_len; i++) {
            _qual_scores_r1.emplace_back();
            _qual_scores_r2.emplace_back();
        }
        _max_len = larger_len;
    }
    for (size_t i = 0; i < read_len1; i++) {
        size_t qual_score = r1.quality[i] - _phred_offset;
        // It's a kludge, but we have to use uint64_t and to sync_add here, as
        // std::atomic doesn't like being in a std::array
        _qual_scores_r1[i][qual_score]++;
        //__sync_add_and_fetch(&_qual_scores_r1[i][qual_score], 1);
    }
    for (size_t i = 0; i < read_len2; i++) {
        size_t qual_score = r2.quality[i] - _phred_offset;
        _qual_scores_r2[i][qual_score]++;
        //__sync_add_and_fetch(&_qual_scores_r2[i][qual_score], 1);
    }
    _num_reads += 2;
}

std::string
PerBaseQuality::
report()
{
    using namespace YAML;
    std::ostringstream ss;
    YAML::Emitter yml;

    yml << BeginSeq;
    yml << BeginMap;
    yml << Key << "PerBaseQuality"
        << Value
        << BeginMap
        << Key   << "name"
        << Value << _name
        << Key   << "parameters"
        << Value << BeginMap
                 << Key << "phred_offset"
                 << Value << _phred_offset
                 << EndMap
        << Key   << "output"
        << Value << BeginMap
                 << Key << "num_reads"
                 << Value << _num_reads
                 << Key << "r1_phred_scores"
                 << Value << BeginSeq;
    // Handle R1 phred scores
    for (size_t i = 0; i < _max_len; i++) {
        yml << Flow;
        yml << BeginSeq;
        for (size_t j = 0; j < _qual_scores_r1[i].size(); j++) {
            yml << _qual_scores_r1[i][j];
        }
        yml << EndSeq;
    }
    yml                   << EndSeq; // End of r1_phred_scores
    yml          << Key << "r2_phred_scores"
                 << Value << BeginSeq;
    if (_have_r2) {
        // Handle R2 phred scores
        for (size_t i = 0; i < _max_len; i++) {
            yml << Flow;
            yml << BeginSeq;
            for (size_t j = 0; j < _qual_scores_r2[i].size(); j++) {
                yml << _qual_scores_r2[i][j];
            }
            yml << EndSeq;
        }
    }
    yml                   << EndSeq; // End of r2_phred_scores

    yml          << EndMap
        << EndMap;
    yml << EndMap;
    yml << EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}

} // end namespace qcpp
