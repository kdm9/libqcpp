/*
 * ============================================================================
 *
 *       Filename:  qc-qualtrim.hh
 *    Description:  Trim low quality sequences via various methods
 *        License:  LGPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
*/

#ifndef QC_QUALTRIM_HH
#define QC_QUALTRIM_HH

#include "qc-config.hh"
#include "qc-processor.hh"

namespace qcpp
{

class WindowedQualTrim: public ReadProcessor
{
    // This class implements a similar algorithm to Nik Joshi's sickle tool
public:
    WindowedQualTrim                (const std::string &name,
                                     int8_t             phred_cutoff,
                                     int8_t             phred_offset,
                                     size_t             len_cutoff,
                                     size_t             window_size);
    WindowedQualTrim                (const std::string &name,
                                     int8_t             phred_cutoff,
                                     int8_t             phred_offset,
                                     size_t             len_cutoff);

    void
    process_read                    (Read              &the_read);

    void
    process_read_pair               (ReadPair          &the_read_pair);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_reads_trimmed;
    std::atomic_ullong      _num_reads_dropped;
    int8_t                  _phred_cutoff;
    int8_t                  _phred_offset;
    size_t                  _len_cutoff;
    size_t                  _window_size;

    inline int8_t
    _qual_of_base                   (const Read &the_read,
                                     const size_t idx)
    {
        return the_read.quality[idx] - _phred_offset;
    }
};


} // namespace qcpp


#endif /* QC_QUALTRIM_HH */
