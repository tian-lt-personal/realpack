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
  // TODO: FIXIT
  //{
  //  auto expected = real::create_z<small>(-1234);
  //  auto num = real::create_z<small>(" -1234");
  //  EXPECT_EQ(real::cmp_z(num, expected), 0);
  //}
}
