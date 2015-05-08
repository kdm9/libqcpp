/*
 * ============================================================================
 *
 *       Filename:  qcparse.cc
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
    qcpp::ReadParser rp;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }
    rp.open(argv[1]);
    while (rp.parse_read(r)) {
        std::cout << r.str();
    }
    return EXIT_SUCCESS;
}
