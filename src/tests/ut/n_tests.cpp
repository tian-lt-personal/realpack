// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <z.hpp>

TEST(n_tests, is_zero) { EXPECT_TRUE(real::is_zero(real::z{})); }
