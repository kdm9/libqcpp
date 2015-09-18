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

static system_clock::time_point start;

void
progress(size_t n)
{
    std::chrono::duration<double> tdiff = system_clock::now() - start;
    double secs = tdiff.count();
    double k_reads = n / 1000.0;
    double rate = k_reads / secs;
    std::cerr << "\x1b[2K" << "Kept " << k_reads << "K read pairs in "
              << (int)secs << "s (" << std::setprecision(1) << (int)rate
              << std::setprecision(3) << "K RP/sec)\r";
}

int
usage_err()
{
    using std::cerr;
    using std::endl;
    cerr << "USAGE: gbsqc [-t THREADS -y REPORT -o OUTPUT] <read_file>"
         << std::endl << std::endl;
    cerr << "OPTIONS:" << endl;
    cerr << " -y YAML    YAML report file. [default: none]" << endl;
    cerr << " -o OUTPUT  Output file. [default: stdout]" << endl;
    return EXIT_FAILURE;
}

const char *cli_opts = "y:o:";

int
main (int argc, char *argv[])
{
    using namespace qcpp;

    std::string             yaml_fname;
    std::ostream           *output = &std::cout;
    std::ofstream           output_file;
    size_t                  threads = std::thread::hardware_concurrency() - 1;

    int c = 0;
    while ((c = getopt(argc, argv, cli_opts)) > 0) {
        switch (c) {
            case 't':
                threads = atoi(optarg);
                break;
            case 'y':
                yaml_fname = optarg;
                break;
            case 'o':
                output_file.open(optarg);
                output = &output_file;
                break;
            default:
                std::cerr << "Bad arg '" << std::string(1, optopt) << "'"
                          << std::endl << std::endl;
                return usage_err();
        }
    }

    if (optind + 1 > argc) {
        std::cerr << "Must provide filename" << std::endl << std::endl;
        return usage_err();
    }

    std::string             in_fname(argv[optind]);
    ThreadedQCProcessor     proc(in_fname, output, threads);
    bool                    qc_before = false;

    if (qc_before) {
        proc.append_processor<PerBaseQuality>("before qc");
    }
    proc.append_processor<AdaptorTrimPE>("trim or merge reads", 10);
    proc.append_processor<WindowedQualTrim>("QC", SangerEncoding, 28, 64);
    proc.append_processor<PerBaseQuality>("after qc");

    proc.set_progress_callback(progress);
    start = system_clock::now();
    size_t num_reads = proc.run();
    std::cerr << std::endl;
    std::cerr << "Done! Processed " << (float)num_reads / 1000 << "K reads."
              << std::endl;
    if (yaml_fname.size() > 0) {
        std::ofstream yml_output(yaml_fname);
        yml_output << proc.report();
    }
    return EXIT_SUCCESS;
}
