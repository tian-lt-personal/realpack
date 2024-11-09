// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>
#include <_op.hpp>

TEST(op_tests, add) {
  {
    auto zero = real::create_z(0);
    auto one = real::create_z(1);
    auto res = zero + one;
    EXPECT_EQ(real::cmp_z(one, res), 0);
  }
}

TEST(op_tests, sub) {
  {
    auto zero = real::create_z(0);
    auto one = real::create_z(1);
    auto exp = real::create_z(-1);
    auto res = zero - one;
    EXPECT_EQ(real::cmp_z(exp, res), 0);
  }
}

TEST(op_tests, mul) {
  {
    auto one = real::create_z(1);
    auto res = one * one;
    EXPECT_EQ(real::cmp_z(one, res), 0);
  }
}

TEST(op_tests, div) {
  {
    auto seven = real::create_z(7);
    auto four = real::create_z(4);
    auto exp = real::create_z(1);
    auto q = seven / four;
    EXPECT_EQ(real::cmp_z(q, exp), 0);
  }
}

TEST(op_tests, mod) {
  {
    auto seven = real::create_z(7);
    auto four = real::create_z(4);
    auto exp = real::create_z(3);
    auto q = seven % four;
    EXPECT_EQ(real::cmp_z(q, exp), 0);
  }
}
