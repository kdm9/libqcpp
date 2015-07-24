/*
 * ============================================================================
 *
 *       Filename:  qc-qualtrim.cc
 *    Description:  Trim low qualtiy sequences via various methods
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <yaml-cpp/yaml.h>
#include "qc-qualtrim.hh"

namespace qcpp
{

WindowedQualTrim::
WindowedQualTrim(const std::string &name, int8_t phred_cutoff,
                 int8_t phred_offset, size_t len_cutoff, size_t window_size):
    ReadProcessor(name),
    _num_reads_trimmed(0),
    _num_reads_dropped(0),
    _phred_cutoff(phred_cutoff),
    _phred_offset(phred_offset),
    _len_cutoff(len_cutoff),
    _window_size(window_size)
{
}

WindowedQualTrim::
WindowedQualTrim(const std::string &name, int8_t phred_cutoff,
                 int8_t phred_offset, size_t len_cutoff):
    WindowedQualTrim(name, phred_offset, phred_cutoff, len_cutoff, 0)
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
    size_t          first_to_keep   = 0;
    size_t          last_to_keep    = read_len - 1;
    bool            need_trimming   = false;

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
    while (_qual_of_base(the_read, win_start) < _phred_cutoff) {
        need_trimming = true;
        win_start++;
    }
    first_to_keep = win_start;

    // pre-sum the first window
    for (size_t i = win_start; i < win_size; i++) {
        win_sum += _qual_of_base(the_read, i);
    }
    // Trim with windows
    for (; win_start < read_len - win_size + 1; win_start++) {
        win_avg = win_sum / (float)win_size;
        // IF the window is below threshold
        if (win_avg < _phred_cutoff || win_start >= read_len - win_size) {
            last_to_keep = win_start - 1;
            while (_qual_of_base(the_read, ++last_to_keep) >= _phred_cutoff);
            if (last_to_keep < read_len - 1) {
                need_trimming = true;
            }
            break;
        }
        win_sum -= _qual_of_base(the_read, win_start);
        win_sum += _qual_of_base(the_read, win_start + win_size);
    }
    if ((last_to_keep - first_to_keep) < _len_cutoff) {
        the_read.erase();
        _num_reads_dropped++;
    } else if (need_trimming) {
        if (last_to_keep < read_len - 1) {
            the_read.erase(last_to_keep);
        }
        if (first_to_keep > 0) {
            the_read.erase(0, first_to_keep);
        }
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
                       << YAML::Key << "phred_offset"
                       << YAML::Value << _phred_offset
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
