#ifndef REAL_MATH_PARSE_HPP
#define REAL_MATH_PARSE_HPP

#include <any>
#include <cstdint>
#include <expected>
#include <istream>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>

namespace real::math::parse {

namespace ast {

struct compound_stmt {};
struct doc {
  compound_stmt* root;
};

}  // namespace ast

enum struct token_type { value, id, plus, minus, mul, div, perc, lparen, rparen, exp, eql, dot, comma };
enum struct token_error_code { eof, bad_token, stream_error };

struct token_error {
  token_error_code code;
  std::optional<std::string> what;
};

struct token {
  token_type type;
  std::optional<std::string> str;  // TODO: use range
};

class tokenizer {
 private:
  std::istream& in_;
  std::streampos rmark_ = {};

 public:
  explicit tokenizer(std::istream& in) : in_(in) {};
  std::expected<token, token_error> operator()();
};

struct parse_error : std::runtime_error {
  parse_error();
  explicit parse_error(const char* msg);
};

struct parse_state {
  std::set<std::any> objs;
  std::optional<parse_error> error;
  bool done = false;
};

void parse(std::istream& stream);

}  // namespace real::math::parse

#endif  // !REAL_MATH_PARSE_HPP
