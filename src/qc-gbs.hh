/*
 * ============================================================================
 *
 *       Filename:  qc-gbs.hh
 *    Description:  Trim reads from Genotyping-by-Sequcing protocols
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */


#include "qc-processor.hh"

namespace qcpp
{

class GBSTrimPE: public ReadProcessor
{
public:
    GBSTrimPE                       (const std::string &name,
                                     const std::string &re_site,
                                     int                overhang_pos);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_pairs_trimmed;
    const std::string       _re_site;
    std::string             _re_key;
    const size_t            _overhang_pos;
    size_t                  _key_length;

    void
    process_read                    (Read              &the_read)
    {} // "Deleted" Pure Virtual function

};

} // namespace qcpp
