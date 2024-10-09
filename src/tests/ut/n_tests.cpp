// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

namespace {
namespace n_tests_is_zero {

static_assert(real::is_zero(real::z{}));
static_assert([] {
  real::z num;
  real::init_z(num, 1u);
  return !real::is_zero(num);
}());

}  // namespace n_tests_is_zero
}  // namespace
TEST(n_tests, is_zero) {
  {
    real::z num;
    EXPECT_TRUE(real::is_zero(num));
  }
  {
    real::z num;
    real::init_z(num, 1u);
    EXPECT_FALSE(real::is_zero(num));
  }
}

TEST(n_tests, init_z_unsigned) {
  {
    real::z<std::vector<unsigned>, 10> dec;
    real::init_z(dec, 0u);
    EXPECT_TRUE(dec.digits.empty());
    EXPECT_FALSE(dec.sign);
  }
  {
    real::z<std::vector<unsigned char>, 10> dec;
    real::init_z(dec, 1u);
    EXPECT_FALSE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 1);
    EXPECT_EQ(dec.digits[0], 1);
  }
  {
    real::z<std::vector<unsigned long long>, 3> dec;
    real::init_z(dec, 5u);
    EXPECT_FALSE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 2);
    EXPECT_EQ(dec.digits[0], 2);
    EXPECT_EQ(dec.digits[1], 1);
  }
  {
    real::z<std::vector<unsigned>, 3> dec;
    real::init_z(dec, 6u);
    EXPECT_FALSE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 2);
    EXPECT_EQ(dec.digits[0], 0);
    EXPECT_EQ(dec.digits[1], 2);
  }
}

TEST(n_tests, init_z_signed) {
  {
    real::z<std::vector<unsigned>, 10> dec;
    real::init_z(dec, 0);
    EXPECT_TRUE(dec.digits.empty());
    EXPECT_FALSE(dec.sign);
  }
  {
    real::z<std::vector<unsigned>, 10> dec;
    real::init_z(dec, -1);
    EXPECT_FALSE(dec.digits.empty());
    EXPECT_TRUE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 1);
    EXPECT_EQ(dec.digits[0], 1);
  }
  {
    real::z<std::vector<unsigned long long>, 3> dec;
    real::init_z(dec, -5);
    EXPECT_TRUE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 2);
    EXPECT_EQ(dec.digits[0], 2);
    EXPECT_EQ(dec.digits[1], 1);
  }
  {
    real::z<std::vector<unsigned>, 3> dec;
    real::init_z(dec, -6);
    EXPECT_TRUE(dec.sign);
    EXPECT_EQ(dec.digits.size(), 2);
    EXPECT_EQ(dec.digits[0], 0);
    EXPECT_EQ(dec.digits[1], 2);
  }
}
