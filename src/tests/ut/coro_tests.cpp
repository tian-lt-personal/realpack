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

TEST(coro_tests, lazy) {
  int side_effect = 0;
  constexpr auto lazy_observe = [](int& side_effect) -> coro::lazy<int> {
    constexpr auto lazy_set_2 = [](int& side_effect) -> coro::lazy<int> {
      EXPECT_EQ(side_effect, 0);
      constexpr auto lazy_set_1 = [](int& side_effect) -> coro::lazy<int> {
        EXPECT_EQ(side_effect, 0);
        side_effect = 1;
        co_return 1;
      };
      auto one = co_await lazy_set_1(side_effect);
      EXPECT_EQ(one, 1);
      EXPECT_EQ(side_effect, 1);
      side_effect = 2;
      co_return 2;
    };
    auto two = co_await lazy_set_2(side_effect);
    EXPECT_EQ(two, 2);
    co_return 0;
  };
  EXPECT_EQ(side_effect, 0);
  coro::sync_wait(lazy_observe(side_effect));
  EXPECT_EQ(side_effect, 2);
}
