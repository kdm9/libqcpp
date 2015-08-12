/*
 * ============================================================================
 *
 *       Filename:  test-qualtrim.cc
 *    Description:  Tests of the QualTrim module
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

