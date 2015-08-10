/*
 * ============================================================================
 *
 *       Filename:  test-length.cc
 *    Description:  Tests of the length module.
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

#include "catch.hpp"
#include "helpers.hh"

#include "qc-io.hh"
#include "qc-length.hh"

TEST_CASE("QC length counter works", "[ReadLenCounter]") {
    qcpp::ReadParser        parser;
    qcpp::ReadLenCounter    rlc("A Read Length Counter");
    TestConfig             *config = TestConfig::get_config();
    std::string             infile = config->get_data_file("valid_il.fastq");
    std::string             se_report_file = config->get_data_file(
                                    "qc_reports/report_ReadLenCounter_se.yml");
    std::string             pe_report_file = config->get_data_file(
                                    "qc_reports/report_ReadLenCounter_pe.yml");
    size_t                  n_reads = 0;
    std::string             report;

    parser.open(infile);
    SECTION("Single-end works") {
        qcpp::Read read;
        while (parser.parse_read(read)) {
            rlc.process_read(read);
            n_reads++;
        }

        report = rlc.report();
        REQUIRE(n_reads == 10);
        REQUIRE(filestrcmp(se_report_file, report));
    }
    SECTION("Paired-end works") {
        qcpp::ReadPair pair;
        while (parser.parse_read_pair(pair)) {
            rlc.process_read_pair(pair);
            n_reads += 2;
        }

        report = rlc.report();
        REQUIRE(n_reads == 10);
        REQUIRE(filestrcmp(pe_report_file, report));
    }
}
