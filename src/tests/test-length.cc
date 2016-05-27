/* Copyright (c) 2015-2016 Kevin Murray
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
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

        report = rlc.yaml_report();
        REQUIRE(n_reads == 10);
        REQUIRE(filestrcmp(se_report_file, report));
    }
    SECTION("Paired-end works") {
        qcpp::ReadPair pair;
        while (parser.parse_read_pair(pair)) {
            rlc.process_read_pair(pair);
            n_reads += 2;
        }

        report = rlc.yaml_report();
        REQUIRE(n_reads == 10);
        REQUIRE(filestrcmp(pe_report_file, report));
    }
}
