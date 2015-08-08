/*
 * ============================================================================
 *
 *       Filename:  test-qualtrim.cc
 *    Description:  Tests of the QualTrim module
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */

#include "catch.hpp"
#include "helpers.hh"

#include "qc-io.hh"
#include "qc-qualtrim.hh"


TEST_CASE("WindowedQualTrimmer low_qual.fastq", "[qualtrim]") {
    qcpp::ReadParser        parser;
    qcpp::Read              r1, r2;
    TestConfig             *config = TestConfig::get_config();
    std::string             infile = config->get_data_file("low_qual.fastq");
    parser.open(infile);

    SECTION("Cutoff 20, len 30") {
        qcpp::WindowedQualTrim  wqt("qt", 20, 33, 30);
        REQUIRE_NOTHROW(parser.parse_read(r1));
        REQUIRE_NOTHROW(parser.parse_read(r2));

        std::cerr << "Before:\n";
        std::cerr << r1.str();
        std::cerr << r2.str();

        wqt.process_read(r1);
        wqt.process_read(r2);
        std::cerr << "After:\n";
        std::cerr << r1.str();
        std::cerr << r2.str();

        std::cerr << "YAML:\n";
        std::cerr << wqt.report();
    }
}
