/*
 * ============================================================================
 *
 *       Filename:  qc-length.cc
 *    Description:  Filter on and count read lengths
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include "qc-length.hh"

#include <yaml-cpp/yaml.h>

namespace qcpp
{

ReadLenCounter::
ReadLenCounter(const std::string  &name):
    ReadProcessor(name)
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
        std::lock_guard<std::mutex> lg(_map_mutex);
        for (size_t i = _max_len + 1; i <= larger_len; i++) {
            _len_map_r1[i] = 0;
            _len_map_r2[i] = 0;
        }
    }
    _len_map_r1[read_len1]++;
    _len_map_r2[read_len2]++;
    _num_reads += 2;
}

std::string
ReadLenCounter::
report()
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
                       << YAML::Value << _len_map_r1
                       << YAML::Key << "r2_lengths"
                       << YAML::Value << _len_map_r2
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}

ReadLenFilter::
ReadLenFilter(const std::string  &name,
              size_t threshold):
    ReadProcessor(name)
{
    _num_r1_trimmed = 0;
    _num_r2_trimmed = 0;
    _num_pairs_trimmed = 0;
    _threshold = threshold;
}

void
ReadLenFilter::
process_read(Read &the_read)
{
    size_t read_len = the_read.size();
    if (read_len < _threshold) {
        the_read.clear();
        _num_r1_trimmed++;
    }
    _num_reads++;
}

void
ReadLenFilter::
process_read_pair(ReadPair &the_read_pair)
{
    size_t read_len1 = the_read_pair.first.size();
    size_t read_len2 = the_read_pair.second.size();
    bool r1_trimmed = false;
    if (read_len1 < _threshold) {
        the_read_pair.first.clear();
        _num_r1_trimmed++;
        r1_trimmed = true;
    }
    if (read_len2 < _threshold) {
        the_read_pair.second.clear();
        _num_r2_trimmed++;
        if (r1_trimmed) {
            _num_pairs_trimmed++;
        }
    }
    _num_reads += 2;
}

std::string
ReadLenFilter::
report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_trimmed = (_num_r1_trimmed + _num_r2_trimmed) * 100;
    percent_trimmed /= (float) _num_reads;

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
                       << YAML::Key << "num_r1_trimmed"
                       << YAML::Value << _num_r1_trimmed
                       << YAML::Key << "num_r2_trimmed"
                       << YAML::Value << _num_r2_trimmed
                       << YAML::Key << "percent_trimmed"
                       << YAML::Value << percent_trimmed
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();
}

} // end namespace qcpp
