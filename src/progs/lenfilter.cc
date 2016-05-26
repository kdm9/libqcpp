/*
 * ============================================================================
 *
 *       Filename:  lenfilter.cc
 *    Description:  Drops reads less than N, truncates reads greater than N
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
#include <chrono>
#include <iomanip>

#include <getopt.h>

#include "qcpp.hh"

#include "qc-length.hh"


using std::chrono::system_clock;
using namespace qcpp;

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


void
paired_main(std::ostream &output, qcpp::ProcessedReadStream &stream)
{
    ReadPair                rp;
    size_t                  n_pairs = 0;

    system_clock::time_point start = system_clock::now();
    while (stream.parse_read_pair(rp)) {
        if (n_pairs % 100000 == 0) {
            progress(n_pairs, start);
        }
        n_pairs++;
        output << rp.str();
    }
    progress(n_pairs, start);
    std::cerr << std::endl;
}

void
single_main(std::ostream &output, qcpp::ProcessedReadStream &stream)
{
    Read                read;
    size_t              n_reads = 0;

    system_clock::time_point start = system_clock::now();
    while (stream.parse_read(read)) {
        if (n_reads % 100000 == 0) {
            progress(n_reads, start);
        }
        n_reads++;
        output << read.str();
    }
    progress(n_reads, start);
    std::cerr << std::endl;
}

void
usage()
{
    using std::cerr;
    using std::endl;
    cerr << "USAGE: lenfilter [-l LEN -y YML -o OUTPUT -p ] <read_file>"
         << endl << endl;
    cerr << "OPTIONS:" << endl;
    cerr << " -l LEN     Minimum length of reads. [default: 40]" << endl;
    cerr << " -y YAML    YAML report file. [default: none]" << endl;
    cerr << " -o OUTPUT  Output file. [default: stdout]" << endl;
    cerr << " -p         Enable paired-end mode, both reads must meet length" << endl
         << "            requirement or the pair will be discarded." << endl;
}

const char *cli_opts = "o:l:y:p";

int
main (int argc, char *argv[])
{
    std::cerr << argv[0] << " version " << QCPP_VERSION
                          << std::endl << std::endl;

    size_t                  len_cutoff = 40;
    std::string             yaml_fname;
    bool                    paired_mode = false;
    bool                    use_stdout = true;
    std::ofstream           read_output;

    int c = 0;
    while ((c = getopt(argc, argv, cli_opts)) > 0) {
        switch (c) {
            case 'l':
                len_cutoff = atoi(optarg);
                break;
            case 'y':
                yaml_fname = optarg;
                break;
            case 'o':
                read_output.open(optarg);
                use_stdout = false;
                break;
            case 'p':
                paired_mode = true;
                break;
            default:
                std::cerr << "Bad arg '" << std::string(1, optopt) << "'"
                          << std::endl << std::endl;
                usage();
                return EXIT_FAILURE;
        }
    }

    if (optind + 1 > argc) {
        std::cerr << "Must provide filename" << std::endl << std::endl;
        usage();
        return EXIT_FAILURE;
    }
    std::string input_file = argv[optind];
    if (input_file == "-") {
        input_file = "/dev/stdin";
    }

    ProcessedReadStream     stream(input_file);

    stream.append_processor<ReadLenCounter>("Length Counter");
    stream.append_processor<ReadLenFilter>("Length Filter", len_cutoff);

    if (paired_mode) {
        if (use_stdout) {
            paired_main(std::cout, stream);
        } else {
            paired_main(read_output, stream);
        }
    } else {
        if (use_stdout) {
            single_main(std::cout, stream);
        } else {
            single_main(read_output, stream);
        }
    }

    if (yaml_fname.size() > 0) {
        std::ofstream yml_output(yaml_fname);
        yml_output << stream.report();
    }
    return EXIT_SUCCESS;
}
