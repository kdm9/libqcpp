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
