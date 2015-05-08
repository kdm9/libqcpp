/*
 * ============================================================================
 *
 *       Filename:  qcpipeline.cc
 *    Description:  A demonstration of how to run a processor pipline with
 *                  libqc++
 *        License:  GPLv3+
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

    if (argc != 3) {
        std::cerr << "USAGE: " << argv[0] << " <read_file> <yml_file>"
                  << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    yml_output.open(argv[2]);
    stream.append_processor<qcpp::PerBasePhredHistogram>("before qc");
    stream.append_processor<qcpp::ReadLenFilter>("trim at 50", 50);
    stream.append_processor<qcpp::ReadLenCounter>("after qc");

    // to enable paralleism do this
    //#pragma omp parallel
    {
        qcpp::ReadPair rp;
        while (stream.parse_read_pair(rp)) {
            // std::cout << rp.str();
        }
    }
    yml_output << stream.report();
    return EXIT_SUCCESS;
}
