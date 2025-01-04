// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <r.hpp>
#include <z.hpp>

// util headers
#include <_to.hpp>

TEST(to_tests, z_to_int) {}

TEST(to_tests, r_to_int) {
  {
    auto eval = real::to_int(real::expr_q(real::identity()), 10);
    auto res = real::coro::sync_get(std::move(eval));
    (void)res;  // TODO
  }
}

TEST(to_tests, r_to_double) {
  {
    auto eval = real::to_double(real::expr_q(real::identity()), 10);
    auto res = real::coro::sync_get(std::move(eval));
    (void)res;  // TODO
  }
}
