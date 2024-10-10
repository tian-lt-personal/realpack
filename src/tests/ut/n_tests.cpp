// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

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
    real::z<std::vector<unsigned char>> num, zero;
    real::init(num, 12345);
    auto r = real::add_n(num, zero);
  }
}
