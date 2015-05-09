/*
 * ============================================================================
 *
 *       Filename:  qc-measure.hh
 *    Description:  Collect measures of read quality
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#ifndef QC_MEASURE_HH
#define QC_MEASURE_HH

#include <map>
#include <array>
#include "qc-processor.hh"

namespace qcpp
{

// It's a kludge, but we have to use uint64_t, as std::atomic doesn't like
// being in a std::array. ALWAYS use __sync_add_and_fetch and friends.
typedef std::array<uint64_t, 50> PhredHistogramArray;

class QCMeasure: public ReadProcessor
{
public:
    QCMeasure                       (const std::string &name,
                                     unsigned           phred_offset=33);

protected:
    bool                    _have_r2;
    unsigned                _phred_offset;
};

class PerBaseQuality: public QCMeasure
{
public:
    PerBaseQuality                  (const std::string &name,
                                     unsigned           phred_offset=33);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    size_t                  _max_len;
    std::mutex              _expansion_mutex;
    // Vector of arrays, one histogram array per base of each read
    std::vector<PhredHistogramArray> _qual_scores_r1;
    std::vector<PhredHistogramArray> _qual_scores_r2;
};

} // end namespace qcpp

#endif /* QC_MEASURE_HH */
