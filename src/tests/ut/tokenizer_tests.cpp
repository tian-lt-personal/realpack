// std headers
#include <sstream>

// gtest headers
#include <gtest/gtest.h>

// realpack headers
#include "parse.hpp"

namespace parse = real::math::parse;

TEST(tokenizer_tests, ids) {
  {
    std::istringstream iss{"abc"};
    iss.exceptions(std::ios::failbit);
    parse::tokenizer get_token{iss};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(t->str->c_str(), "abc");
  }
}
