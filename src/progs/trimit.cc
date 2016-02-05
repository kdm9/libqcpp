/*
 * ============================================================================
 *
 *       Filename:  trimmit.cc
 *    Description:  Just merges and quality trims interleaved reads.
 *        License:  LGPL-3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

/* Copyright (c) 2015 Kevin Murray
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <iomanip>

#include <getopt.h>

#include "qcpp.hh"

#include "qc-measure.hh"
#include "qc-length.hh"
#include "qc-qualtrim.hh"
#include "qc-gbs.hh"
#include "qc-adaptor.hh"


using std::chrono::system_clock;

inline void
progress(size_t n, system_clock::time_point start)
{
    std::chrono::duration<double> tdiff = system_clock::now() - start;
    double secs = tdiff.count();
    double k_reads = n / 1000.0;
    double rate = k_reads / secs;
    std::cerr << "\33[2K" << "Kept " << std::setprecision(3) << k_reads
              << "K read pairs in " << (int)secs << "s (" << (int)rate
              << "K RP/sec)\r";
}

int
usage_err()
{
    using std::cerr;
    using std::endl;
    cerr << "USAGE: gbsqc [-b -s -y REPORT -o OUTPUT] [<read_file>]" << std::endl
         << std::endl;
    cerr << "OPTIONS:" << endl;
    cerr << " -b         Use broken-paired output (don't keep read pairing) [default: false]" << endl;
    cerr << " -q QUAL    Minimum acceptable PHRED score. [default: 25]" << endl;
    cerr << " -y YAML    YAML report file. [default: none]" << endl;
    cerr << " -o OUTPUT  Output file. [default: stdout]" << endl;
    cerr << " -s         Single ended mode (no trim-merge). [default: false]" << endl;
    cerr << " -h         Show this help message." << endl;
    cerr << endl;
    cerr << "By default, reads from stdin and spits out good reads on stdout." <<  endl;
    return EXIT_FAILURE;
}

const char *cli_opts = "shq:y:o:b";

int
main (int argc, char *argv[])
{
    using namespace qcpp;

    std::string             yaml_fname;
    bool                    broken_paired = false;
    bool                    single_end = false;
    std::ofstream           read_output;
    std::string             outfile = "/dev/stdout";
    std::string             infile = "/dev/stdin";
    int                     qual_threshold = 25;

    int c = 0;
    while ((c = getopt(argc, argv, cli_opts)) > 0) {
        switch (c) {
            case 'y':
                yaml_fname = optarg;
                break;
            case 'o':
                outfile = optarg;
                break;
            case 'b':
                broken_paired = true;
                break;
            case 's':
                single_end = true;
                break;
            case 'q':
                qual_threshold = atoi(optarg);
                break;
            case 'h':
                usage_err();
                return EXIT_SUCCESS;
            default:
                std::cerr << "Bad arg '" << std::string(1, optopt) << "'"
                          << std::endl << std::endl;
                return usage_err();
        }
    }

    read_output.open(outfile);

    if (optind + 1 > argc) {
        std::cerr << "Reading from stdin! (Use -h for help)" << std::endl;
    } else {
        infile = argv[optind];
    }

    ProcessedReadStream     stream;
    uint64_t                n_pairs = 0;
    bool                    qc_before = false;

    try {
        stream.open(infile);
    } catch (qcpp::IOError  &e) {
        std::cerr << "Error opening input file:" << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (qc_before) {
        stream.append_processor<PerBaseQuality>("before qc");
    }
    if (!single_end) {
        stream.append_processor<AdaptorTrimPE>("trim or merge reads", 10);
    }
    stream.append_processor<WindowedQualTrim>("QC", SangerEncoding, qual_threshold, 1);
    stream.append_processor<PerBaseQuality>("after qc");

    system_clock::time_point start = system_clock::now();
    if (single_end) {
        Read rd;
        while (stream.parse_read(rd)) {
            read_output << rd.str();
            if (n_pairs % 10000 == 0) {
                progress(n_pairs, start);
            }
            n_pairs++;
        }
    } else {
        ReadPair rp;
        while (stream.parse_read_pair(rp)) {
            std::string rp_str = "";

            if (broken_paired) {
                if (rp.first.size() >= 64) {
                    rp_str = rp.first.str();
                }
                if (rp.second.size() >= 64) {
                    rp_str += rp.second.str();
                }
            } else {
                rp_str = rp.str();
            }

            if (n_pairs % 10000 == 0) {
                progress(n_pairs, start);
            }
            n_pairs++;

            read_output << rp_str;
        }
    }
    progress(n_pairs, start);
    std::cerr << std::endl;
    if (yaml_fname.size() > 0) {
        std::ofstream yml_output(yaml_fname);
        yml_output << stream.report();
    }
    return EXIT_SUCCESS;
}