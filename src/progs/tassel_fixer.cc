/*
 * ============================================================================
 *
 *       Filename:  tassel_fixer.cc
 *    Description:  Adds an A at the start of every read.
 *        License:  GPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 *
 * Copyright (c) 2015- Kevin Murray
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

#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>

#include "qcpp.hh"
#include "qc-length.hh"

using namespace qcpp;


void
usage()
{
    using std::cerr;
    using std::endl;
    cerr << "USAGE: tassel_fixer [-i INPUT -o OUTPUT]" << endl
         << endl
         << "OPTIONS:" << endl
         << " -o OUTPUT     Output file. [default stdout]" << endl
         << " -i INPUT      Input file. [default stdin]" << endl
         << endl;
}
const char *cli_opts = "o:i:";

int
main (int argc, char *argv[])
{
    ProcessedReadStream stream;
    std::ofstream       output;
    Read                read;
    std::string         input_file = "/dev/stdin";
    std::string         output_file = "/dev/stdout";

    int c = 0;
    while ((c = getopt(argc, argv, cli_opts)) > 0) {
        switch (c) {
            case 'o':
                output_file = optarg;
                break;
            case 'i':
                input_file = optarg;
                break;
            default:
                std::cerr << "Bad arg '" << std::string(1, optopt) << "'"
                          << std::endl << std::endl;
                usage();
                return EXIT_FAILURE;
        }
    }

    output.open(output_file);
    stream.append_processor<ReadTruncator>("Fix Length", SangerEncoding, 64);
    stream.open(input_file);
    while (stream.parse_read(read)) {
        read.sequence.insert(0, "A");
        read.quality.insert(0, "I");
        output << read.str();
    }
    return EXIT_SUCCESS;
}
