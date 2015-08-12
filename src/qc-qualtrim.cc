/*
 * ============================================================================
 *
 *       Filename:  qc-qualtrim.cc
 *    Description:  Trim low quality sequences via various methods
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
#include "qc-qualtrim.hh"

namespace qcpp
{


WindowedQualTrim::
WindowedQualTrim(const std::string &name, QualityEncoding quality_encoding,
                 int8_t phred_cutoff, size_t len_cutoff, size_t window_size):
    ReadProcessor(name),
    _num_reads_trimmed(0),
    _num_reads_dropped(0),
    _encoding(quality_encoding),
    _phred_cutoff(phred_cutoff),
    _len_cutoff(len_cutoff),
    _window_size(window_size)
{
}


WindowedQualTrim::
WindowedQualTrim(const std::string &name, QualityEncoding quality_encoding,
                 int8_t phred_cutoff, size_t len_cutoff):
    WindowedQualTrim(name, quality_encoding, phred_cutoff, len_cutoff, 0)
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

    _num_reads++;
    // Throw out reads which are already too short
    if (read_len < _len_cutoff) {
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
        if (_qual_of_base(the_read, win_start) >= _phred_cutoff) {
            break;
        }
        win_start++;
    }
    keep_from = win_start;
    keep_until = win_start;

    // pre-sum the first window
    for (size_t i = win_start; i < win_size; i++) {
        win_sum += _qual_of_base(the_read, i);
    }
    // Trim with windows
    for (; win_start < read_len - win_size + 1; win_start += 1) {
        keep_until = win_start;
        win_avg = win_sum / (float)win_size;
        if (win_avg < _phred_cutoff) {
            // If the window is below threshold, stop and trim below
            break;
        }
        win_sum -= _qual_of_base(the_read, win_start);
        if (win_start + win_size < read_len) {
            win_sum += _qual_of_base(the_read, win_start + win_size);
        }
    }

    // Find the last position above the threshold, trim there
    while (keep_until < read_len) {
        if (_qual_of_base(the_read, keep_until) < _phred_cutoff) {
            // Don't increment keep_until, as we should cut at this position
            break;
        }
        keep_until++;
    }


    size_t new_len = keep_until - keep_from;
    if (new_len < _len_cutoff) {
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


std::string
WindowedQualTrim::
report()
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
                       << YAML::Key << "phred_cutoff"
                       << YAML::Value << _phred_cutoff
                       << YAML::Key << "quality_encoding"
                       << YAML::Value << _encoding.name
                       << YAML::Key << "len_cutoff"
                       << YAML::Value << _len_cutoff
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
