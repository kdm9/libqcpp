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
#include "ssw_cpp.h"

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
    const size_t            _overhang_pos;
    StripedSmithWaterman::Aligner _aligner;
    StripedSmithWaterman::Filter _filter;

    void
    process_read                    (Read              &the_read)
    {} // "Deleted" Pure Virtual function

};

} // namespace qcpp
