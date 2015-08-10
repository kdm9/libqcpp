/*
 * ============================================================================
 *
 *       Filename:  test-io.cc
 *    Description:  Tests of the IO module.
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


#include <iostream>

TEST_CASE("Read structure behaves correctly", "[Read]") {
    qcpp::Read read("Name", "ACGT", "IIII");

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

    SECTION("size() gives correct results w/ quality") {
        REQUIRE(read.sequence.size() == read.size());
        REQUIRE(read.quality.size() == read.size());
    }

    SECTION("size() gives correct results w/o quality") {
        read.quality.clear();
        REQUIRE(read.sequence.size() == read.size());
        REQUIRE(read.quality.size() == 0);
    }

    SECTION("str() works w/ quality") {
        REQUIRE(read.str() == "@Name\nACGT\n+\nIIII\n");
    }

    SECTION("str() works w/o quality") {
        read.quality.clear();
        REQUIRE(read.str() == ">Name\nACGT\n");
    }

    SECTION("Erase with just start") {
        read.erase(1);
        REQUIRE(read.sequence == "A");
        REQUIRE(read.quality == "I");
        REQUIRE(read.size() == 1);
    }

    SECTION("Erase in middle with start and end") {
        read.erase(1, 2);
        REQUIRE(read.sequence == "AT");
        REQUIRE(read.quality == "II");
        REQUIRE(read.size() == 2);
    }

    SECTION("Erase from start with start and end") {
        read.erase(0, 1);
        REQUIRE(read.sequence == "CGT");
        REQUIRE(read.quality == "III");
        REQUIRE(read.size() == 3);
    }
    SECTION("Erase count=0") {
        read.erase(0, 0);
        REQUIRE(read.sequence == "ACGT");
        REQUIRE(read.quality == "IIII");
        REQUIRE(read.size() == 4);
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
    qcpp::ReadWriter writer;
    TestConfig *config = TestConfig::get_config();
    std::string outfile = config->get_writable_file("fastq");

    CAPTURE(outfile);
    REQUIRE_NOTHROW(writer.open(outfile));

    SECTION("Writing a single read works") {
        qcpp::Read read("Name", "ACGT", "IIII");

        REQUIRE(writer.get_num_reads() == 0);

        REQUIRE_NOTHROW(writer.write_read(read));

        REQUIRE(writer.get_num_reads() == 1);

        REQUIRE_NOTHROW(writer.close());
        REQUIRE(filecmp(outfile, "data/truths/se.fastq"));
    }
    SECTION("Writing a single read works") {
        qcpp::ReadPair rp("seq1", "ACGT", "IIII",
                          "seq2", "ACGT", "IIII");

        REQUIRE(writer.get_num_reads() == 0);

        REQUIRE_NOTHROW(writer.write_read_pair(rp));

        REQUIRE(writer.get_num_reads() == 2);

        REQUIRE_NOTHROW(writer.close());
        REQUIRE(filecmp(outfile, "data/truths/pe.fastq"));
    }
}

TEST_CASE("Round-trip parse-write", "[ReadParser]") {
    qcpp::ReadParser    parser;
    qcpp::ReadWriter    writer;
    TestConfig         *config = TestConfig::get_config();
    std::string         infile = config->get_data_file("valid_il.fastq");
    std::string         outfile = config->get_writable_file("fastq", true);
    size_t              n_reads = 0;

    CAPTURE(infile);
    CAPTURE(outfile);
    REQUIRE_NOTHROW(parser.open(infile));
    REQUIRE_NOTHROW(writer.open(outfile));

    SECTION("Single-ended round trip") {
        qcpp::Read read;

        while (parser.parse_read(read)) {
            REQUIRE_NOTHROW(writer.write_read(read));
            n_reads++;
        }

        REQUIRE(n_reads == 10);
        REQUIRE(parser.get_num_reads() == n_reads);
        REQUIRE(writer.get_num_reads() == n_reads);

        REQUIRE_NOTHROW(writer.close());
        REQUIRE(filecmp(infile, outfile));
    }

    SECTION("Paired-ended round trip") {
        qcpp::ReadPair pair;

        while (parser.parse_read_pair(pair)) {
            REQUIRE_NOTHROW(writer.write_read_pair(pair));
            n_reads++;
        }

        REQUIRE(n_reads == 5); // Pairs, not reads
        REQUIRE(parser.get_num_reads() == 10);
        REQUIRE(writer.get_num_reads() == 10);

        REQUIRE_NOTHROW(writer.close());
        REQUIRE(filecmp(infile, outfile));
    }
}
