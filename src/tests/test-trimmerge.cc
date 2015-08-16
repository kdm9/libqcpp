/*
 * ============================================================================
 *
 *       Filename:  test-trimmerge.cc
 *    Description:  Tests of the TrimMerge module
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
#include "qc-adaptor.hh"


TEST_CASE("AdaptorTrimPE correctness", "[AdaptorTrimPE]") {
    qcpp::ReadParser    parser;
    qcpp::ReadPair      rp; TestConfig         *config = TestConfig::get_config();
    qcpp::AdaptorTrimPE at("tm", 10);

    SECTION("tm-trim") {
        std::string         infile = config->get_data_file("tm-trim.fastq");

        parser.open(infile);

        std::vector<size_t> r1_expected_lens {94, 76};

        for (size_t i = 0; parser.parse_read_pair(rp); i++) {
            at.process_read_pair(rp);
            REQUIRE(rp.first.size() == r1_expected_lens[i]);
            REQUIRE(rp.second.size() == 0);
        }
    }

    SECTION("tm-merge") {
        std::string         infile = config->get_data_file("tm-merge.fastq");

        parser.open(infile);

        std::vector<size_t> r1_expected_lens {166};
        for (size_t i = 0; parser.parse_read_pair(rp); i++) {
            at.process_read_pair(rp);
            REQUIRE(rp.first.size() == r1_expected_lens[i]);
            REQUIRE(rp.second.size() == 0);
        }
    }
}
