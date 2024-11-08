// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <_coro.hpp>

namespace coro = real::coro;

TEST(coro_tests, sync_wait) {
  int side_effect = 0;
  constexpr auto lazy_run = [](int& side_effect) -> coro::lazy<int> {
    side_effect = 1;
    co_return 0;
  };
  auto task = lazy_run(side_effect);
  EXPECT_EQ(side_effect, 0);
  coro::sync_wait(std::move(task));
  EXPECT_EQ(side_effect, 1);
}
