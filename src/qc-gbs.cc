/*
 * ============================================================================
 *
 *       Filename:  qc-gbs.cc
 *    Description:  Trim reads from Genotyping-by-Sequcing protocols
 *        License:  LGPL-3+
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
    _key_length = _re_site.size() - (2 * _overhang_pos);
    _re_key = _re_site.substr(_overhang_pos, _key_length);
    _aligner.SetReferenceSequence(_re_key.c_str(), _re_key.size());

}

void
GBSTrimPE::
process_read_pair(ReadPair &the_read_pair)
{
    using std::cout;
    using std::cerr;
    using std::endl;

    cerr << std::string(80, '-') << endl;

    Read &r1 = the_read_pair.first;
    Read &r2 = the_read_pair.second;
    const std::string &r1_seq = r1.sequence;
    const std::string &r2_seq = r2.sequence;

    size_t re_remainder = _re_site.length() - _overhang_pos;

    if (r1.size() < re_remainder || r2.size() < re_remainder) {
        std::cerr << r1.str() << r2.str();
        r1.erase();
        r2.erase();
        return;
    }

#if 1
    size_t r1_pos = re_remainder;
    size_t r2_pos = re_remainder;
    do {
        r1_pos = r1_seq.find(_re_key, r1_pos + 1);
        r2_pos = r2_seq.find(_re_key, r2_pos + 1);
        cerr << r1_pos << " " << r2_pos << std::endl;
        if (r1_pos != std::string::npos && r1_pos == r2_pos) {
#define DSDSDFSD
#ifdef DSDSDFSD
            cerr << "1    " << r1_seq << endl;
            cerr << std::string(r1_pos + 4, ' ') << "CTGCA|G" << endl;
            cerr << "2    " << r2_seq << endl;
            cerr << std::string(r2_pos + 4, ' ') << "CTGCA|G" << endl;

            std::string r2_rc = r2_seq.substr(0, r2_pos - _overhang_pos);
            seqan::reverseComplement(r2_rc);
            cerr << endl;
            cerr << "1    " << r1_seq << endl;
            cerr << "rc   " << r2_rc << endl;
#endif
            r1.erase(r1_pos + re_remainder * 2 - _overhang_pos);
            r2.erase(0);
#ifdef DSDSDFSD
            cerr << endl << r1.str();
#endif
            _num_pairs_trimmed++;
        }
    } while(r1_pos != std::string::npos && r2_pos != std::string::npos);
#else
    StripedSmithWaterman::Alignment r1_aln;
    StripedSmithWaterman::Alignment r2_aln;

    _aligner.Align(r1_query.c_str(), _filter, &r1_aln);
    _aligner.Align(r2_query.c_str(), _filter, &r2_aln);

#if 0
    if (r1_aln.ref_begin == 0 && r1_aln.ref_end == 4 &&
        r2_aln.ref_begin == 0 && r2_aln.ref_end == 4 &&
        r1_aln.query_begin == r2_aln.query_begin) {
#else
    cerr << r1_aln.query_begin << " " << r2_aln.query_begin << endl;
    if (r1_aln.query_begin == r2_aln.query_begin) {
#endif
#if 1
        cerr << "1  " << r1_query << endl;
        cerr << std::string(r1_aln.query_begin + 2, ' ') << "CTGCA|G" << endl;
        cerr << "2  " << r2_query << endl;
        cerr << std::string(r2_aln.query_begin + 2, ' ') << "CTGCA|G" << endl;

        std::string r2_rc = r2_query.substr(0, r2_aln.query_begin - _overhang_pos);
        seqan::reverseComplement(r2_rc);
        cerr << endl;
        cerr << "1  " << r1_query << endl;
        cerr << "rc " << r2_rc << endl;
        cerr << std::string(80, '-') << endl;
#endif
        the_read_pair.first.erase(r1_aln.query_begin + re_remainder);
        the_read_pair.second.erase(0);
        _num_pairs_trimmed++;
    }
#endif
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
