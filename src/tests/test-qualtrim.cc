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
#include "qc-qualtrim.hh"


TEST_CASE("WindowedQualTrimmer low_qual.fastq", "[qualtrim]") {
    qcpp::ReadParser        parser;
    qcpp::Read              r1, r2;
    TestConfig             *config = TestConfig::get_config();
    std::string             infile = config->get_data_file("low_qual.fastq");
    parser.open(infile);

    SECTION("Cutoff 20, len 30") {
        qcpp::WindowedQualTrim  wqt("qt", qcpp::SangerEncoding, 20, 30);
        REQUIRE_NOTHROW(parser.parse_read(r1));
        REQUIRE_NOTHROW(parser.parse_read(r2));

        wqt.process_read(r1);
        wqt.process_read(r2);

        REQUIRE(r1.size() == 96);
        REQUIRE(r2.size() == 0);
    }

    SECTION("Cutoff 20, len 1") {
        qcpp::WindowedQualTrim  wqt("qt", qcpp::SangerEncoding, 20, 1);
        REQUIRE_NOTHROW(parser.parse_read(r1));
        REQUIRE_NOTHROW(parser.parse_read(r2));

        wqt.process_read(r1);
        wqt.process_read(r2);

        REQUIRE(r1.size() == 96);
        REQUIRE(r2.size() == 23);
    }

    SECTION("Cutoff 20, len 1, paired mode") {
        qcpp::WindowedQualTrim  wqt("qt", qcpp::SangerEncoding, 20, 1);
        qcpp::ReadPair rp;
        REQUIRE_NOTHROW(parser.parse_read_pair(rp));

        wqt.process_read_pair(rp);

        REQUIRE(rp.first.size() == 96);
        REQUIRE(rp.second.size() == 23);
    }
}

