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
    std::string infile = config->get_data_file("valid_il.fastq");

    SECTION("opening valid FASTQ as std::string works") {
        REQUIRE_NOTHROW(parser.open(infile));
    }

    SECTION("opening valid FASTQ as char * works") {
        REQUIRE_NOTHROW(parser.open(infile.c_str()));
    }
}

TEST_CASE("Valid file parsing, including get_num_reads", "[ReadParser]") {
    qcpp::Read read;
    qcpp::ReadParser parser;
    TestConfig *config = TestConfig::get_config();
    std::string infile;
    size_t n_reads = 0;

    SECTION("valid_il.fastq") {
        infile = config->get_data_file("valid_il.fastq");

        REQUIRE_NOTHROW(parser.open(infile));

        // Count all reads, parse_read returns false on EOF
        while (parser.parse_read(read)) {
            n_reads++;
        }

        REQUIRE(n_reads == 10);
        REQUIRE(parser.get_num_reads() == n_reads);
    }

    SECTION("valid.fasta") {
        infile = config->get_data_file("valid.fasta");

        REQUIRE_NOTHROW(parser.open(infile));

        // Count all reads, parse_read returns false on EOF
        while (parser.parse_read(read)) {
            n_reads++;
        }

        REQUIRE(n_reads == 10);
        REQUIRE(parser.get_num_reads() == n_reads);
    }
}

TEST_CASE("Parsing of invalid or weird files", "[ReadParser]") {
    qcpp::Read              read;
    qcpp::ReadParser        parser;
    TestConfig             *config = TestConfig::get_config();
    std::string             infile;
    size_t                  n_reads = 0;


    SECTION("Empty fastq") {
        infile = config->get_data_file("empty.fastq");
        REQUIRE_THROWS_AS(parser.open(infile), qcpp::IOError);

        while (parser.parse_read(read)) {
            n_reads++;
        }

        REQUIRE(n_reads == 0);
        REQUIRE(parser.get_num_reads() == n_reads);
    }

    SECTION("Truncated fastq") {
        infile = config->get_data_file("truncated.fastq");
        REQUIRE_NOTHROW(parser.open(infile));

        REQUIRE_NOTHROW(parser.parse_read(read)); // First read is OK
        REQUIRE_THROWS_AS(parser.parse_read(read), qcpp::IOError); // 2nd bad
    }
}

TEST_CASE("Read Interleaving", "[ReadInterleaver]") {
    qcpp::ReadPair          read_parser_pair;
    qcpp::ReadPair          read_interleaver_pair;
    qcpp::ReadParser        parser;
    qcpp::ReadInterleaver   interleaver;
    TestConfig             *config = TestConfig::get_config();
    std::string             il_file = config->get_data_file("valid_il.fastq");
    std::string             r1_file = config->get_data_file("valid_R1.fastq");
    std::string             r2_file = config->get_data_file("valid_R2.fastq");

    REQUIRE_NOTHROW(parser.open(il_file));
    REQUIRE_NOTHROW(interleaver.open(r1_file, r2_file));

    SECTION("Interleaving yields a correctly paired read stream") {
        size_t n_pairs = 0;
        bool parse_ret = true, il_ret = true;

        // Count all reads, parse_read returns false on EOF
        while (parse_ret && il_ret) {
            parse_ret = parser.parse_read_pair(read_parser_pair);
            il_ret = interleaver.parse_read_pair(read_interleaver_pair);
            REQUIRE(read_parser_pair == read_interleaver_pair);
            if (parse_ret && il_ret) {
                n_pairs++;
            }
        }
        REQUIRE(parse_ret == il_ret);

        size_t n_reads = n_pairs * 2;
        REQUIRE(n_reads == 10);
        REQUIRE(parser.get_num_reads() == n_reads);
        REQUIRE(interleaver.get_num_reads() == n_reads);
        REQUIRE(interleaver.get_num_pairs() == n_pairs);
    }
}

TEST_CASE("Fastq writing", "[ReadParser]") {
    qcpp::Read read;
    qcpp::ReadParser parser;
    TestConfig *config = TestConfig::get_config();
    std::string infile = config->get_data_file("valid_il.fastq");
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

