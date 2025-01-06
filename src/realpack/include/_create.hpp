#ifndef REALPACK_INC_CREATE_HPP
#define REALPACK_INC_CREATE_HPP

#include "z.hpp"

namespace real {

namespace details {

constexpr bool is_ws(char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f'; }
constexpr bool is_decimal(char ch) { return '0' <= ch && ch <= '9'; }
constexpr std::optional<unsigned> char2decimal(char ch) {
  if (is_decimal(ch))
    return static_cast<unsigned>(ch - '0');
  else
    return std::nullopt;
}
template <class D>
consteval int decimal_parsing_rank() {
  constexpr int ranks[] = {2, 4, 9, -1, 19};
  static_assert(ranks[(sizeof(D)) >> 1] > 0);
  return ranks[(sizeof(D)) >> 1];
}

inline bool parse_sign_opt(std::string_view& str) {
  bool res = false;
  size_t i = 0;
  for (; i < str.length(); ++i) {
    auto ch = str[i];
    if (!is_ws(ch)) {
      if (ch == '-') {
        ++i;
        res = true;
        break;
      } else {
        break;
      }
    } else {
      continue;
    }
  }
  str = str.substr(i);
  return res;
}

template <z_digit_container C = details::z_default_container>
C parse_unsigned_integer(std::string_view& str) {
  // find beg:
  size_t beg = 0;
  for (; beg < str.length(); ++beg) {
    if (!details::is_ws(str[beg])) {
      break;
    }
  }
  if (beg == str.length()) {
    throw z_parse_error{"bad unsigned integer string. (no beg)"};
  }
  // find end:
  size_t end = beg;
  for (; end < str.length(); ++end) {
    if (!details::is_decimal(str[end])) {
      break;
    }
  }
  if (end - beg == 0) {
    throw z_parse_error{"bad unsigned integer string. (empty string)"};
  }
  // string to value
  auto raw = str.substr(beg, end - beg);
  str = str.substr(end);
  C c{};
  constexpr auto rank = details::decimal_parsing_rank<details::digit_t<C>>();
  (void)rank;
  for (auto iter = raw.rbegin(); iter != raw.rend(); ++iter) {
  }
  return c;
}

}  // namespace details

template <z_digit_container C = details::z_default_container>
constexpr z<C> create_z(std::integral auto i) {
  z<C> num;
  init(num, i);
  return num;
}

// z := sign_opt ws_opt digits
template <z_digit_container C = details::z_default_container>
constexpr z<C> create_z(std::string_view str) {
  if (str.empty()) {
    throw z_parse_error{"bad integer number string. (empty string)"};
  }
  z<C> num;
  num.sign = details::parse_sign_opt(str);
  num.digits = details::parse_unsigned_integer(str);
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_CREATE_HPP
