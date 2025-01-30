// std headers
#include <sstream>

// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include "parse.hpp"

namespace parse = real::math::parse;

namespace {

std::istringstream create_stream(std::string text) {
  std::istringstream iss{std::move(text)};
  iss.exceptions(std::ios::failbit);
  return iss;
}
}  // namespace

TEST(tokenizer_tests, ids) {
  {
    auto stream = create_stream("abc");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "abc");
  }
}
