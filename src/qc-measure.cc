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

/////////////////////////////// PerBaseQuality /////////////////////////
PerBaseQuality::
PerBaseQuality(const std::string &name, QualityEncoding encoding)
    : ReadProcessor(name, encoding)
    , _have_r2(false)
    , _max_len(0);
{
}


PerBaseQuality::Report
PerBaseQuality::
report()
{
    return _report;
}

PerBaseQuality::Report
PerBaseQuality::
consolidate_reports(std::vector<PerBaseQuality::Report> reports)
{
    Report new_rep;

    for (const auto &report: reports) {
        new_rep.name = report.name;
        while (new_rep.qual_scores_r1.size() < report.qual_scores_r1.size()) {
            new_rep.emplace_back()
        }
        while (new_rep.qual_scores_r2.size() < report.qual_scores_r2.size()) {
            new_rep.emplace_back()
        }
        for (const auto &pair: report.qual_scores_r1) {
            new_rep.qual_scores_r1[pair.first] += pair.second;
        }
    }
    return _report;
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
            _report.qual_scores_r1.emplace_back();
            _report.qual_scores_r2.emplace_back();
        }
        _max_len = larger_len;
    }
    for (size_t i = 0; i < read_len1; i++) {
        size_t qual_score = r1.quality[i] - _phred_offset;
        _report.qual_scores_r1[i][qual_score]++;
    }
    for (size_t i = 0; i < read_len2; i++) {
        size_t qual_score = r2.quality[i] - _phred_offset;
        _report.qual_scores_r2[i][qual_score]++;
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
                 << Value << _report.num_reads
                 << Key << "r1_phred_scores"
                 << Value << BeginSeq;
    // Handle R1 phred scores
    for (size_t i = 0; i < _max_len; i++) {
        yml << Flow;
        yml << BeginSeq;
        for (size_t j = 0; j < _report.qual_scores_r1[i].size(); j++) {
            yml << _report.qual_scores_r1[i][j];
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
            for (size_t j = 0; j < _report.qual_scores_r2[i].size(); j++) {
                yml << _report.qual_scores_r2[i][j];
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
