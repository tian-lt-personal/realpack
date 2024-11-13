#ifndef REALPACK_INC_TO_HPP
#define REALPACK_INC_TO_HPP

// std headers
#include <optional>

// real headers
#include "r.hpp"
#include "z.hpp"

// util headers
#include "_create.hpp"

namespace real {

template <z_digit_container C>
std::optional<int> to_int(const z<C>&) {
  return 0;  // TODO
}

template <z_digit_container C>
coro::lazy<std::optional<int>> to_int(evaluator<C> num, size_t n) {
  auto q = co_await num(n);
  ndiv_2exp_z<C>(q, n);
  co_return to_int(q);
}

}  // namespace real

#endif  // !REALPACK_INC_TO_HPP
