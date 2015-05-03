/*
 * ============================================================================
 *
 *       Filename:  qc-gbs.cc
 *    Description:  Trim reads from Genotyping-by-Sequcing protocols
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */


#include <yaml-cpp/yaml.h>

#include <seqan/modifier.h>

#include "qc-gbs.hh"

namespace qcpp
{

GBSTrimPE::
GBSTrimPE(const std::string &name, const std::string &re_site,
          int overhang_pos):
    ReadProcessor(name),
    _num_pairs_trimmed(0),
    _re_site(re_site),
    _overhang_pos(overhang_pos),
    _filter(true, true, 8, 4)
{
    size_t re_remainder = _re_site.size() - _overhang_pos;
    std::string re_seq = _re_site.substr(0, re_remainder);
    _aligner.SetReferenceSequence(re_seq.c_str(), re_seq.size());
}

void
GBSTrimPE::
process_read_pair(ReadPair &the_read_pair)
{
    using std::cout;
    using std::cerr;
    using std::endl;

    StripedSmithWaterman::Alignment r1_aln;
    StripedSmithWaterman::Alignment r2_aln;
    size_t re_remainder = _re_site.length() - _overhang_pos;
    std::string r1_query = the_read_pair.first.sequence.substr(re_remainder);
    std::string r2_query = the_read_pair.second.sequence.substr(re_remainder);
    _aligner.Align(r1_query.c_str(), _filter, &r1_aln);
    _aligner.Align(r2_query.c_str(), _filter, &r2_aln);
    if (r1_aln.ref_begin == 0 && r1_aln.ref_end == 4 &&
        r2_aln.ref_begin == 0 && r2_aln.ref_end == 4 &&
        r1_aln.query_begin == r2_aln.query_begin) {
#if 0
        cerr << "1  " << r1_query << endl;
        cerr << std::string(r1_aln.query_begin + 3, ' ') << "CTGCA" << endl;
        cerr << "2  " << r2_query << endl;
        cerr << std::string(r2_aln.query_begin + 3, ' ') << "CTGCA" << endl;

        std::string r2_rc = r2_query.substr(0, r2_aln.query_begin);
        seqan::reverseComplement(r2_rc);
        cerr << endl;
        cerr << "1  " << r1_query << endl;
        cerr << "rc " << r2_rc << endl;
#endif
        the_read_pair.first.erase(r1_aln.query_begin + re_remainder);
        the_read_pair.second.erase(0);
        _num_pairs_trimmed++;
    }
    _num_reads += 2;
}

std::string
GBSTrimPE::
report()
{
    std::ostringstream ss;
    YAML::Emitter yml;
    float percent_trimmed = (_num_pairs_trimmed * 2 / (float) _num_reads ) * 100;

    yml << YAML::BeginSeq;
    yml << YAML::BeginMap;
    yml << YAML::Key   << "GBSTrimPE"
        << YAML::Value
        << YAML::BeginMap
        << YAML::Key   << "name"
        << YAML::Value << _name
        << YAML::Key   << "parameters"
        << YAML::Value << YAML::BeginMap
                       << YAML::Key << "re_site"
                       << YAML::Value << _re_site
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
