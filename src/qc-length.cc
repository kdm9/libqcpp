/*
 * ============================================================================
 *
 *       Filename:  qc-length.cc
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

#include "qc-length.hh"

#include <yaml-cpp/yaml.h>

namespace qcpp
{

ReadLenCounter::
ReadLenCounter(const std::string &name, const QualityEncoding &encoding)
    : ReadProcessor(name, encoding)
{
    _max_len = 0;
    _have_r2 = false;
}

void
ReadLenCounter::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len > _max_len) {
        for (size_t i = _max_len + 1; i <= read_len; i++) {
            _len_map_r1[i] = 0;
        }
        _max_len = read_len;
    }
    _len_map_r1[read_len]++;
    _num_reads++;
}

void
ReadLenCounter::
process_read_pair(ReadPair &the_read_pair)
{
    size_t read_len1 = the_read_pair.first.size();
    size_t read_len2 = the_read_pair.second.size();
    size_t larger_len = read_len1 > read_len2 ? read_len1 : read_len2;
    if (larger_len > _max_len) {
        for (size_t i = _max_len + 1; i <= larger_len; i++) {
            _len_map_r1[i] = 0;
            _len_map_r2[i] = 0;
        }
        _max_len = larger_len;
    }
    _len_map_r1[read_len1]++;
    _len_map_r2[read_len2]++;
    _num_reads += 2;
}

void
ReadLenCounter::
add_stats_from(ReadProcessor *other_ptr)
{
    ReadLenCounter &other = *reinterpret_cast<ReadLenCounter *>(other_ptr);
    _num_reads += other._num_reads;

    for (const auto &pair: other._len_map_r1) {
        _len_map_r1[pair.first] += pair.second;
    }
    for (const auto &pair: other._len_map_r2) {
        _len_map_r2[pair.first] += pair.second;
    }
}

std::string
ReadLenCounter::
yaml_report()
{
    std::ostringstream ss;
    YAML::Emitter yml;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key << "ReadLenCounter"
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
                       << YAML::Key << "r1_lengths"
                       << YAML::Flow
                       << YAML::Value << _len_map_r1
                       << YAML::Key << "r2_lengths"
                       << YAML::Flow
                       << YAML::Value << _len_map_r2
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}

ReadLenFilter::
ReadLenFilter(const std::string  &name, size_t threshold,
              const QualityEncoding &encoding)
    : ReadProcessor(name, encoding)
{
    _num_r1_dropped = 0;
    _num_r2_dropped = 0;
    _num_pairs_dropped = 0;
    _threshold = threshold;
}

void
ReadLenFilter::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len < _threshold) {
        the_read.clear();
        _num_r1_dropped++;
    }
    _num_reads++;
}

void
ReadLenFilter::
process_read_pair(ReadPair &the_read_pair)
{
    size_t read_len1 = the_read_pair.first.size();
    size_t read_len2 = the_read_pair.second.size();
    bool r1_dropped = false;
    if (read_len1 < _threshold) {
        the_read_pair.first.clear();
        _num_r1_dropped++;
        r1_dropped = true;
    }
    if (read_len2 < _threshold) {
        the_read_pair.second.clear();
        _num_r2_dropped++;
        if (r1_dropped) {
            _num_pairs_dropped++;
        }
    }
    _num_reads += 2;
}

void
ReadLenFilter::
add_stats_from(ReadProcessor *other_ptr)
{
    ReadLenFilter &other = *reinterpret_cast<ReadLenFilter *>(other_ptr);
    _num_reads += other._num_reads;
    _num_r1_dropped += other._num_r1_dropped;
    _num_r2_dropped += other._num_r2_dropped;
    _num_pairs_dropped += other._num_pairs_dropped;

}

std::string
ReadLenFilter::
yaml_report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_dropped = (_num_r1_dropped + _num_r2_dropped) * 100;
    percent_dropped /= (float) _num_reads;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "ReadLenFilter"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "name"
        << YAML::Value << _name
        << YAML::Key   << "parameters"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "threshold"
                       << YAML::Value << _threshold
                       << YAML::EndMap
        << YAML::Key   << "output"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "num_reads"
                       << YAML::Value << _num_reads
                       << YAML::Key << "num_r1_dropped"
                       << YAML::Value << _num_r1_dropped
                       << YAML::Key << "num_r2_dropped"
                       << YAML::Value << _num_r2_dropped
                       << YAML::Key << "num_pairs_dropped"
                       << YAML::Value << _num_pairs_dropped
                       << YAML::Key << "percent_dropped"
                       << YAML::Value << percent_dropped
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}


ReadTruncator::
ReadTruncator(const std::string  &name, const QualityEncoding &encoding,
              size_t threshold)
    : ReadProcessor(name, encoding)
{
    _num_r1_dropped = 0;
    _num_r2_dropped = 0;
    _num_pairs_dropped = 0;
    _threshold = threshold;
}

void
ReadTruncator::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len < _threshold) {
        the_read.clear();
        _num_r1_dropped++;
    } else {
        the_read.erase(_threshold);
    }
    _num_reads++;
}

void
ReadTruncator::
process_read_pair(ReadPair &the_read_pair)
{
    size_t read_len1 = the_read_pair.first.size();
    size_t read_len2 = the_read_pair.second.size();
    bool r1_dropped = false;
    if (read_len1 < _threshold) {
        the_read_pair.first.clear();
        _num_r1_dropped++;
        r1_dropped = true;
    } else {
        the_read_pair.first.erase(_threshold);
    }
    if (read_len2 < _threshold) {
        the_read_pair.second.clear();
        _num_r2_dropped++;
        if (r1_dropped) {
            _num_pairs_dropped++;
        }
    } else {
        the_read_pair.second.erase(_threshold);
    }
    _num_reads += 2;
}

void
ReadTruncator::
add_stats_from(ReadProcessor *other_ptr)
{
    ReadTruncator &other = *reinterpret_cast<ReadTruncator *>(other_ptr);
    _num_reads += other._num_reads;
    _num_r1_dropped += other._num_r1_dropped;
    _num_r2_dropped += other._num_r2_dropped;
    _num_pairs_dropped += other._num_pairs_dropped;

}

std::string
ReadTruncator::
yaml_report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_dropped = (_num_r1_dropped + _num_r2_dropped) * 100;
    percent_dropped /= (float) _num_reads;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "ReadTruncator"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "name"
        << YAML::Value << _name
        << YAML::Key   << "parameters"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "threshold"
                       << YAML::Value << _threshold
                       << YAML::EndMap
        << YAML::Key   << "output"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "num_reads"
                       << YAML::Value << _num_reads
                       << YAML::Key << "num_r1_dropped"
                       << YAML::Value << _num_r1_dropped
                       << YAML::Key << "num_r2_dropped"
                       << YAML::Value << _num_r2_dropped
                       << YAML::Key << "num_pairs_dropped"
                       << YAML::Value << _num_pairs_dropped
                       << YAML::Key << "percent_dropped"
                       << YAML::Value << percent_dropped
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}

} // end namespace qcpp
