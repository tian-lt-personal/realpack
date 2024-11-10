#ifndef REALPACK_INC_R_HPP
#define REALPACK_INC_R_HPP

// std headers
#include <stdexcept>

// real headers
#include "z.hpp"

// util headers
#include "_coro.hpp"

namespace real {

namespace details {}  // namespace details

template <z_digit_container C>
using evaluator = std::function<coro::lazy<z<C>>(size_t)>;

template <z_digit_container C>
evaluator<C> expr_q(z<C> n, z<C> d) {
  return [n = std::move(n), d = std::move(d)](size_t n) -> coro::lazy<z<C>> {
    co_return z<C>{};  // TODO: qn = floor(q * B^n)
  };
}

template <z_digit_container C>
evaluator<C> expr_add(evaluator<C> lhs, evaluator<C> rhs) {
  return [lhs = std::move(lhs), rhs = std::move(rhs)](size_t n) -> coro::lazy<z<C>> {
    auto x = co_await lhs(n + 2);
    auto y = co_await rhs(n + 2);
    co_return z<C>{};  // TODO:
  };
}

}  // namespace real

#endif  // !REALPACK_INC_R_HPP
