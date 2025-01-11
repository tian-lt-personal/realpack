// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>
#include <s.hpp>

TEST(s_tests, frac_n) {
  { auto num = real::frac_n(real::create_z(0), real::create_z(1), 2); }
}
