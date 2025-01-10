// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>

namespace {

using small = std::vector<uint8_t>;
using middle = std::vector<uint16_t>;

}  // namespace

TEST(create_tests, z_from_unsigned) {
  {
    auto zero = real::create_z(0);
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto one = real::create_z(1);
    EXPECT_EQ(real::cmp_z(one, real::identity()), 0);
  }
}

TEST(create_tests, z_from_string) {
  {
    auto zero = real::create_z("0");
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto one = real::create_z<small>("1");
    EXPECT_EQ(real::cmp_z(one, real::identity<small>()), 0);
  }
  {
    auto expected = real::create_z<small>(11u);
    auto num = real::create_z<small>("11");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-123);
    auto num = real::create_z<small>("-123");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-1234);
    auto num = real::create_z<small>(" -1234");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<small>(-8237239);
    auto num = real::create_z<small>(" -8237239");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<middle>(1234567890ull);
    auto num = real::create_z<middle>("   1234567890");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto expected = real::create_z<middle>(8812349812342376ull);
    expected.sign = true;
    auto num = real::create_z<middle>(" -  8812349812342376");
    EXPECT_EQ(real::cmp_z(num, expected), 0);
  }
  {
    auto num1 = real::create_z(
        "10000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
    auto num2 = real::create_z("10000000000");
    EXPECT_EQ(real::cmp_z(num1, real::pow_z(num2, 10)), 0);
  }
  {
    auto num1 = real::create_z(
        "61823746812376498172634981726394871623987461923874619283746192873641928736419827364192873461928374619287364192"
        "83764192839128374019823740198273401982374019283740192837401982348172634987162349871236984761293874619287364918"
        "2736419253192346918723491827346918726349176239476192837467");
    real::z r;
    real::div_z(num1, real::create_z(2), &r);
    EXPECT_EQ(real::cmp_z(r, real::identity()), 0);
  }
}
