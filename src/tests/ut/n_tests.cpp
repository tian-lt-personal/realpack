// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

namespace {

using small = std::vector<unsigned char>;
using middle = std::vector<unsigned short>;

template <class C>
constexpr auto pow_of_2(unsigned pwr) {
  auto res = real::identity<C>();
  auto two = real::add(real::identity<C>(), real::identity<C>());
  for (auto i = 0u; i < pwr; ++i) {
    res = real::mul(res, two);
  }
  return res;
}
static_assert([] {
  auto num = pow_of_2<small>(0);
  auto one = real::identity<small>();
  return real::cmp(num, one) == 0;
}());
static_assert([] {
  auto num = pow_of_2<small>(1);
  auto two = real::add(real::identity<small>(), real::identity<small>());
  return real::cmp(num, two) == 0;
}());
static_assert([] {
  auto num = pow_of_2<small>(2);
  auto two = real::add(real::identity<small>(), real::identity<small>());
  auto four = real::add(two, two);
  return real::cmp(num, four) == 0;
}());

}  // namespace

TEST(n_tests, details_umul) {
  {
    unsigned char o;
    auto prod = real::details::umul<unsigned long, unsigned char>(0, 0, o);
    EXPECT_EQ(o, 0);
    EXPECT_EQ(prod, 0);
  }
  {
    unsigned char o;
    auto prod = real::details::umul<unsigned long, unsigned char>(1, 0, o);
    EXPECT_EQ(o, 0);
    EXPECT_EQ(prod, 0);
    prod = real::details::umul<unsigned long, unsigned char>(0, 1, o);
    EXPECT_EQ(o, 0);
    EXPECT_EQ(prod, 0);
  }
  {
    unsigned char o;
    auto prod = real::details::umul<unsigned long, unsigned char>(1, 1, o);
    EXPECT_EQ(o, 0);
    EXPECT_EQ(prod, 1);
    prod = real::details::umul<unsigned long, unsigned char>(1, 1, o);
    EXPECT_EQ(o, 0);
    EXPECT_EQ(prod, 1);
  }
  {
    unsigned char o;
    auto prod = real::details::umul<unsigned long, unsigned char>(255, 254, o);
    EXPECT_EQ(o, 253);
    EXPECT_EQ(prod, 2);
    prod = real::details::umul<unsigned long, unsigned char>(254, 255, o);
    EXPECT_EQ(o, 253);
    EXPECT_EQ(prod, 2);
  }
  {
    unsigned char o;
    auto prod = real::details::umul<unsigned char, unsigned char>(255, 254, o);
    EXPECT_EQ(o, 253);
    EXPECT_EQ(prod, 2);
    prod = real::details::umul<unsigned char, unsigned char>(254, 255, o);
    EXPECT_EQ(o, 253);
    EXPECT_EQ(prod, 2);
  }
}

TEST(n_tests, details_nlz) {
  EXPECT_EQ(real::details::nlz(0ul), 32);
  EXPECT_EQ(real::details::nlz(1ul), 31);
  EXPECT_EQ(real::details::nlz(356ul), 23);
  EXPECT_EQ(real::details::nlz(213123ul), 14);
  EXPECT_EQ(real::details::nlz(84627813ul), 5);
  EXPECT_EQ(real::details::nlz(0xffffffffu), 0);

  EXPECT_EQ(real::details::nlz<unsigned char>(0), 8);
  EXPECT_EQ(real::details::nlz<unsigned char>(1), 7);
  EXPECT_EQ(real::details::nlz<unsigned char>(123), 1);
  EXPECT_EQ(real::details::nlz<unsigned char>(0xff), 0);

  EXPECT_EQ(real::details::nlz<unsigned short>(0), 16);
  EXPECT_EQ(real::details::nlz<unsigned short>(1), 15);
  EXPECT_EQ(real::details::nlz<unsigned short>(1234), 5);
  EXPECT_EQ(real::details::nlz<unsigned short>(0xffff), 0);
}

TEST(n_tests, details_bit_shift) {
  {
    real::z<small> zero;
    real::details::bit_shift<typename small::value_type>(zero.digits, 0);
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    real::z<small> zero;
    real::details::bit_shift<typename small::value_type>(zero.digits, 6);
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto num = real::identity<small>();
    auto expected = real::mul(num, pow_of_2<small>(7));
    real::details::bit_shift<typename small::value_type>(num.digits, 7);
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
  {
    real::z<small> num;
    real::init(num, 8365473u);
    auto expected = real::mul(num, pow_of_2<small>(3));
    auto cy = real::details::bit_shift<typename small::value_type>(num.digits, 3);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
  {
    real::z<small> num;
    real::init(num, 99382171723ull);
    auto expected = real::mul(num, pow_of_2<small>(6));
    auto cy = real::details::bit_shift<typename small::value_type>(num.digits, 6);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
  {
    real::z<middle> num;
    real::init(num, 9382723635117365ull);
    auto expected = real::mul(num, pow_of_2<middle>(6));
    auto cy = real::details::bit_shift<typename middle::value_type>(num.digits, 6);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
  {
    real::z<small> expected;
    real::init(expected, 1234u);
    auto num = real::mul(expected, pow_of_2<small>(7));
    auto cy = real::details::bit_shift<typename small::value_type>(num.digits, -7);
    real::norm_n(num);
    EXPECT_EQ(cy, 0);
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
  {
    real::z<small> expected;
    real::init(expected, 7462981237123ull);
    auto num = real::mul(expected, pow_of_2<small>(4));
    auto cy = real::details::bit_shift<typename small::value_type>(num.digits, -4);
    real::norm_n(num);
    EXPECT_EQ(cy, 0);
    EXPECT_EQ(real::cmp(num, expected), 0);
  }
}

TEST(n_tests, cmp_n) {
  real::z zero;
  {
    real::z num;
    EXPECT_EQ(real::cmp_n(num, zero), 0);
    EXPECT_EQ(real::cmp_n(zero, num), 0);
  }
  {
    real::z num;
    real::init(num, 1);
    EXPECT_GT(real::cmp_n(num, zero), 0);
    EXPECT_LT(real::cmp_n(zero, num), 0);
  }
  {
    real::z a, b;
    real::init(a, 654321);
    real::init(b, 664321);
    EXPECT_LT(real::cmp_n(a, b), 0);
    EXPECT_GT(real::cmp_n(b, a), 0);
  }
}

TEST(n_tests, norm_n) {
  {
    real::z zero;
    real::norm_n(zero);
    EXPECT_TRUE(real::is_zero(zero));
    EXPECT_FALSE(zero.sign);
    EXPECT_TRUE(zero.digits.empty());
  }
  {
    real::z zero{.sign = true};
    for (auto _ : std::views::iota(1, 10)) {
      zero.digits.push_back(0);
    }
    real::norm_n(zero);
    EXPECT_TRUE(real::is_zero(zero));
    EXPECT_FALSE(zero.sign);
    EXPECT_TRUE(zero.digits.empty());
  }
  {
    real::z num, expected;
    real::init(num, -12345);
    expected = num;
    for (auto _ : std::views::iota(1, 20)) {
      num.digits.push_back(0);
    }
    real::norm_n(num);
    EXPECT_EQ(real::cmp_n(num, expected), 0);
    EXPECT_EQ(num.sign, expected.sign);
    EXPECT_EQ(num.digits.size(), expected.digits.size());
  }
}

TEST(n_tests, shift_n) {
  {
    real::z zero;
    real::shift_n(zero, 0);
    EXPECT_TRUE(real::is_zero(zero));
    EXPECT_FALSE(zero.sign);
    EXPECT_TRUE(zero.digits.empty());
  }
  {
    real::z zero;
    real::shift_n(zero, 0, true);
    EXPECT_TRUE(real::is_zero(zero));
    EXPECT_FALSE(zero.sign);
    EXPECT_TRUE(zero.digits.empty());
  }
  {
    real::z<small> num, expected;
    real::init(expected, 256);
    real::init(num, 1);
    real::shift_n(num, 1);
    EXPECT_EQ(real::cmp_n(num, expected), 0);
  }
  {
    real::z<small> num, expected;
    real::init(expected, 65536);
    real::init(num, 1);
    real::shift_n(num, 2);
    EXPECT_EQ(real::cmp_n(num, expected), 0);
  }
  {
    real::z<small> num;
    real::init(num, 1);
    real::shift_n(num, 1, true);
    EXPECT_TRUE(real::is_zero(num));
  }
}

TEST(n_tests, digit_n) {
  {
    real::z zero;
    EXPECT_EQ(real::digit_n(zero, 0), 0);
    EXPECT_EQ(real::digit_n(zero, 100), 0);
  }
  {
    real::z<small> num;
    real::init(num, 41312);
    EXPECT_EQ(real::digit_n(num, 0), 0x60);
    EXPECT_EQ(real::digit_n(num, 1), 0xa1);
    EXPECT_EQ(real::digit_n(num, 2), 0);
  }
}
TEST(n_tests, add_n) {
  {
    real::z<small> num, zero;
    real::init(num, 12345u);
    auto sum = real::add_n(num, zero);
    EXPECT_EQ(real::cmp_n(sum, num), 0);
    sum = real::add_n(zero, num);
    EXPECT_EQ(real::cmp_n(sum, num), 0);
  }
  {
    real::z<small> num, one, expected;
    real::init(one, 1u);
    real::init(num, 12345u);
    real::init(expected, 12346u);
    auto sum = real::add_n(num, one);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
    sum = real::add_n(one, num);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
  }
  {
    real::z<small> num, num2, expected;
    real::init(num, 65536u);
    real::init(num2, 262144u);
    real::init(expected, 327680u);
    auto sum = real::add_n(num, num2);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
    sum = real::add_n(num2, num);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
  }
  {
    real::z<small> num, expected;
    real::init(num, 12345u);
    real::init(expected, 24690u);
    auto sum = real::add_n(num, num);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
  }
  {
    real::z num, expected;
    real::init(num, 625373805u);
    real::init(expected, 1250747610u);
    auto sum = real::add_n(num, num);
    EXPECT_EQ(real::cmp_n(sum, expected), 0);
  }
}

TEST(n_tests, sub_n) {
  {
    real::z<small> num, zero;
    real::init(num, 12345u);
    auto diff = real::sub_n(num, zero);
    EXPECT_EQ(real::cmp_n(diff, num), 0);
  }
  {
    real::z<small> num, one, expected;
    real::init(one, 1u);
    real::init(num, 12345u);
    real::init(expected, 12344u);
    auto diff = real::sub_n(num, one);
    EXPECT_EQ(real::cmp_n(diff, expected), 0);
  }
  {
    real::z<small> minuend, subtrahend, expected;
    real::init(minuend, 1323065u);
    real::init(subtrahend, 12346u);
    real::init(expected, 1310719u);
    auto diff = real::sub_n(minuend, subtrahend);
    EXPECT_EQ(real::cmp_n(diff, expected), 0);
  }
  {
    real::z<small> minuend, subtrahend, expected;
    real::init(minuend, 928392780u);
    real::init(subtrahend, 928269324u);
    real::init(expected, 123456u);
    auto diff = real::sub_n(minuend, subtrahend);
    EXPECT_EQ(real::cmp_n(diff, expected), 0);
  }
  {
    real::z<small> sum, num;
    real::init(num, 9382239483u);
    for (auto _ : std::views::iota(0, 1000)) {
      sum = real::add_n(sum, num);
    }
    for (auto _ : std::views::iota(0, 1000)) {
      sum = real::sub_n(sum, num);
    }
    EXPECT_TRUE(real::is_zero(sum));
  }
}

TEST(n_tests, mul_n) {
  {
    real::z<small> zero;
    real::mul_n(zero, zero);
  }
  {
    real::z<small> num, zero;
    real::init(num, 54321u);
    auto prod = real::mul_n(num, zero);
    EXPECT_TRUE(real::is_zero(prod));
    prod = real::mul_n(zero, num);
    EXPECT_TRUE(real::is_zero(prod));
  }
  {
    real::z<small> num, one;
    real::init(one, 1u);
    real::init(num, 54321u);
    auto prod = real::mul_n(num, one);
    EXPECT_EQ(real::cmp_n(num, prod), 0);
    prod = real::mul_n(one, num);
    EXPECT_EQ(real::cmp_n(num, prod), 0);
  }
  {
    real::z<small> num, num2, expected;
    real::init(num, 65536u);
    real::init(num2, 65536u);
    real::init(expected, 4294967296ui64);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    prod = real::mul_n(num2, num);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
  }
  {
    real::z<small> num, num2, expected;
    real::init(num, 63847u);
    real::init(num2, 19383u);
    real::init(expected, 1237546401ui64);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    prod = real::mul_n(num2, num);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
  }
  {
    real::z<small> num, num2, expected;
    real::init(num, 8463732u);
    real::init(num2, 2847463u);
    real::init(expected, 24100163711916ui64);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    prod = real::mul_n(num2, num);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
  }
  {
    real::z<middle> num, num2, expected;
    real::init(num, 8463732u);
    real::init(num2, 2847463u);
    real::init(expected, 24100163711916ui64);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    prod = real::mul_n(num2, num);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
  }
  {
    real::z num, num2, expected;
    real::init(num, 8473u);
    real::init(num2, 99281u);
    real::init(expected, 841207913u);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    prod = real::mul_n(num2, num);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
  }
  {
    real::z num, num2, sum, expected;
    real::init(num, 9384u);
    real::init(num2, 281942u);
    real::init(expected, 2645743728u);
    auto prod = real::mul_n(num, num2);
    EXPECT_EQ(real::cmp_n(prod, expected), 0);
    for (auto _ : std::views::iota(0, 9384)) {
      sum = real::add(sum, num2);
    }
    EXPECT_EQ(real::cmp_n(prod, sum), 0);
  }
}

TEST(n_tests, short_div_n) {
  {
    real::z<small> zero;
    unsigned char r;
    auto q = real::div_n(zero, 1, &r);
    EXPECT_TRUE(real::is_zero(q));
    EXPECT_EQ(r, 0);
  }
  {
    auto one = real::identity<small>();
    unsigned char r;
    auto q = real::div_n(one, 1, &r);
    EXPECT_EQ(real::cmp_n(q, one), 0);
    EXPECT_EQ(r, 0);
  }
  {
    real::z<small> num;
    real::init(num, 256);
    unsigned char r;
    auto q = real::div_n(num, 255u, &r);
    EXPECT_EQ(real::cmp_n(q, real::identity<small>()), 0);
    EXPECT_EQ(r, 1u);
  }
  {
    real::z<small> num, expected;
    real::init(num, 837261932u);
    real::init(expected, 4839664u);
    unsigned char r;
    auto q = real::div_n(num, 173u, &r);
    EXPECT_EQ(real::cmp_n(q, expected), 0);
    EXPECT_EQ(r, 60u);
  }
  {
    real::z<small> num, expected;
    real::init(num, 123878293827291834ull);
    real::init(expected, 485797230695262ull);
    unsigned char r;
    auto q = real::div_n(num, 255, &r);
    EXPECT_EQ(real::cmp_n(q, expected), 0);
    EXPECT_EQ(r, 24u);
  }
  {
    real::z<middle> num, expected;
    real::init(num, 892387334912834125ull);
    real::init(expected, 14193264861673u);
    unsigned short r;
    auto q = real::div_n(num, 62874, &r);
    EXPECT_EQ(real::cmp_n(q, expected), 0);
    EXPECT_EQ(r, 5923u);
  }
  {
    real::z num, expected;
    num.digits.push_back(8237234u);
    num.digits.push_back(2829247u);
    num.digits.push_back(9238412u);
    expected.digits.push_back(3346301246u);
    expected.digits.push_back(41838672u);
    unsigned long r;
    auto q = real::div_n(num, 948373234u, &r);
    EXPECT_EQ(real::cmp_n(q, expected), 0);
    EXPECT_EQ(r, 27398166u);
  }
}

TEST(n_tests, div_n) {
  //{
  //  real::z<small> zero, one;
  //  real::init(one, 1);
  //  real::div_n<small>(zero, one, nullptr);
  //}
  //{
  //  real::z<small> u, v;
  //  real::init(u, 8347330149823u);
  //  real::init(v, 182734u);
  //  real::div_n<small>(u, v, nullptr);
  //}
}
