// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

namespace {
namespace z_tests_is_zero {

static_assert(real::is_zero(real::z{}));
static_assert([] {
  real::z num;
  real::init(num, 1u);
  return !real::is_zero(num);
}());
static_assert([] {
  real::z num;
  real::init(num, -2);
  return !real::is_zero(num);
}());

}  // namespace z_tests_is_zero
}  // namespace
TEST(z_tests, is_zero) {
  {
    real::z num;
    EXPECT_TRUE(real::is_zero(num));
  }
  {
    real::z num;
    real::init(num, 1u);
    EXPECT_FALSE(real::is_zero(num));
  }
}

TEST(z_tests, init_unsigned) {
  {
    real::z num;
    real::init(num, 0u);
    EXPECT_TRUE(num.digits.empty());
    EXPECT_FALSE(num.sign);
  }
  {
    real::z num;
    real::init(num, 1u);
    EXPECT_FALSE(num.sign);
    EXPECT_EQ(num.digits.size(), 1);
    EXPECT_EQ(num.digits[0], 1);
  }
  {
    real::z<std::vector<unsigned char>> num;
    real::init(num, 257u);
    EXPECT_FALSE(num.sign);
    EXPECT_EQ(num.digits.size(), 2);
    EXPECT_EQ(num.digits[0], 1u);
    EXPECT_EQ(num.digits[1], 1u);
  }
}

TEST(z_tests, init_signed) {
  {
    real::z num;
    real::init(num, 0);
    EXPECT_TRUE(num.digits.empty());
    EXPECT_FALSE(num.sign);
  }
  {
    real::z num;
    real::init(num, -1);
    EXPECT_TRUE(num.sign);
    EXPECT_EQ(num.digits.size(), 1);
    EXPECT_EQ(num.digits[0], 1);
  }
  {
    real::z<std::vector<unsigned char>> num;
    real::init(num, -257);
    EXPECT_TRUE(num.sign);
    EXPECT_EQ(num.digits.size(), 2);
    EXPECT_EQ(num.digits[0], 1u);
    EXPECT_EQ(num.digits[1], 1u);
  }
}

TEST(z_tests, init_decstr) {
  // TODO
}
