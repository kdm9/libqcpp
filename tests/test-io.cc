/*
 * ============================================================================
 *
 *       Filename:  test-io.cc
 *    Description:  Tests of the IO module.
 *        License:  GPLv3+
 *         Author:  Kevin Murray, spam@kdmurray.id.au
 *
 * ============================================================================
 */


#include "catch.hpp"
#include "helpers.hh"

#include "qc-io.hh"


#include <iostream>

TEST_CASE("Read structure behaves correctly", "[Read]") {
    qcpp::Read read;
    read.name = "Name";
    read.sequence = "ACGT";
    read.quality = "IIII";

    SECTION("Filling read members works") {
        REQUIRE(read.name.size() == 4);
        REQUIRE(read.sequence.size() == 4);
        REQUIRE(read.quality.size() == 4);
    }

    SECTION("Clearing a read empties members") {
        read.clear();

        REQUIRE(read.name.size() == 0);
        REQUIRE(read.sequence.size() == 0);
        REQUIRE(read.quality.size() == 0);
    }

    SECTION("str() works with quality") {
        REQUIRE(read.str() == "@Name\nACGT\n+\nIIII\n");
    }
    SECTION("str() works without quality") {
        read.quality.clear();
        REQUIRE(read.str() == ">Name\nACGT\n");
    }
}

TEST_CASE("ReadParser opening works", "[ReadParser]") {
    qcpp::ReadParser parser;
    TestConfig *config = TestConfig::get_config();
    std::string infile = config->get_data_file("valid.fastq");

    SECTION("opening valid FASTQ as std::string works") {
        REQUIRE_NOTHROW(parser.open(infile));
    }

    SECTION("opening valid FASTQ as char * works") {
        REQUIRE_NOTHROW(parser.open(infile.c_str()));
    }
}

TEST_CASE("Valid Fastq Reading", "[ReadParser]") {
    qcpp::Read read;
    qcpp::ReadParser parser;
    TestConfig *config = TestConfig::get_config();
    std::string infile = config->get_data_file("valid.fastq");
    REQUIRE_NOTHROW(parser.open(infile));

    SECTION("Parsing a valid fastq works, including get_num_reads") {
        size_t n_reads = 0;

        // Count all reads, parse_read returns false on EOF
        while (parser.parse_read(read)) {
            n_reads++;
        }

        REQUIRE(n_reads == 10);
        REQUIRE(parser.get_num_reads() == n_reads);
    }
}
