/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#include <yaml-cpp/yaml.h>

#include "qc-measure.hh"

namespace qcpp
{

/////////////////////////////// PerBaseQuality /////////////////////////
PerBaseQuality::
PerBaseQuality(const std::string &name, const QualityEncoding &encoding)
    : ReadProcessor(name, encoding)
    , _have_r2(false)
    , _max_len(0)
{
}


void
PerBaseQuality::
add_stats_from(ReadProcessor *other_ptr)
{
    PerBaseQuality &other = *reinterpret_cast<PerBaseQuality *>(other_ptr);

    _num_reads += other._num_reads;

    while (_qual_scores_r1.size() < other._qual_scores_r1.size()) {
        _qual_scores_r1.emplace_back();
    }
    while (_qual_scores_r2.size() < other._qual_scores_r2.size()) {
        _qual_scores_r2.emplace_back();
    }
    for (size_t i = 0, len = _qual_scores_r1.size(); i < len; i++) {
        for (const auto &pair: other._qual_scores_r1[i]) {
            _qual_scores_r1[i][pair.first] += pair.second;
        }
    }
    for (size_t i = 0, len = _qual_scores_r2.size(); i < len; i++) {
        for (const auto &pair: other._qual_scores_r2[i]) {
            _qual_scores_r2[i][pair.first] += pair.second;
        }
    }
}


void
PerBaseQuality::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len > _max_len) {
        for (size_t i = _max_len; i <= read_len; i++) {
            _qual_scores_r1.emplace_back();
        }
        _max_len = read_len;
    }

    for (size_t i = 0; i < read_len; i++) {
        size_t qual_score = _encoding.p2q(the_read.quality[i]);
        _qual_scores_r1[i][qual_score]++;
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
        for (size_t i = _max_len + 1; i <= larger_len; i++) {
            _qual_scores_r1.emplace_back();
            _qual_scores_r2.emplace_back();
        }
        _max_len = larger_len;
    }
    for (size_t i = 0; i < read_len1; i++) {
        size_t qual_score = _encoding.p2q(r1.quality[i]);
        _qual_scores_r1[i][qual_score]++;
    }
    for (size_t i = 0; i < read_len2; i++) {
        size_t qual_score = _encoding.p2q(r2.quality[i]);
        _qual_scores_r2[i][qual_score]++;
    }
    _num_reads += 2;
}

std::string
PerBaseQuality::
yaml_report()
{
    using namespace YAML;
    std::ostringstream ss;
    YAML::Emitter yml;

    yml << BeginSeq;
    yml << BeginMap;
    yml << Key << "PerBaseQuality" << Value;

    yml << BeginMap;
    yml << Key << "name" << Value << _name;
    yml << Key << "parameters"
        << Value << BeginMap
            << Key << "quality_encoding" << Value << _encoding.name
            << EndMap;

    yml << Key << "output"
        << Value << BeginMap
             << Key << "num_reads"
             << Value << _num_reads
             << Key << "r1_phred_scores"
             << Value << BeginSeq;
            // Handle R1 phred scores
            for (size_t i = 0; i < _max_len; i++) {
                yml << Flow << _qual_scores_r1[i];
            }
            yml << EndSeq; // End of r1_phred_scores
            yml << Key << "r2_phred_scores"
                << Value << BeginSeq;
    if (_have_r2) {
        // Handle R2 phred scores
        for (size_t i = 0; i < _max_len; i++) {
            yml << Flow << _qual_scores_r2[i];
        }
    }
    yml << EndSeq; // End of r2_phred_scores

    yml << EndMap
        << EndMap;
    yml << EndMap;  // PerBaseQuality
    yml << EndSeq;  // root
    ss << yml.c_str() << "\n";
    return ss.str();
}

} // end namespace qcpp
