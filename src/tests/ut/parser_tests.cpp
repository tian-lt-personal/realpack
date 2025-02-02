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

TEST(parser_tests, sub) {
  {
    auto stream = create_stream("1+2");
    parse::parse(stream);
  }
}

TEST(parser_tests, error_when_empty) {
  {
    bool has_error = false;
    auto stream = create_stream("");
    try {
      parse::parse(stream);
    }
    catch(const parse::parse_error&) {
      has_error = true;
    }
    EXPECT_TRUE(has_error);
  }
  {
    bool has_error = false;
    auto stream = create_stream("\t \n \r\n");
    try {
      parse::parse(stream);
    }
    catch(const parse::parse_error&) {
      has_error = true;
    }
    EXPECT_TRUE(has_error);
  }
}
