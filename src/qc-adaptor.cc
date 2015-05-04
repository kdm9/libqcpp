/*
 * ============================================================================
 *
 *       Filename:  qc-adaptor.cc
 *    Description:  Trim adaptor read-through from reads
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
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
    _filter(true, true, 8, 4),
    _min_overlap(min_overlap)
{
}

void
AdaptorTrimPE::
process_read_pair(ReadPair &the_read_pair)
{
    using std::cout;
    using std::cerr;
    using std::endl;

    seqan::Align<std::string, seqan::ArrayGaps> aligner;
    resize(rows(aligner), 2);

    StripedSmithWaterman::Alignment align;

    std::string r2_rc = the_read_pair.second.sequence;
    seqan::reverseComplement(r2_rc);

    assignSource(row(aligner, 0), the_read_pair.first.sequence);
    assignSource(row(aligner, 1), r2_rc);

    _aligner.Align(the_read_pair.first.sequence.c_str(),
                   r2_rc.c_str(),
                   r2_rc.size(),
                   _filter,
                   &align);

    int score = seqan::globalAlignment(aligner,
                               seqan::Score<int, seqan::Simple>(1, -3, -3, -3),
                               seqan::AlignConfig<true, true, true, true>());
    size_t r1_len = the_read_pair.first.size();
    size_t r2_len = the_read_pair.second.size();
    ssize_t read_len_diff = r1_len - r2_len;

    if (score >= _min_overlap) {
        size_t r1_start = toViewPosition(row(aligner, 0), 0);
        size_t r2_start = toViewPosition(row(aligner, 1), 0);
        std::string &r1_seq = the_read_pair.first.sequence;
        std::string &r2_seq = the_read_pair.second.sequence;
        std::string &r1_qual = the_read_pair.first.quality;
        std::string &r2_qual = the_read_pair.second.quality;

        // cerr << r1_start << " " << r2_start << endl;
        if (r1_start > r2_start) {
            // Adaptor read-through, trim R1, truncate R2.
            size_t new_seq_len = r1_len - read_len_diff - r1_start + 1;

            // Complement the R2, as we use it to correct R1 below
            seqan::complement(r2_seq);

            // If R1 base is lower quality than R2, use the base from R2
            for (size_t i = 0; i < new_seq_len; i++) {
                size_t r2_pos = r2_len + read_len_diff - r1_start - i;
                if (r1_qual[i] < r2_qual[r2_pos]) {
                    r1_seq[i] = r2_seq[r2_pos];
                    r1_qual[i] = r2_qual[r2_pos];
                }
            }

            // Trim the read to its new length
            r1_seq.erase(new_seq_len - 1);
            r1_qual.erase(new_seq_len - 1);
            r2_seq.erase();
            r2_qual.erase();

            _num_pairs_trimmed++;
            cerr << the_read_pair.str();
        } else {
            // Read-through into acutal read, needs merging
            size_t overlap_starts = r2_start - read_len_diff;
            size_t overlap_size = r1_len - r2_start;

            // Complement the R2, as we use it to correct R1 below
            seqan::complement(r2_seq);

            for (size_t i = 0; i < overlap_size; i++) {
                size_t r1_pos = overlap_starts + i + read_len_diff;
                size_t r2_pos = r2_len + read_len_diff - i;
#if 0
                cerr << r1_qual[r1_pos] << " " << r2_qual[r2_pos] << " ";
                cerr << (r1_qual[r1_pos] < r2_qual[r2_pos]) << " ";
                cerr << r1_seq[r1_pos] << " " << r2_seq[r2_pos] << " ";
                cerr << r2_pos << endl;
#endif
                if (r1_qual[r1_pos] < r2_qual[r2_pos]) {
                    r1_seq[r1_pos] = r2_seq[r2_pos];
                    r1_qual[r1_pos] = r2_qual[r2_pos];
                }
            }

            r2_seq.erase(overlap_starts);
            r2_qual.erase(overlap_starts);

            std::reverse(r2_seq.begin(), r2_seq.end());
            std::reverse(r2_qual.begin(), r2_qual.end());

            r1_seq += r2_seq;
            r1_qual += r2_qual;

            r2_seq.erase();
            r2_qual.erase();

#if 0
            cerr << overlap_starts << endl;
            cerr << overlap_size << endl;
            cerr << aligner << endl;
            cerr << r1_seq << " ";
            cerr << r2_rc.substr(overlap_size) << endl;
            cerr << r1_seq << " ";
            cerr << r2_seq << endl;
#endif
            cerr << the_read_pair.str();
        }
    }

#if 0
    if (//align.ref_begin == 0 &&
            (r1_len - align.ref_begin) == align.query_end ||
            (r2_len - align.ref_end) == align.query_begin) {
    if ((r1_len - align.ref_begin) == align.query_end) {
        cerr << endl << endl;
        cerr << (r1_len - align.ref_begin) << " "
             << align.query_end << " "
             << (r1_len - align.ref_end) << " "
             << align.query_begin << endl;
        cerr << align.sw_score << endl;
        cerr << align.cigar_string << endl;
        cerr << std::string(align.ref_end, ' ') << the_read_pair.first.sequence << std::endl;
        cerr << r2_rc << std::endl;
        //align.clear();
        _num_pairs_trimmed++;
    }
#endif
    _num_reads += 2;
}

std::string
AdaptorTrimPE::
report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_trimmed = (_num_pairs_trimmed * 2 / (float) _num_reads ) * 100;

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
                       << YAML::Key << "percent_trimmed"
                       << YAML::Value << percent_trimmed
                       << YAML::EndMap
        << YAML::EndMap;
    yml << YAML::EndMap;
    yml << YAML::EndSeq;
    ss << yml.c_str() << "\n";
    return ss.str();

}

} // namespace qcpp
