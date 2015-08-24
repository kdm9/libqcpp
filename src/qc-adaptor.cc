/*
 * ============================================================================
 *
 *       Filename:  qc-adaptor.cc
 *    Description:  Trim adaptor read-through from reads
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

#include <seqan/modifier.h>
#include <seqan/align.h>

#include "qc-adaptor.hh"

namespace qcpp
{

AdaptorTrimPE::
AdaptorTrimPE(const std::string &name, int min_overlap):
    ReadProcessor(name),
    _num_pairs_trimmed(0),
    _num_pairs_joined(0),
    _min_overlap(min_overlap)
{
}

void
AdaptorTrimPE::
process_read_pair(ReadPair &the_read_pair)
{
    seqan::Align<std::string, seqan::ArrayGaps> aligner;
    std::string r2_rc = the_read_pair.second.sequence;
    int score = 0;

    seqan::reverseComplement(r2_rc);
    resize(rows(aligner), 2);
    assignSource(row(aligner, 0), the_read_pair.first.sequence);
    assignSource(row(aligner, 1), r2_rc);

    score = seqan::globalAlignment(aligner,
                           seqan::Score<int, seqan::Simple>(1, -3, -3, -3),
                           seqan::AlignConfig<true, true, true, true>());

    if (score >= _min_overlap) {
        size_t r1_len = the_read_pair.first.size();
        size_t r2_len = the_read_pair.second.size();
        ssize_t read_len_diff = r1_len - r2_len;
        size_t r1_start = toViewPosition(row(aligner, 0), 0);
        size_t r2_start = toViewPosition(row(aligner, 1), 0);
        std::string &r1_seq = the_read_pair.first.sequence;
        std::string &r2_seq = the_read_pair.second.sequence;
        std::string &r1_qual = the_read_pair.first.quality;
        std::string &r2_qual = the_read_pair.second.quality;

        // Complement R2, as we use it to correct R1 below or concatenation it
        // to R1 if it's the read needs merging.
        seqan::complement(r2_seq);

        if (r1_start + read_len_diff >= r2_start) {
            // Adaptor read-through, trim R1, remove R2.
            size_t new_len = r1_len - read_len_diff - r1_start;
            new_len = new_len > r1_seq.size() ?  r1_seq.size() : new_len;

            for (size_t i = 0; i < new_len; i++) {
                size_t r2_pos = new_len - i - 1;
                if (r1_qual[i] < r2_qual[r2_pos]) {
                    r1_seq[i] = r2_seq[r2_pos];
                    r1_qual[i] = r2_qual[r2_pos];
                }
            }

            // Trim the read to its new length
            r1_seq.erase(new_len);
            r1_qual.erase(new_len);

            // Remove R2, as it's just duplicated
            r2_seq.erase();
            r2_qual.erase();

            _num_pairs_trimmed++;
        } else {
            // Read-through into acutal read, needs merging
            size_t overlap_starts = r2_start - read_len_diff;
            size_t overlap_size = r1_len - r2_start;

            // If R1 base is lower quality than R2, use the base from R2
            for (size_t i = 0; i < overlap_size; i++) {
                size_t r1_pos = overlap_starts + i + read_len_diff;
                size_t r2_pos = r2_len + read_len_diff - i;
                if (r1_qual[r1_pos] < r2_qual[r2_pos]) {
                    r1_seq[r1_pos] = r2_seq[r2_pos];
                    r1_qual[r1_pos] = r2_qual[r2_pos];
                }
            }

            // Remove the overlap from the read
            r2_seq.erase(overlap_starts);
            r2_qual.erase(overlap_starts);

            // Reverse the second read, so we can append it directly to the
            // first read
            std::reverse(r2_seq.begin(), r2_seq.end());
            std::reverse(r2_qual.begin(), r2_qual.end());

            // Append to form one psuedo read
            r1_seq += r2_seq;
            r1_qual += r2_qual;

            // Remove the second read from the read pair, so it doesn't get
            // printed.
            r2_seq.erase();
            r2_qual.erase();

            _num_pairs_joined++;
        }
    }

    _num_reads += 2;
}

std::string
AdaptorTrimPE::
report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_trimmed = (_num_pairs_trimmed * 2 / (float) _num_reads ) * 100;
    float percent_merged = (_num_pairs_joined * 2 / (float) _num_reads ) * 100;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "AdaptorTrimPE"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "name"
        << YAML::Value << _name
        << YAML::Key   << "parameters"
        << YAML::Value << YAML::BeginMap
                       << YAML::EndMap
        << YAML::Key   << "output"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "num_reads"
                       << YAML::Value << _num_reads
                       << YAML::Key << "num_trimmed"
                       << YAML::Value << (_num_pairs_trimmed * 2)
                       << YAML::Key << "num_merged"
                       << YAML::Value << (_num_pairs_joined * 2)
                       << YAML::Key << "percent_trimmed"
                       << YAML::Value << percent_trimmed
                       << YAML::Key << "percent_merged"
                       << YAML::Value << percent_merged
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();

}

} // namespace qcpp
