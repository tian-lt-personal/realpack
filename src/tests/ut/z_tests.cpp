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

TEST(z_tests, identity) {
  {
    auto one = real::identity<real::details::z_default_container>();
    EXPECT_FALSE(real::is_zero(one));
  }
  {
    real::z expected;
    real::init(expected, 1u);
    auto one = real::identity<real::details::z_default_container>();
    EXPECT_EQ(real::cmp(one, expected), 0);
  }
}

TEST(z_tests, z_view) {
  {
    real::z num;
    real::z_view zv{num};
    real::z_view zv2 = num;
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
  {
    real::z num;
    real::init(num, 4294967296ull);
    EXPECT_FALSE(num.sign);
    EXPECT_EQ(num.digits.size(), 2);
    EXPECT_EQ(num.digits[0], 0u);
    EXPECT_EQ(num.digits[1], 1u);
  }
  {
    real::z num;
    real::init(num, 943988731981237123ull);
    EXPECT_FALSE(num.sign);
    EXPECT_EQ(num.digits.size(), 2);
    EXPECT_EQ(num.digits[0], 297175939u);
    EXPECT_EQ(num.digits[1], 219789504u);
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
  {
    real::z num;
    real::init(num, -4294967297ll);
    EXPECT_TRUE(num.sign);
    EXPECT_EQ(num.digits.size(), 2);
    EXPECT_EQ(num.digits[0], 1u);
    EXPECT_EQ(num.digits[1], 1u);
  }
}

TEST(z_tests, init_decstr) {
  // TODO
}

TEST(z_tests, add) {
  {
    real::z zero;
    auto sum = real::add(zero, zero);
    EXPECT_TRUE(real::is_zero(sum));
  }
  {
    real::z zero;
    real::z num;
    real::init(num, 123456);
    auto sum = real::add(num, zero);
    EXPECT_EQ(real::cmp(sum, num), 0);
  }
  {
    using zch = real::z<std::vector<unsigned char>>;
    zch num;
    zch expected;
    real::init(num, 123456u);
    real::init(expected, 246912u);
    auto sum = real::add(num, num);
    EXPECT_EQ(real::cmp(sum, expected), 0);
  }
  {
    using zch = real::z<std::vector<unsigned char>>;
    zch num, num2, zero;
    real::init(num, 123456u);
    real::init(num2, -123456);
    auto sum = real::add(num, num2);
    EXPECT_EQ(real::cmp(sum, zero), 0);
    sum = real::add(num2, num);
    EXPECT_EQ(real::cmp(sum, zero), 0);
  }
  {
    using zch = real::z<std::vector<unsigned char>>;
    zch num, num2;
    zch expected;
    real::init(num, 123456u);
    real::init(num2, -928392780);
    real::init(expected, -928269324);
    auto sum = real::add(num, num2);
    EXPECT_EQ(real::cmp(sum, expected), 0);
  }
}

TEST(z_tests, mul) {
  {
    real::z zero;
    auto prod = real::mul(zero, zero);
    EXPECT_TRUE(real::is_zero(prod));
  }
  {
    real::z zero, num;
    real::init(num, 123456);
    auto prod = real::mul(num, zero);
    EXPECT_TRUE(real::is_zero(prod));
    prod = real::mul(zero, num);
    EXPECT_TRUE(real::is_zero(prod));
  }
  {
    real::z one, num;
    real::init(one, 1);
    real::init(num, 123456);
    auto prod = real::mul(num, one);
    EXPECT_EQ(real::cmp(prod, num), 0);
    prod = real::mul(one, num);
    EXPECT_EQ(real::cmp(prod, num), 0);
  }
  {
    real::z minus_one, num;
    real::init(minus_one, -1);
    real::init(num, 123456);
    auto prod = real::mul(num, minus_one);
    EXPECT_TRUE(real::is_zero(real::add(num, prod)));
    prod = real::mul(minus_one, num);
    EXPECT_TRUE(real::is_zero(real::add(num, prod)));
  }
  {
    real::z num, num2, expected;
    real::init(num, 123456);
    real::init(num2, -4321);
    real::init(expected, -533453376);
    auto prod = real::mul(num, num2);
    EXPECT_EQ(real::cmp(prod, expected), 0);
    prod = real::mul(num2, num);
    EXPECT_EQ(real::cmp(prod, expected), 0);
  }
}

TEST(z_tests, div) {
  {
    real::z u, v, r, expected_r;
    real::init(u, 11);
    real::init(v, 7);
    real::init(expected_r, 4);
    auto q = real::div(u, v, &r);
    EXPECT_EQ(real::cmp(q, real::identity()), 0);
    EXPECT_EQ(real::cmp(r, expected_r), 0);
  }
  {
    real::z u, v, r, expected_r, expected_q;
    real::init(u, 11);
    real::init(v, -7);
    real::init(expected_r, -3);
    real::init(expected_q, -1);
    auto q = real::div(u, v, &r);
    EXPECT_EQ(real::cmp(q, expected_q), 0);
    EXPECT_EQ(real::cmp(r, expected_r), 0);
  }
  {
    real::z u, v, r, expected_r, expected_q;
    real::init(u, -11);
    real::init(v, 7);
    real::init(expected_r, 3);
    real::init(expected_q, -1);
    auto q = real::div(u, v, &r);
    EXPECT_EQ(real::cmp(q, expected_q), 0);
    EXPECT_EQ(real::cmp(r, expected_r), 0);
  }
  {
    real::z u, v, r, expected_r;
    real::init(u, -11);
    real::init(v, -7);
    real::init(expected_r, -4);
    auto q = real::div(u, v, &r);
    EXPECT_EQ(real::cmp(q, real::identity()), 0);
    EXPECT_EQ(real::cmp(r, expected_r), 0);
  }
}
