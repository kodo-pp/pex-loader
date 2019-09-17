#define CATCH_CONFIG_MAIN
#define CATCH_CONFIG_FAST_COMPILE
#include <catch.hpp>

#include <pex_loader/pex_loader.hpp>

#include <list>
#include <string_view>
#include <vector>


using namespace std::literals;
using Catch::Matchers::Predicate;


TEST_CASE("v0::read_sections is working", "[read_sections]") {
    using namespace pex::loader;
    SECTION("0 sections") {
        auto blob = (
            "\x00\x00\x00\x00\x00\x00\x00\x00"
            ""sv
        );
        CHECK(v0::read_sections(blob).size() == 0);
    }
    SECTION("invalid section number") {
        auto blob = (
            "\x00\x00\x00\x00\x00\x00\x00"
            ""sv
        );
        REQUIRE_THROWS(v0::read_sections(blob));
    }
    SECTION("3 sections, valid") {
        auto blob = (
            "\x00\x00\x00\x00\x00\x00\x00\x03"
            
            // Section 0
            // Size: 5 + 4 = 9
            "\x00\x00\x00\x00\x00\x00\x00\x09"
            // Name: 1234
            "1234"
            // Offset: 8 + 8 + 4 = 20
            // Data
            "Hello"

            // Section 1
            // Size: 0 + 4 = 4
            "\x00\x00\x00\x00\x00\x00\x00\x04"
            // Name: test
            "test"
            // Offset: (20 + 5) + 8 + 4 = 37
            // Zero-length data

            // Section 2
            // Size: 16 + 4 = 30
            "\x00\x00\x00\x00\x00\x00\x00\x14"
            // Name "\x00\x01\x02\x03"
            "\x00\x01\x02\x03"
            // Offset: (37 + 0) + 8 + 4 = 49
            // Data
            "0123456789abcdef"

            ""sv
        );

        auto sections = v0::read_sections(blob);
        REQUIRE(sections.size() == 3);
        CHECK(sections[0].name == std::array<char, 4>{'1', '2', '3', '4'});
        CHECK(sections[0].size == 5);
        CHECK(sections[0].offset == 20);

        CHECK(sections[1].name == std::array<char, 4>{'t', 'e', 's', 't'});
        CHECK(sections[1].size == 0);
        CHECK(sections[1].offset == 37);

        CHECK(sections[2].name == std::array<char, 4>{0, 1, 2, 3});
        CHECK(sections[2].size == 16);
        CHECK(sections[2].offset == 49);
    }
    SECTION("3 sections, EOF in data") {
        auto blob = (
            "\x00\x00\x00\x00\x00\x00\x00\x03"
            
            // Section 0
            // Size: 5
            "\x00\x00\x00\x00\x00\x00\x00\x05"
            // Name: 1234
            "1234"
            // Offset: 8 + 8 + 4 = 20
            // Data
            "Hello"

            // Section 1
            // Size: 0
            "\x00\x00\x00\x00\x00\x00\x00\x00"
            // Name: test
            "test"
            // Offset: (20 + 5) + 8 + 4 = 37
            // Zero-length data

            // Section 2
            // Size: 16
            "\x00\x00\x00\x00\x00\x00\x00\x10"
            // Name "\x00\x01\x02\x03"
            "\x01\x02\x03\x04"
            // Offset: (37 + 0) + 8 + 4 = 59
            // Data
            "0123456789abcde"   // Oops... 1 byte is missing

            ""sv
        );

        REQUIRE_THROWS(v0::read_sections(blob));
    }
    SECTION("3 sections, 1 absent") {
        auto blob = (
            "\x00\x00\x00\x00\x00\x00\x00\x03"
            
            // Section 0
            // Size: 5
            "\x00\x00\x00\x00\x00\x00\x00\x05"
            // Name: 1234
            "1234"
            // Offset: 8 + 8 + 4 = 20
            // Data
            "Hello"

            // Section 1
            // Size: 0
            "\x00\x00\x00\x00\x00\x00\x00\x00"
            // Name: test
            "test"
            // Offset: (20 + 5) + 8 + 4 = 37
            // Zero-length data

            // A section is missing

            ""sv
        );

        REQUIRE_THROWS(v0::read_sections(blob));
    }
}
