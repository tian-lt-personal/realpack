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

TEST(tokenizer_tests, empty) {
  {
    auto stream = create_stream("");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream("\t");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream("  \n");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream("  \r\n");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream("\r\n\r\n\v\f");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
}

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

TEST(tokenizer_tests, whitespaces) {
  {
    auto stream = create_stream("   ");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream(" \t\nabc");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "abc");
  }
  {
    auto stream = create_stream("\v\fabc  ");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "abc");
    t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
  {
    auto stream = create_stream("\v\fabc  \n\tdef");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "abc");
    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "def");
  }
}
