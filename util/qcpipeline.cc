/*
 * ============================================================================
 *
 *       Filename:  qc-processor.hh
 *    Description:  A demonstration of how to parse reads with libqc++. It's
 *                  also used to test the parsing code.
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include <iostream>
#include <string>

#include "qcpp.hh"

int
main (int argc, char *argv[])
{
    qcpp::Read r;
    qcpp::ProcessedReadStream stream;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }

    stream.open(argv[1]);
    stream.append_processor<qcpp::ReadLenCounter>("before qc");
    stream.append_processor<qcpp::ReadLenFilter>("trim at 50", 50);
    stream.append_processor<qcpp::ReadLenCounter>("after qc");

    while (stream.parse_read(r)) {
        // std::cout << r.str();
    }
    std::cerr << stream.report();
    return EXIT_SUCCESS;
}
