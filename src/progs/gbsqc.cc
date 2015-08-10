/*
 * ============================================================================
 *
 *       Filename:  gbsqc.cc
 *    Description:  A GBS quality control pipeline
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

#include "qcpp.hh"

#include "qc-measure.hh"
#include "qc-length.hh"
#include "qc-qualtrim.hh"
#include "qc-gbs.hh"

using std::chrono::system_clock;

inline void
progress(size_t n, system_clock::time_point start)
{
    std::chrono::duration<double> tdiff = system_clock::now() - start;
    double secs = tdiff.count();
    double k_reads = n / 1000.0;
    double rate = k_reads / secs;
    std::cerr << "\x1b[2K" << "Kept " << k_reads << "K read pairs in "
              << (int)secs << "s (" << std::setprecision(3) << rate
              << std::setprecision(9) << "K RP/sec)\r";
}

int
main (int argc, char *argv[])
{
    std::cerr << "GBS-QC" << std::endl << std::endl;

    if (argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <read_file> <yml_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    qcpp::ProcessedReadStream   stream(argv[1]);
    std::ofstream               yml_output(argv[2]);
    uint64_t                    n_pairs = 0;

    //stream.append_processor<qcpp::PerBaseQuality>("before qc");
    stream.append_processor<qcpp::GBSTrimPE>("trim Pst1 read-through", "CTGCAG", 1);
    stream.append_processor<qcpp::WindowedQualTrim>("QC", 28, 33, 50);
    //stream.append_processor<qcpp::PerBaseQuality>("after qc");

    system_clock::time_point start = system_clock::now();
    qcpp::ReadPair rp;
    while (stream.parse_read_pair(rp)) {
        if (n_pairs % 100000 == 0) {
            progress(n_pairs, start);
        }
        n_pairs++;
    }
    }
    progress(n_pairs, start);
    std::cerr << std::endl;
    yml_output << stream.report();
    return EXIT_SUCCESS;
}
