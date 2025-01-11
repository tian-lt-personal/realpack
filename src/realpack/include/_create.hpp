#ifndef REALPACK_INC_CREATE_HPP
#define REALPACK_INC_CREATE_HPP

#include <array>

#include "z.hpp"

namespace real {

template <z_digit_container C = details::z_default_container>
constexpr z<C> create_z(std::integral auto i) {
  z<C> num;
  init(num, i);
  return num;
}

namespace details {

template <class C>
constexpr auto ten() {
  z<C> num;
  real::init(num, 10);
  return num;
}

constexpr bool is_ws(char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f'; }
constexpr bool is_decimal(char ch) { return '0' <= ch && ch <= '9'; }
template <class D>
constexpr std::optional<D> char2decimal(char ch) {
  if (is_decimal(ch))
    return static_cast<D>(ch - '0');
  else
    return std::nullopt;
}

template <class D>
consteval unsigned int decimal_parsing_rank() {
  constexpr int ranks[] = {2, 4, 9, -1, 19};
  static_assert(ranks[(sizeof(D)) >> 1] > 0);
  return ranks[(sizeof(D)) >> 1];
}

template <class D>
consteval D pow10_impl(unsigned int exp) {
  D res = 1u;
  for (unsigned int i = 0; i < exp; ++i) {
    res *= 10;
  }
  return res;
}

template <class D>
consteval auto pow10_lookup_table() {
  constexpr auto rank = decimal_parsing_rank<D>() + 1;
  std::array<D, rank> table;
  for (unsigned int i = 0; i < rank; ++i) {
    table[i] = pow10_impl<D>(i);
  }
  return table;
}

template <class D>
constexpr D pow10(unsigned int exp) {
  constexpr auto table = pow10_lookup_table<D>();
  return table[exp];
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
      } else if (ch == '+') {
        ++i;
        res = false;
        break;
      } else if (is_decimal(ch)) {
        break;
      } else {
        throw z_parse_error{"bad sign characters. (unknown character)"};
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
  {  // string to value:
    using D = details::digit_t<C>;
    z<C> sum;
    D d = 0u;
    auto ten = create_z<C>(10u);
    auto raw = str.substr(beg, end - beg);
    str = str.substr(end);
    constexpr auto rank = details::decimal_parsing_rank<D>();
    unsigned int i = 0u;
    for (auto iter = raw.rbegin(); iter != raw.rend(); ++iter, ++i) {
      if (auto val = char2decimal<D>(*iter); val.has_value()) {
        d += static_cast<D>(details::pow10<D>(i % rank) * (*val));
        if (i % rank == rank - 1) {
          sum = real::add_n(real::mul_n(real::pow_n(ten, rank * (i / rank)), create_z<C>(d)), sum);
          d = 0u;
        }
      } else {
        throw z_parse_error{"bad unsigned integer string. (bad number sequence)"};
      }
    }
    if (d > 0u) {
      sum = real::add_n(real::mul_n(real::pow_n(ten, rank * ((i - 1) / rank)), create_z<C>(d)), sum);
    }
    return std::move(sum.digits);
  }
}

}  // namespace details

// z := sign_opt ws_opt digits
template <z_digit_container C = details::z_default_container>
constexpr z<C> create_z(std::string_view str) {
  if (str.empty()) {
    throw z_parse_error{"bad integer number string. (empty string)"};
  }
  z<C> num;
  num.sign = details::parse_sign_opt(str);
  num.digits = details::parse_unsigned_integer<C>(str);
  return num;
}

}  // namespace real

#endif  // !REALPACK_INC_CREATE_HPP
