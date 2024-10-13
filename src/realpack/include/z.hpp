#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

// std headers
#include <algorithm>
#include <cassert>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <stack>
#include <string_view>
#include <vector>

namespace real {

namespace details {

using z_max_digit_type = unsigned long;
using z_default_container = std::vector<z_max_digit_type>;

constexpr bool is_ws(char ch) { return ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f'; }
constexpr bool is_decimal(char ch) { return '0' <= ch && ch <= '9'; }
constexpr std::optional<unsigned> char2decimal(char ch) {
  if (is_decimal(ch))
    return static_cast<unsigned>(ch - '0');
  else
    return std::nullopt;
}

template <class MaxD, std::unsigned_integral T>
constexpr T umul(T lhs, T rhs, T& o) {
  if constexpr (sizeof(T) >= sizeof(MaxD)) {
    constexpr T s = sizeof(T) * 4u;
    constexpr T mask = std::numeric_limits<T>::max() >> s;
    T a0 = lhs & mask, b0 = rhs & mask;
    T a1 = lhs >> s, b1 = rhs >> s;
    T ll = a0 * b0, lh = a0 * b1, hl = a1 * b0, hh = a1 * b1;
    T w = lh + (ll >> s);
    w += hl;
    if (w < hl) hh += mask + 1u;
    o = hh + (w >> s);
    return (w << s) + (ll & mask);
  } else {
    constexpr unsigned s = sizeof(T) * 8u;
    MaxD l = lhs, r = rhs;
    auto prod = l * r;
    o = static_cast<T>(prod >> s);
    return static_cast<T>(prod);
  }
}

}  // namespace details

struct z_error : std::domain_error {
  z_error(const char* reason) : std::domain_error(reason) {}
};

struct z_operation_error : z_error {
  z_operation_error(const char* reason) : z_error(reason) {}
};
struct z_divided_by_zero_error : z_operation_error {
  z_divided_by_zero_error() : z_operation_error("divided by zero") {}
};
struct z_parse_error : z_error {
  z_parse_error(const char* reason) : z_error(reason) {}
};

#if defined(_DEBUG) || defined(DEBUG)
#define _REAL_CHECK_ZERO(Z) \
  if (is_zero(Z)) throw z_divided_by_zero_error{};
#else
#define _REAL_CHECK_ZERO(Z)
#endif

template <class T>
concept z_digit_container = std::ranges::contiguous_range<T> && std::ranges::sized_range<T> && requires {
  typename T::value_type;
  sizeof(typename T::value_type) <= sizeof(details::z_max_digit_type);  // the max digit type is `unsigned long`
  std::is_unsigned_v<typename T::value_type>;
};

template <z_digit_container C = details::z_default_container>
struct z {
  using container_type = C;
  using digit_type = typename C::value_type;

  C digits;           // least significant digits (LSD) first
  bool sign = false;  // false for non-negative. -0 is ill-formed.
};

template <z_digit_container C>
constexpr bool is_zero(const z<C>& num) noexcept {
  if (num.digits.size() == 0) return true;
  return std::ranges::all_of(num.digits, [](typename z<C>::digit_type x) { return x == 0; });
}

// assumes: is_zero(num) == true && num.sign == false
template <z_digit_container C, std::integral T>
constexpr z<C>& init(z<C>& num, T val) {
  if (val == 0) return num;
  if constexpr (std::is_signed_v<T>) {
    num.sign = val < 0;
    val = num.sign ? -val : val;
  }
  constexpr auto digit_size = sizeof(typename z<C>::digit_type);
  if constexpr (sizeof(T) <= digit_size) {
    num.digits.push_back(val);
  } else {
    static_assert(digit_size < 4);
    constexpr unsigned long base = 1u << (digit_size * 8);
    while (val > 0) {
      num.digits.push_back(val % base);
      val /= base;
    }
  }
  return num;
}

// returns: the sign of `num`.
template <z_digit_container C>
constexpr bool sign(const z<C>& num) {
  return num.sign;
}

// ignores: the signs of `lhs` and `rhs`
// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <z_digit_container C>
constexpr int cmp_n(const z<C>& lhs, const z<C>& rhs) {
  if (lhs.digits.size() != rhs.digits.size()) return lhs.digits.size() < rhs.digits.size() ? -1 : 1;
  auto l = lhs.digits.crbegin();
  auto r = rhs.digits.crbegin();
  for (; l != lhs.digits.crend(); ++l, ++r) {
    if (*l != *r) {
      return *l < *r ? -1 : 1;
    }
  }
  return 0;
}

// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <z_digit_container C>
constexpr int cmp(const z<C>& lhs, const z<C>& rhs) {
  if (sign(lhs) == sign(rhs)) {
    auto abs_cmp = cmp_n(lhs, rhs);
    return sign(lhs) == false ? abs_cmp : -abs_cmp;
  } else {
    return sign(lhs) == false ? 1 : -1;
  }
}

// effects: num = -num.
// returns: ref to `num`.
template <z_digit_container C>
constexpr z<C>& neg(z<C>& num) {
  num.sign = !num.sign;
  return num;
}

// effects: remove redundant zeros in digits.
// returns: ref to `num`
template <z_digit_container C>
constexpr z<C>& norm_n(z<C>& num) {
  using D = typename z<C>::digit_type;
  auto view = num.digits | std::views::reverse | std::views::drop_while([](D x) { return x == 0; });
  num.digits.resize(std::ranges::distance(view));
  num.sign = num.digits.empty() == false ? num.sign : false;
  return num;
}

// effects: shift `num` with the `offset` digits towards msd or lsd.
// returns: ref to `num`.
template <z_digit_container C>
constexpr z<C>& shift_n(z<C>& num, size_t offset, bool lsd = false) {
  using D = typename z<C>::digit_type;
  // constexpr bool has_pop_front = ;
  if (lsd) {
    if (offset < num.digits.size()) {
      num.digits.erase(num.digits.begin(), num.digits.begin() + offset);
    } else {
      num.digits.clear();
    }
  } else {
    // msd
    num.digits.insert(num.digits.begin(), offset, (D)0);
  }
  return num;
}

// returns: the n-th digit of `num`.
// notes: `n` is an index that starts counting from 0.
template <z_digit_container C>
constexpr auto digit_n(const z<C>& num, size_t n) {
  using D = typename z<C>::digit_type;
  return n < num.digits.size() ? num.digits[n] : (D)0;
}

// ignores: the signs of `lhs` and `rhs`.
// returns: r = lhs + rhs.
template <z_digit_container C>
constexpr z<C> add_n(const z<C>& lhs, const z<C>& rhs) {
  using D = typename z<C>::digit_type;
  z<C> r;
  // ensure size(a.digits) <= size(b.digits).
  auto& a = lhs.digits.size() <= rhs.digits.size() ? lhs : rhs;
  auto& b = lhs.digits.size() <= rhs.digits.size() ? rhs : lhs;
  size_t i = 0;
  D cy = 0;
  for (; i < a.digits.size(); ++i) {
    D t = a.digits[i] + b.digits[i] + cy;
    cy = t >= a.digits[i] && t >= b.digits[i] ? 0u : 1u;
    r.digits.push_back(t);
  }
  for (; i < b.digits.size(); ++i) {
    D t = b.digits[i] + cy;
    cy = t >= b.digits[i] && t >= cy ? 0u : 1u;
    r.digits.push_back(t);
  }
  if (cy > 0) {
    r.digits.push_back(1);
  }
  return r;
}

// requires: lhs >= rhs
// ignores: the signs of `lhs` and `rhs`
// returns: r = lhs - rhs;
template <z_digit_container C>
constexpr z<C> sub_n(const z<C>& lhs, const z<C>& rhs) {
  using D = typename z<C>::digit_type;
  z<C> r;
  auto& a = lhs;
  auto& b = rhs;
  size_t i = 0;
  D cy = 0;
  for (; i < b.digits.size(); ++i) {
    D t = a.digits[i] - b.digits[i] - cy;
    cy = t > a.digits[i];
    r.digits.push_back(t);
  }
  for (; i < a.digits.size(); ++i) {
    D t = a.digits[i] - cy;
    cy = t > a.digits[i];
    r.digits.push_back(t);
  }
  assert(cy == 0);
  norm_n(r);
  return r;
};

// ignores: the signs of `lhs` and `rhs`
// returns: r = lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul_n(const z<C>& lhs, const z<C>& rhs) {
  // using the long multiplication method, which is
  // the same one you learnt in grade school.
  // todo: use other fast muliplication algorithms.
  using D = typename z<C>::digit_type;
  z<C> r;
  r.digits.resize(lhs.digits.size() + rhs.digits.size());
  for (size_t j = 0; j < rhs.digits.size(); ++j) {
    D cy = 0;
    for (size_t i = 0; i < lhs.digits.size(); ++i) {
      D o;
      auto prod = details::umul<details::z_max_digit_type>(lhs.digits[i], rhs.digits[j], o);
      prod += cy;
      cy = (prod < cy ? 1u : 0u) + o;
      r.digits[i + j] += prod;
      if (r.digits[i + j] < prod) ++cy;
    }
    r.digits[j + lhs.digits.size()] = cy;
  }
  norm_n(r);
  return r;
}

// ignores: the signs of `dividend` and `divisor`
// returns: the quotient of (dividend / divisor), and out put its remainder
template <z_digit_container C>
constexpr z<C> div_n(const z<C>& dividend, const z<C>& divisor, z<C>& remainder) {
  _REAL_CHECK_ZERO(divisor);
  // long division.
  // todo: use some views over z to avoid intermediate copies.
  z<C> q;  // quotient
  z<C>& r = remainder;
  const auto k = dividend.digits.size();
  const auto l = divisor.digits.size();

  return q;
}

// returns: r = lhs + rhs;
// notes: operands could be negative integers
template <z_digit_container C>
constexpr z<C> add(const z<C>& lhs, const z<C>& rhs) {
  z<C> r;
  if (sign(lhs) == sign(rhs)) {
    auto tmp_sign = sign(lhs);
    r = add_n(lhs, rhs);
    r.sign = tmp_sign;
  } else {
    const z<C>* minuend = &lhs;
    const z<C>* substrahend = &rhs;
    if (cmp_n(lhs, rhs) < 0) {
      minuend = &rhs;
      substrahend = &lhs;
    }
    r = sub_n(*minuend, *substrahend);
    r.sign = r.digits.empty() == false ? sign(*minuend) : false;
  }
  return r;
}

// returns: r = lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul(const z<C>& lhs, const z<C>& rhs) {
  z<C> r;
  r = mul_n(lhs, rhs);
  r.sign = lhs.sign != rhs.sign;
  return r;
}

template <z_digit_container C>
constexpr z<C>& init_decstr(z<C>& num, std::string_view str) {
  // illustration:
  // "    -       2024"
  //  sign | gap |num
  bool all0 = true;
  enum class parse_state { sign, gap, num, done };
  auto ps = parse_state::sign;
  std::vector<unsigned> digits;
  for (size_t i = 0; i < str.length() && ps != parse_state::done; ++i) {
    auto ch = str[i];
    switch (ps) {
      case parse_state::sign:
        if (details::is_ws(ch)) continue;
        switch (ch) {
          case '+':
            ps = parse_state::gap;
            break;
          case '-':
            num.sign = true;
            ps = parse_state::gap;
            break;
          default:
            if (details::is_decimal(ch)) {
              ps = parse_state::num;
              if (auto v = details::char2decimal(ch); v.has_value()) {
                all0 &= *v == 0;
                digits.push_back(*v);
              } else
                throw z_parse_error{"bad number symbol."};
            } else
              throw z_parse_error{"bad sign symbol."};
        }
        break;
      case parse_state::gap:
        if (details::is_ws(ch))
          continue;
        else {
          ps = parse_state::num;
        }
        [[fallthrough]];
      case parse_state::num: {
        if (details::is_ws(ch)) {
          ps = parse_state::done;
          break;
        }
        if (auto v = details::char2decimal(ch); v.has_value()) {
          all0 &= *v == 0;
          digits.push_back(*v);
        } else
          throw z_parse_error{"bad number symbol."};
        break;
      }
      default:
        throw std::logic_error{"unhandled state."};
    }
  }
  if (!all0) {
    // TODO: convert the base 10 number to other bases.
  }
  return num;
}

}  // namespace real

#undef _REAL_CHECK_ZERO
#endif  // !REALPACK_INC_Z_HPP
