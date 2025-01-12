// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include <r.hpp>
#include <z.hpp>

// util headers
#include <_to.hpp>

namespace {

using small = std::vector<uint8_t>;
using middle = std::vector<uint16_t>;

}  // namespace

TEST(to_tests, to_decimal_string) {
  {
    auto text = real::to_decimal_string(real::create_z<small>(0));
    EXPECT_EQ(text, "0");
  }
  {
    auto text = real::to_decimal_string(real::create_z<small>(-1));
    EXPECT_EQ(text, "-1");
  }
  {
    auto text = real::to_decimal_string(real::create_z<small>(256));
    EXPECT_EQ(text, "256");
  }
  {
    auto text = real::to_decimal_string(real::create_z<small>(100000));
    EXPECT_EQ(text, "100000");
  }
  {
    auto text = real::to_decimal_string(real::create_z<small>(-65536));
    EXPECT_EQ(text, "-65536");
  }
  {
    auto text = real::to_decimal_string(real::create_z<small>(7162394761923ull));
    EXPECT_EQ(text, "7162394761923");
  }
  {
    auto text = real::to_decimal_string(real::create_z<middle>("-1"));
    EXPECT_EQ(text, "-1");
  }
  {
    auto text =
        real::to_decimal_string(real::create_z<middle>("-872346918726349817236412374091823410982340198237410928374"));
    EXPECT_EQ(text, "-872346918726349817236412374091823410982340198237410928374");
  }
  {
    auto text = real::to_decimal_string(real::create_z<middle>("-10000000000000000000000000000000000"));
    EXPECT_EQ(text, "-10000000000000000000000000000000000");
  }
  {
    auto text = real::to_decimal_string(real::create_z(0));
    EXPECT_EQ(text, "0");
  }
  {
    auto text = real::to_decimal_string(real::create_z(-0));
    EXPECT_EQ(text, "0");
  }
  {
    auto text = real::to_decimal_string(real::create_z(1000));
    EXPECT_EQ(text, "1000");
  }
  {
    auto text = real::to_decimal_string(real::create_z("000000000000000000000000000001234"));
    EXPECT_EQ(text, "1234");
  }
  {
    auto text = real::to_decimal_string(real::create_z(
        "98712349871203984710298374102983741098237401892734098127304981256769345276345690871348546364574754695648076507"
        "54830758430658493086587342947653679439876754862316943476453296785432976098542324980563472894650369727549761394"
        "84367849367845360783282737857282911945367568377964276045279864647924593407615436751045103489"));
    EXPECT_EQ(
        text,
        "98712349871203984710298374102983741098237401892734098127304981256769345276345690871348546364574754695648076507"
        "54830758430658493086587342947653679439876754862316943476453296785432976098542324980563472894650369727549761394"
        "84367849367845360783282737857282911945367568377964276045279864647924593407615436751045103489");
  }
}

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
