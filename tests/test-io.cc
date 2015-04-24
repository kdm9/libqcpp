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

#include "qc-io.hh"

TEST_CASE("Read access and clearing", "[Read]") {

    qcpp::Read read;

    SECTION("Filling read members works") {
        read.name = "Name";
        read.sequence = "ACGT";
        read.quality = "IIII";

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
}
