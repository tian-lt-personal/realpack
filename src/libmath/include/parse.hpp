#ifndef REAL_MATH_PARSE_HPP
#define REAL_MATH_PARSE_HPP

#include <expected>
#include <istream>
#include <optional>
#include <string>

namespace real::math::parse {

enum struct token_type { value, id, plus, minus, mul, div, perc, lparen, rparen, exp, eql };
enum struct token_error_code { eof, bad_token, stream_error };

struct token_error {
  token_error_code code;
  std::optional<std::string> what;
};

struct token {
  token_type type;
  std::optional<std::string> str; // TODO: use range
};

class tokenizer {
 private:
  std::istream& in_;
  std::streampos rmark_ = {};

 public:
  explicit tokenizer(std::istream& in) : in_(in) {};
  std::expected<token, token_error> operator()();
};

}  // namespace real::math::parse

#endif  // !REAL_MATH_PARSE_HPP
