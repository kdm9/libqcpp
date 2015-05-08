/*
 * Copyright 2015 Kevin Murray <spam@kdmurray.id.au>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// This file is a demonstration of how to parse reads with libqc++
// It's also used to test the parsing code.

#include <iostream>
#include <string>

#include "qcpp.hh"
#include "qc-length.hh"

int
main (int argc, char *argv[])
{
    qcpp::Read r;
    qcpp::ReadParser rp;
    qcpp::ReadProcessorPipeline pipe;

    if (argc != 2) {
        std::cerr << "USAGE: " << argv[0] << " <read_file>" << std::endl;
        return EXIT_FAILURE;
    }
    rp.open(argv[1]);
    pipe.append_processor<qcpp::ReadLenCounter>("Count length");
    while (rp.parse_read(r)) {
        pipe.process_read(r);
    }
    std::cout << pipe.report();
    return EXIT_SUCCESS;
}
