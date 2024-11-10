#ifndef REALPACK_INC_R_HPP
#define REALPACK_INC_R_HPP

// std headers
#include <optional>
#include <stdexcept>

// real headers
#include "z.hpp"

// util headers
#include "_coro.hpp"
#include "_op.hpp"

namespace real {

namespace details {}  // namespace details

template <z_digit_container C>
using evaluator = std::function<coro::lazy<z<C>>(size_t)>;

template <z_digit_container C>
evaluator<C> expr_q(z<C> p, std::optional<z<C>> q = std::nullopt) {
  return [p = std::move(p), q = std::move(q)](size_t n) -> coro::lazy<z<C>> {
    auto res = mul_2exp_z(p, n);
    if (q.has_value()) {
      co_return div_z(res, *q);
    }
    co_return res;
  };
}

template <z_digit_container C>
evaluator<C> expr_add(evaluator<C> lhs, evaluator<C> rhs) {
  return [lhs = std::move(lhs), rhs = std::move(rhs)](size_t n) -> coro::lazy<z<C>> {
    auto x = co_await lhs(n + 2);
    auto y = co_await rhs(n + 2);
    co_return ndiv_2exp_z(x + y, 2);
  };
}

}  // namespace real

#endif  // !REALPACK_INC_R_HPP
