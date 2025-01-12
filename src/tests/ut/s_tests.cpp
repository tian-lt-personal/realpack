// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_create.hpp>
#include <s.hpp>

namespace {

using small = std::vector<uint8_t>;
using middle = std::vector<uint16_t>;

}  // namespace

TEST(s_tests, frac_n) {
  { auto num = real::frac_n(real::create_z<small>(3), real::create_z<small>(4), 2); }
}
