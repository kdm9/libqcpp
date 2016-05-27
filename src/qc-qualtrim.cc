/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 */


#include <yaml-cpp/yaml.h>
#include "qc-qualtrim.hh"

namespace qcpp
{


WindowedQualTrim::
WindowedQualTrim(const std::string &name, const QualityEncoding &encoding,
                 int8_t min_quality, size_t min_length, size_t window_size)
    : ReadProcessor(name, encoding)
    , _min_quality(min_quality)
    , _min_length(min_length)
    , _window_size(window_size)
    , _num_reads_trimmed(0)
    , _num_reads_dropped(0)
{
}


WindowedQualTrim::
WindowedQualTrim(const std::string &name, const QualityEncoding &encoding,
                 int8_t min_quality, size_t min_length)
    : WindowedQualTrim(name, encoding, min_quality, min_length, 0)
{
}


void
WindowedQualTrim::
process_read(Read &the_read)
{
    int64_t         win_sum         = 0;
    size_t          win_start       = 0;
    size_t          win_size        = 0;
    float           win_avg         = 0.0;
    size_t          read_len        = the_read.size();
    size_t          keep_from       = 0;
    size_t          keep_until      = 0;
    std::string    &qual            = the_read.quality;

    _num_reads++;
    // Throw out reads which are already too short
    if (read_len < _min_length) {
        _num_reads_dropped++;
        return;
    }

    // Caclulate window size
    if (_window_size > 0) {
        // Use the size we've been told to
        win_size = _window_size;
    } else if (read_len > 20) {
        // Use 10% of the read length
        win_size = read_len * 0.1;
    } else {
        // Read too short, use whole read
        win_size = the_read.size();
    }

    // Trim until the first base which is of acceptable quality
    for (; win_start < read_len;) {
        if (_encoding.p2q(qual[win_start]) >= _min_quality) {
            break;
        }
        win_start++;
    }
    keep_from = win_start;
    keep_until = win_start;

    // pre-sum the first window
    for (size_t i = win_start; i < win_size; i++) {
        win_sum += _encoding.p2q(qual[i]);
    }
    // Trim with windows
    for (; win_start < read_len - win_size + 1; win_start += 1) {
        keep_until = win_start;
        win_avg = win_sum / (float)win_size;
        if (win_avg < _min_quality) {
            // If the window is below threshold, stop and trim below
            break;
        }
        win_sum -= _encoding.p2q(qual[win_start]);
        if (win_start + win_size < read_len) {
            win_sum += _encoding.p2q(qual[win_start + win_size]);
        }
    }

    // Find the last position above the threshold, trim there
    while (keep_until < read_len) {
        if (_encoding.p2q(qual[keep_until]) < _min_quality) {
            // Don't increment keep_until, as we should cut at this position
            break;
        }
        keep_until++;
    }


    size_t new_len = keep_until - keep_from;
    if (new_len < _min_length) {
        the_read.erase();
        _num_reads_dropped++;
        return;
    }
    bool trimmed = false;
    if (keep_until < read_len) {
        the_read.erase(keep_until);
        trimmed = true;
    }
    if (keep_from > 0) {
        the_read.erase(0, keep_from);
        trimmed = true;
    }
    if (trimmed) {
        _num_reads_trimmed++;
    }
}


void
WindowedQualTrim::
process_read_pair(ReadPair &the_read_pair)
{
    process_read(the_read_pair.first);
    process_read(the_read_pair.second);
}

void
WindowedQualTrim::
add_stats_from(ReadProcessor *other_ptr)
{
    WindowedQualTrim &other = *reinterpret_cast<WindowedQualTrim *>(other_ptr);

    _num_reads += other._num_reads;
    _num_reads_trimmed += other._num_reads_trimmed;
    _num_reads_dropped += other._num_reads_dropped;
}

std::string
WindowedQualTrim::
yaml_report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_trimmed = (_num_reads_trimmed * 2 / (float) _num_reads ) * 100;
    float percent_dropped = (_num_reads_dropped * 2 / (float) _num_reads ) * 100;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "WindowedQualTrim"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "name"
        << YAML::Value << _name
        << YAML::Key   << "parameters"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "min_quality"
                       << YAML::Value << _min_quality
                       << YAML::Key << "quality_encoding"
                       << YAML::Value << _encoding.name
                       << YAML::Key << "min_length"
                       << YAML::Value << _min_length
                       << YAML::Key << "window_size"
                       << YAML::Value << _window_size
                       << YAML::EndMap
        << YAML::Key   << "output"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "num_reads"
                       << YAML::Value << _num_reads
                       << YAML::Key << "num_trimmed"
                       << YAML::Value << (_num_reads_trimmed * 2)
                       << YAML::Key << "num_dropped"
                       << YAML::Value << (_num_reads_dropped * 2)
                       << YAML::Key << "percent_trimmed"
                       << YAML::Value << percent_trimmed
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
