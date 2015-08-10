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
    _overhang_pos(overhang_pos)
{
    _key_length = _re_site.size() - (2 * _overhang_pos);
    _re_key = _re_site.substr(_overhang_pos, _key_length);

}

void
GBSTrimPE::
process_read_pair(ReadPair &the_read_pair)
{
    Read &r1 = the_read_pair.first;
    Read &r2 = the_read_pair.second;
    const std::string &r1_seq = r1.sequence;
    const std::string &r2_seq = r2.sequence;

    size_t re_remainder = _re_site.length() - _overhang_pos;

    if (r1.size() < re_remainder || r2.size() < re_remainder) {
        r1.erase();
        r2.erase();
        return;
    }

    size_t r1_pos = re_remainder;
    size_t r2_pos = re_remainder;
    do {
        r1_pos = r1_seq.find(_re_key, r1_pos + 1);
        r2_pos = r2_seq.find(_re_key, r2_pos + 1);
        if (r1_pos != std::string::npos && r1_pos == r2_pos) {
            size_t trimpos = r1_pos + re_remainder * 2 - _overhang_pos;
            if (trimpos < r1.size()) {
                r1.erase(trimpos);
            }
            r2.erase(0);
            _num_pairs_trimmed++;
        }
    } while(r1_pos != std::string::npos && r2_pos != std::string::npos);
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
