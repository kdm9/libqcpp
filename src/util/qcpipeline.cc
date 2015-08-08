/*
 * ============================================================================
 *
 *       Filename:  qcpipeline.cc
 *    Description:  A demonstration of how to run a processor pipline with
 *                  libqc++
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <iostream>
#include <fstream>
#include <string>

#include "qcpp.hh"

#include "qc-measure.hh"
#include "qc-length.hh"

int
main (int argc, char *argv[])
{
    std::ofstream yml_output;
    qcpp::ProcessedReadStream stream;
    uint64_t n_pairs = 0;

    if (argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <read_file> <yml_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    yml_output.open(argv[2]);
    stream.append_processor<qcpp::PerBaseQuality>("before qc");
    stream.append_processor<qcpp::ReadLenFilter>("trim at 50", 50);
    stream.append_processor<qcpp::ReadLenCounter>("after qc");

    // to enable paralleism uncomment these
    //#pragma omp parallel
    {
        qcpp::ReadPair rp;
        while (stream.parse_read_pair(rp)) {
            // __sync_fetch_and_add(&n_pairs, 1);
            if (n_pairs++ % 100000 == 0) {
                std::cerr << "Processed " << n_pairs << "\r";
            }
            //#pragma omp critical
            {
                // std::cout << rp.str();
            }
        }
    }
    std::cerr << "Processed " << n_pairs << std::endl;
    yml_output << stream.report();
    return EXIT_SUCCESS;
}
