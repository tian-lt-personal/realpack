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

TEST(tokenizer_tests, tokenstream) {
  {
    auto stream = create_stream("a + b = c");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_EQ(t->str, "a");

    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::plus);

    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_EQ(t->str, "b");

    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::eql);

    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_EQ(t->str, "c");

    t = get_token();
    EXPECT_FALSE(t.has_value());
    EXPECT_EQ(t.error().code, parse::token_error_code::eof);
  }
}

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
  {
    auto stream = create_stream("a0");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "a0");
  }
  {
    auto stream = create_stream("_123");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "_123");
  }
  {
    auto stream = create_stream("auwne__8320__");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::id);
    EXPECT_TRUE(t->str.has_value());
    EXPECT_EQ(*t->str, "auwne__8320__");
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

TEST(tokenizer_tests, operators) {
  {
    auto stream = create_stream("+");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::plus);
  }
  {
    auto stream = create_stream("-");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::minus);
  }
  {
    auto stream = create_stream("*");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::mul);
  }
  {
    auto stream = create_stream("/");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::div);
  }
  {
    auto stream = create_stream("%");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::perc);
  }
  {
    auto stream = create_stream("^");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::exp);
  }
  {
    auto stream = create_stream("=");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::eql);
  }
}

TEST(tokenizer_tests, parenthesis) {
  {
    auto stream = create_stream("()");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::lparen);
    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::rparen);
  }
}

TEST(tokenizer_tests, separators) {
  {
    auto stream = create_stream(".,");
    parse::tokenizer get_token{stream};
    auto t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::dot);
    t = get_token();
    EXPECT_TRUE(t.has_value());
    EXPECT_EQ(t->type, parse::token_type::comma);
  }
}
