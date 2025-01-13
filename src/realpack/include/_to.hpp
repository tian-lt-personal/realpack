#ifndef REALPACK_INC_TO_HPP
#define REALPACK_INC_TO_HPP

// std headers
#include <algorithm>
#include <concepts>
#include <optional>

// real headers
#include "r.hpp"
#include "s.hpp"
#include "z.hpp"

// util headers
#include "_create.hpp"

namespace real {

namespace details {

template <class D>
void append_value(std::string& out, D value, bool fullfill) {
  constexpr auto rank = details::decimal_parsing_rank<D>();
  for (unsigned int i = 0; i < rank; ++i) {
    if (!fullfill && value == 0) {
      break;
    }
    out.push_back((value % 10) + '0');
    value /= 10;
  }
}

}  // namespace details

template <z_digit_container C>
std::string to_decimal_string(z<C> num) {
  if (is_zero(num)) {
    return "0";
  }
  std::string res;
  bool sign = num.sign;
  using D = details::digit_t<C>;
  constexpr auto rank = details::decimal_parsing_rank<D>();
  const auto v10 = details::pow10<D>(rank);
  while (!is_zero(num)) {
    D r;
    num = div_n(num, v10, &r);
    details::append_value(res, r, !is_zero(num));
  }
  if (sign) {
    res.push_back('-');
  }
  std::reverse(res.begin(), res.end());
  return res;
}

template <z_digit_container C>
std::string to_decimal_string(fx<C> frac) {
  assert(frac.coeff.digits.size() >= frac.nexp);
  z<C> integers;
  integers.sign = frac.coeff.sign;
  std::copy(frac.coeff.digits.begin() + frac.nexp, frac.coeff.digits.end(), std::back_inserter(integers.digits));
  frac.coeff.digits.resize(frac.nexp);
  auto res = to_decimal_string(std::move(integers));
  res.push_back('.');
  frac.coeff = mul_n(frac.coeff, pow_n(create_z<C>(10), frac.nexp));
  shift_n(frac.coeff, frac.nexp, true);
  auto trailing = to_decimal_string(std::move(frac.coeff));
  res.insert(res.end(), trailing.begin(), trailing.end());
  return res;
}

template <std::integral N, z_digit_container C>
constexpr N to_int(const z<C>&) {
  if constexpr (std::is_unsigned_v<N>) {
    constexpr auto max = create_z<C>(std::numeric_limits<N>::max());
    return 0;  // TODO:
  } else {
    return 0;  // TODO:
  }
}

template <z_digit_container C>
constexpr std::optional<double> to_double(const z<C>&, const z<C>&) {
  return 0;  // TODO
}

template <z_digit_container C>
coro::lazy<std::optional<int>> to_int(evaluator<C> num, size_t n) {
  auto q = co_await num(n);
  ndiv_2exp_z<C>(q, n);
  co_return to_int<int>(q);
}

template <z_digit_container C>
coro::lazy<std::optional<double>> to_double(evaluator<C> num, size_t n) {
  auto q = co_await num(n);
  co_return to_double(q, mul_2exp_z(real::identity<C>(), n));
}

}  // namespace real

#endif  // !REALPACK_INC_TO_HPP
