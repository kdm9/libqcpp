/*
 * ============================================================================
 *
 *       Filename:  qc-adaptor.hh
 *    Description:  Trim adaptor read-through from reads
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */


#include "qc-processor.hh"
#include "ext/ssw_cpp.h"

namespace qcpp
{

class AdaptorTrimPE: public ReadProcessor
{
public:
    AdaptorTrimPE                   (const std::string &name,
                                     int                min_overlap);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_pairs_trimmed;
    std::atomic_ullong      _num_pairs_joined;
    StripedSmithWaterman::Aligner _aligner;
    StripedSmithWaterman::Filter _filter;
    int                     _min_overlap;

    void
    process_read                    (Read              &the_read)
    {} // "Deleted" Pure Virtual function

};

} // namespace qcpp