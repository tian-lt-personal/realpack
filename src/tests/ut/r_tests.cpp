// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <r.hpp>

TEST(r_tests, add) {
  auto q1 = real::expr_q(real::z{}, real::z{});
  auto q2 = real::expr_q(real::z{}, real::z{});
  auto res = real::expr_add(q1, q2);
  real::z val;
  [&]() -> real::coro::forget { val = co_await res(10); }();
}
