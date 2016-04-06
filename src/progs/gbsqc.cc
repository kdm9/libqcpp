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
using std::cerr;
using std::endl;

static system_clock::time_point start;

void
progress(size_t n)
{
    std::chrono::duration<double> tdiff = system_clock::now() - start;
    double secs = tdiff.count();
    double k_reads = n / 1000.0;
    double rate = k_reads / secs;
    cerr << "\x1b[2K" << "Kept " << k_reads << "K read pairs in "
         << (int)secs << "s (" << std::setprecision(1) << (int)rate
         << std::setprecision(3) << "K RP/sec)";
}

int
usage_err()
{
    cerr << "USAGE: gbsqc [options] <read_file>"
         << std::endl << std::endl;
    cerr << "OPTIONS:" << endl;
    cerr << " -y YAML     YAML report file. [default: none]" << endl;
    cerr << " -q QUAL     Minimum quality score [default: 25]" << endl;
    cerr << " -l LEN      Fix read lengths to LEN [default: off]" << endl;
    cerr << " -o OUTPUT   Output file. [default: stdout]" << endl;
    return EXIT_FAILURE;
}

const char *cli_opts = "q:y:o:l:";

int
main (int argc, char *argv[])
{
    using namespace qcpp;

    std::string             yaml_fname;
    std::ostream           *output = &std::cout;
    std::ofstream           output_file;
    size_t                  num_cpu = std::thread::hardware_concurrency();
    size_t                  threads = 1;
    size_t                  fix_length = 0;
    int                     quality_threshold = 25;

    cerr << "gbsqc -- from libqc++ version " << QCPP_VERSION << endl << endl;

    int c = 0;
    while ((c = getopt(argc, argv, cli_opts)) > 0) {
        switch (c) {
            case 'q':
                quality_threshold = atoi(optarg);
                break;
            case 'y':
                yaml_fname = optarg;
                break;
            case 'o':
                output_file.open(optarg);
                output = &output_file;
                break;
            case 'l':
                fix_length = atoll(optarg);
                break;
            default:
                cerr << "Bad arg '" << std::string(1, optopt) << "'"
                          << endl << endl;
                return usage_err();
        }
    }

    if (optind + 1 > argc) {
        cerr << "Must provide filename" << endl << endl;
        return usage_err();
    }

    std::string             in_fname(argv[optind]);
    ProcessedReadStream     stream;
    try {
        stream.open(argv[optind]);
    } catch (qcpp::IOError &e) {
        std::cerr << "Error opening input file:" << std::endl;
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (yaml_fname.size() > 0) {
        stream.append_processor<PerBaseQuality>("Measure Quality (Before)");
    }
    stream.append_processor<AdaptorTrimPE>("Trim or merge reads", 10);
    stream.append_processor<WindowedQualTrim>("Sliding window QC",
                                              SangerEncoding,
                                              quality_threshold, 1);
    if (fix_length > 0) {
        stream.append_processor<ReadTruncator>("Fix Length", SangerEncoding,
                                               fix_length);
    }
    if (yaml_fname.size() > 0) {
        stream.append_processor<PerBaseQuality>("after qc");
        stream.append_processor<ReadLenCounter>("Read Length Distribution");
    }

    while (stream.parse_read_pair(rp)) {
        std::string rp_str = rp.str();

        if (use_stdout) {
            std::cout << rp_str;
        } else {
            read_output << rp_str;
        }

        if (n_pairs % 10000 == 0) {
            progress(n_pairs, start);
        }
        n_pairs++;
    }
    if (yaml_fname.size() > 0) {
        std::ofstream yml_output(yaml_fname);
        yml_output << stream.report();
    }
    return EXIT_SUCCESS;
}
