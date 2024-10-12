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
}
