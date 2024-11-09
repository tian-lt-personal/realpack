// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>

TEST(create_tests, init_from_unsigned) {
  {
    auto zero = real::create_z(0);
    EXPECT_TRUE(real::is_zero(zero));
  }
  {
    auto one = real::create_z(1);
    EXPECT_EQ(real::cmp_z(one, real::identity()), 0);
  }
}
