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

namespace qcpp
{

ReadLenCounter::
ReadLenCounter()
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
}

std::string
ReadLenCounter::
report()
{
    std::ostringstream ss;

    ss << "ReadLenCounter\n";
    ss << "Num reads processed " << _num_reads << "\n";
    for (size_t i = 1; i <= _max_len; i++) {
        size_t r1 = _len_map_r1[i];
        if (r1 > 0) {
            ss << i << '\t' << r1;
            if (_have_r2) {
                ss << '\t' << _len_map_r2[i];
            }
            ss << '\n';
        }
    }

    return ss.str();
}

ReadLenFilter::
ReadLenFilter()
{
    _num_r1_trimmed = 0;
    _num_r2_trimmed = 0;
    _num_pairs_trimmed = 0;
    _threshold = 0;
}

ReadLenFilter::
ReadLenFilter(size_t threshold)
{
    _num_r1_trimmed = 0;
    _num_r2_trimmed = 0;
    _num_pairs_trimmed = 0;
    _threshold = threshold;
}

void
ReadLenFilter::
set_threshold(size_t threshold)
{
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
    float percent_trimmed = _num_reads - (_num_r1_trimmed + _num_r2_trimmed);
    percent_trimmed /= (float) _num_reads;

    ss << "ReadLenFilter\n";
    ss << "Num reads processed " << _num_reads << "\n";
    ss << "Threshold " << _threshold << "\n";
    ss << "Num reads trimmed:\n";
    ss << _num_r1_trimmed << "\t" << _num_r2_trimmed << "\n";
    ss << "Percent passed: " << percent_trimmed  * 100 << "%\n";

    return ss.str();
}

} // end namespace qcpp
