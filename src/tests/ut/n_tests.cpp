// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

using small = std::vector<unsigned char>;
using middle = std::vector<unsigned short>;

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
  EXPECT_EQ(real::details::nlz(1ul), 31);
  EXPECT_EQ(real::details::nlz(356ul), 23);
  EXPECT_EQ(real::details::nlz(213123ul), 14);
  EXPECT_EQ(real::details::nlz(84627813ul), 5);
  EXPECT_EQ(real::details::nlz(0xffffffffu), 0);

  EXPECT_EQ(real::details::nlz<unsigned char>(1), 7);
  EXPECT_EQ(real::details::nlz<unsigned char>(123), 1);
  EXPECT_EQ(real::details::nlz<unsigned char>(0xff), 0);

  EXPECT_EQ(real::details::nlz<unsigned short>(1), 15);
  EXPECT_EQ(real::details::nlz<unsigned short>(1234), 5);
  EXPECT_EQ(real::details::nlz<unsigned short>(0xffff), 0);
}

TEST(n_tests, pwr2_n) {
  constexpr auto run_small = [](unsigned val, unsigned pwr) {
    real::z<small> num, times;
    real::init(num, val);
    real::init(times, 1ul << pwr);
    auto expected = real::mul_n(num, times);
    auto cy = real::pwr2_n(num, pwr);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp_n(num, expected), 0);
  };
  constexpr auto run_middle = [](unsigned val, unsigned pwr) {
    real::z<middle> num, times;
    real::init(num, val);
    real::init(times, 1ul << pwr);
    auto expected = real::mul_n(num, times);
    auto cy = real::pwr2_n(num, pwr);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp_n(num, expected), 0);
  };
  constexpr auto run = [](unsigned val, unsigned pwr) {
    real::z num, times;
    real::init(num, val);
    real::init(times, 1ul << pwr);
    auto expected = real::mul_n(num, times);
    auto cy = real::pwr2_n(num, pwr);
    if (cy > 0) {
      num.digits.push_back(cy);
    }
    EXPECT_EQ(real::cmp_n(num, expected), 0);
  };

  run_small(256u, 0);
  run_small(256u, 7);
  run_small(257u, 4);
  run_small(12832u, 6);
  run_small(3818123u, 7);

  run_middle(65536u, 15);
  run_middle(38265536u, 10);
  run_middle(988265536u, 3);
  run_middle(732938274u, 15);

  run(0, 0);
  run(1898274u, 31);
  run(27371u, 8);
  run(1938173u, 18);
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

TEST(n_tests, div_n) {
  {
    real::z<small> zero, one;
    real::init(one, 1);
    real::div_n<small>(zero, one, nullptr);
  }
  {
    real::z<small> u, v;
    real::init(u, 8347330149823u);
    real::init(v, 182734u);
    real::div_n<small>(u, v, nullptr);
  }
}
