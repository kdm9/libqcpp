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
#include "qc-adaptor.hh"


TEST_CASE("AdaptorTrimPE correctness", "[AdaptorTrimPE]") {
    qcpp::ReadParser    parser;
    qcpp::ReadPair      rp;
    TestConfig         *config = TestConfig::get_config();
    qcpp::AdaptorTrimPE at("tm", 4);

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

        std::vector<std::string> expected_reads {
            "TGCTATATAAATGATGAGGATCATGCGTCGATTCTACTTAATGGTGGGTTGAGTACTTGTTCAC",
            "TGCAGAACCTGGAACTTTCATCTTGTACTTGTGCACG",
            "TGCAGAACCTGGAACTTTCATCTTGTACTTGTGCACG",
            "TGCAGAACCTGGAACTTTCATCTTGTACTTGTGCACG",
            "TGCAGAACCTGGAACTCTCATCTTGTACTTGTGCACG",
            "TGCAGAACCTGGAACTTTCATATTGTACTTGTGCACG",
        };
        std::vector<std::string> expected_qualities {
            "JJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAAAAAAAAA",
            "JJJJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAA",
            "JJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAAAA",
            "JJJJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAA",
            "AAAAAAAAAAAAJJJJJJJJJJJJJJJJJJJJJJJJJ",
            "JJJJJJJJJJJJJJJJJJJJJJJJJAAAAAAAAAAAA",
        };
        for (size_t i = 0; parser.parse_read_pair(rp); i++) {
            at.process_read_pair(rp);
            CAPTURE(i);
            REQUIRE(rp.first.sequence == expected_reads[i]);
            REQUIRE(rp.first.quality == expected_qualities[i]);
            REQUIRE(rp.second.size() == 0);
        }
    }
}
