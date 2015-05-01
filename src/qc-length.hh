/*
 * ============================================================================
 *
 *       Filename:  qc-length.hh
 *    Description:  Filter on and count read lengths
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#ifndef QC_LENGTH_HH
#define QC_LENGTH_HH

#include <map>
#include "qc-processor.hh"

namespace qcpp
{


class ReadLenCounter: public ReadProcessor
{
public:
    ReadLenCounter                 (const std::string  &name);

    void
    process_read                   (Read               &the_read);

    void
    process_read_pair              (ReadPair           &the_read_pair);

    std::string
    report                          ();

private:
    bool                    _have_r2;
    size_t                  _max_len;
    std::map<size_t, size_t> _len_map_r1;
    std::map<size_t, size_t> _len_map_r2;
    std::mutex              _map_mutex;
};


class ReadLenFilter: public ReadProcessor
{
public:
    ReadLenFilter                   (const std::string  &name,
                                     size_t              threshold=0);

    void
    process_read                    (Read               &the_read);

    void
    process_read_pair               (ReadPair           &the_read_pair);

    std::string
    report                          ();

private:
    std::atomic_ullong      _num_r1_trimmed;
    std::atomic_ullong      _num_r2_trimmed;
    std::atomic_ullong      _num_pairs_trimmed;
    size_t                  _threshold;
};


} // end namespace qcpp

#endif /* QC_LENGTH_HH */
