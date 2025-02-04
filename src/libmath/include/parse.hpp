#ifndef REAL_MATH_PARSE_HPP
#define REAL_MATH_PARSE_HPP

#include <any>
#include <cstdint>
#include <expected>
#include <istream>
#include <optional>
#include <stdexcept>
#include <string>
#include <variant>
#include <vector>

namespace real::math::parse {

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

template <class T>
class pool_ptr {
  friend class pool;

 public:
  T* get() {
    auto& item = (*pool_)[idx_];
    return std::any_cast<T>(&item);
  }

 private:
  std::vector<std::any>* pool_;
  size_t idx_;
};

class pool {
 public:
  template <class T>
  pool_ptr<T> append(T&& object) {
    auto idx = objs_.size();
    objs_.push_back(std::forward<T>(object));
    pool_ptr<T> ptr;
    ptr.pool_ = &objs_;
    ptr.idx_ = idx;
    return ptr;
  }

 private:
  std::vector<std::any> objs_;
};

struct parse_state {
  pool objects;
  std::optional<parse_error> error;
  bool done = false;
};

namespace ast {

struct value {};
struct expr;
using atom = std::variant<value, pool_ptr<expr>>;
struct factor;
struct expr_exp {
  pool_ptr<factor> base;
  pool_ptr<atom> exp;
};
struct factor {};
struct expr {};
using eval_stmt = expr;
using stmt = eval_stmt;
struct compound_stmt {
  std::vector<pool_ptr<stmt>> statements;
};
struct doc {
  pool_ptr<compound_stmt> root;
};

template <class T>
pool_ptr<T> create_node(parse_state* state, T&& node) {
  return state->objects.append(std::forward<T>(node));
}

}  // namespace ast

void parse(std::istream& stream);

}  // namespace real::math::parse

#endif  // !REAL_MATH_PARSE_HPP
