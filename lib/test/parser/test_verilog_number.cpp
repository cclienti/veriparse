#include "../helpers/helpers.hpp"
#include <veriparse/parser/verilog_number.hpp>
#include <veriparse/logger/logger.hpp>
#include <gtest/gtest.h>
#include <gmpxx.h>
#include <fstream>
#include <string>

using namespace Veriparse;


TEST(VerilogNumberParserTest, intconst0) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 345;

	ASSERT_EQ(verilog_number.parse("345", mpz, size, base, is_signed, rc), 0);
	ASSERT_EQ(size, -1);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst1) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = -1;

	ASSERT_EQ(verilog_number.parse("4'sd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 4);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst2) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 15;

	ASSERT_EQ(verilog_number.parse("5'sd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 5);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst3) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 15;

	ASSERT_EQ(verilog_number.parse("4'd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 4);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst4) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 7;

	ASSERT_EQ(verilog_number.parse("3'd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 3);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst5) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = -1;

	ASSERT_EQ(verilog_number.parse("3'sd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 3);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst6) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 15;

	ASSERT_EQ(verilog_number.parse("'sd15", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, -1);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst7) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 3735928495;

	ASSERT_EQ(verilog_number.parse("32'hDeAd_BeAf", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 32);
	ASSERT_EQ(base, 16);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst8) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = -559038801;

	ASSERT_EQ(verilog_number.parse("32'SHDeAd_BeAf", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 32);
	ASSERT_EQ(base, 16);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst9) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("fedcba9876543210abcdfedcba9876543210abcd", 16);

	ASSERT_EQ(verilog_number.parse("160'hfedc_ba98_7654_3210_abcd_fedc_ba98_7654_3210_abcd",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 160);
	ASSERT_EQ(base, 16);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst10) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("-6495562832581790688766959896882149561576936499", 10);

	ASSERT_EQ(verilog_number.parse("160'shfedc_ba98_7654_3210_abcd_fedc_ba98_7654_3210_abcd",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 160);
	ASSERT_EQ(base, 16);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst11) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = 342391;

	ASSERT_EQ(verilog_number.parse("21'o01234567", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 21);
	ASSERT_EQ(base, 8);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst12) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref = -1754761;

	ASSERT_EQ(verilog_number.parse("24'sO71234567", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 24);
	ASSERT_EQ(base, 8);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst13) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("fedcba9876543210abcdfedcba9876543210abcd0", 16);

	ASSERT_EQ(verilog_number.parse("164'b11111110110111001011101010011000011101100101010000110010000100001010101111001101111111101101110010111010100110000111011001010100001100100001000010101011110011010000",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 164);
	ASSERT_EQ(base, 2);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst14) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("-103929005321308651020271358350114392985230983984");

	ASSERT_EQ(verilog_number.parse("164'sb11111110110111001011101010011000011101100101010000110010000100001010101111001101111111101101110010111010100110000111011001010100001100100001000010101011110011010000",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 164);
	ASSERT_EQ(base, 2);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst15) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("edcba9876543210abcdfedcba9876543210abcd0", 16);

	ASSERT_EQ(verilog_number.parse("160'b11111110110111001011101010011000011101100101010000110010000100001010101111001101111111101101110010111010100110000111011001010100001100100001000010101011110011010000",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 160);
	ASSERT_EQ(base, 2);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst16) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;
	mpz_class mpz_ref("-103929005321308651020271358350114392985230983984");

	ASSERT_EQ(verilog_number.parse("160'sb11111110110111001011101010011000011101100101010000110010000100001010101111001101111111101101110010111010100110000111011001010100001100100001000010101011110011010000",
	                               mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 160);
	ASSERT_EQ(base, 2);
	ASSERT_EQ(is_signed, true);
	ASSERT_EQ(rc, 0);
	ASSERT_EQ(mpz, mpz_ref);
}

TEST(VerilogNumberParserTest, intconst17) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;

	ASSERT_EQ(verilog_number.parse("32'dz", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 32);
	ASSERT_EQ(base, 10);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 1);
}

TEST(VerilogNumberParserTest, intconst18) {
	ENABLE_LOGGER;

	Parser::VerilogNumber verilog_number;
	mpz_class mpz;
	int size;
	int base;
	bool is_signed;
	int rc;

	ASSERT_EQ(verilog_number.parse("32'h0100_??Xz", mpz, size, base, is_signed, rc, "test.v", 4), 0);
	ASSERT_EQ(size, 32);
	ASSERT_EQ(base, 16);
	ASSERT_EQ(is_signed, false);
	ASSERT_EQ(rc, 1);
}
