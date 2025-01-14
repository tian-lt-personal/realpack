#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

// std headers
#include <algorithm>
#include <cassert>
#include <climits>
#include <concepts>
#include <cstdint>
#include <limits>
#include <string_view>
#include <vector>

namespace real {

namespace details {

using z_max_digit_type = uint32_t;
using z_default_container = std::vector<z_max_digit_type>;

template <class C>
using digit_t = typename C::value_type;

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

template <std::unsigned_integral T>
constexpr int nlz(T x) {
  if (x == 0) return (int)(sizeof(T) * CHAR_BIT);
  unsigned count = 0;
  T mask = T{1u} << (sizeof(T) * CHAR_BIT - 1);
  while ((x & mask) == 0) {
    ++count;
    mask >>= 1;
  }
  return count;
}

template <class C>
constexpr digit_t<C> bit_shift(C& digits, signed offset) {
  using D = digit_t<C>;
  assert((sizeof(D) * CHAR_BIT) > std::abs(offset));
  if (offset > 0) {
    // left shift
    const D mask = ((1u << offset) - 1) << (sizeof(D) * CHAR_BIT - offset);
    D cy = 0;
    for (auto& d : digits) {
      D t = (d << offset) | cy;
      cy = (d & mask) >> (sizeof(D) * CHAR_BIT - offset);
      d = t;
    }
    return cy;
  } else if (offset < 0) {
    offset = std::abs(offset);
    // right shift
    const D mask = (1u << offset) - 1;
    D cy = 0;
    for (auto d = digits.rbegin(); d != digits.rend(); ++d) {
      D t = (*d >> offset) | cy;
      cy = (*d & mask) << (sizeof(D) * CHAR_BIT - offset);
      *d = t;
    }
    return cy;
  } else {
    return 0;
  }
}

// returns: the quotient of `(u1*b + u0) / v`, where b == 2^32
// notes:
// 1. this is a short-division with the specialization of 64-bit arithmetics, and
// 2. `u1` is the MSD, and `u0` is the LSD.
// 3. `r` is the remainder of `(u1*b + u0) / v`, where b == 2^32
inline uint64_t div_2ul_impl(uint32_t u1, uint32_t u0, uint32_t v, uint32_t* r) {
  auto u = ((u1 * uint64_t{1}) << 32 | u0);
  *r = static_cast<uint32_t>(u % v);
  return u / v;
};
inline uint32_t div_2ul_impl(uint32_t u, uint16_t v, uint16_t* r) {
  *r = static_cast<uint16_t>(u % v);
  return u / v;
};
template <std::unsigned_integral D>
auto div_2ul(D u1, D u0, D v, D* r) {
  if constexpr (sizeof(D) == sizeof(uint32_t)) {
    uint32_t _r;
    auto q = div_2ul_impl(u1, u0, v, &_r);
    *r = static_cast<D>(_r);
    return q;
  } else {
    static_assert(sizeof(D) * 2 <= sizeof(uint32_t));
    uint16_t _r;
    auto q = div_2ul_impl(u1 << (sizeof(D) * CHAR_BIT) | u0, v, &_r);
    *r = static_cast<D>(_r);
    return q;
  }
};

}  // namespace details

struct z_error : std::domain_error {
  explicit z_error(const char* reason) : std::domain_error(reason) {}
};

struct z_operation_error : z_error {
  z_operation_error(const char* reason) : z_error(reason) {}
};
struct z_divided_by_zero_error : z_operation_error {
  z_divided_by_zero_error() : z_operation_error("divided by zero") {}
};
struct z_parse_error : z_error {
  explicit z_parse_error(const char* reason) : z_error(reason) {}
};

template <class T>
concept z_digit_container = std::ranges::contiguous_range<T> && std::ranges::sized_range<T> && requires {
  typename T::value_type;
  sizeof(typename T::value_type) <= sizeof(details::z_max_digit_type);  // the max digit type is `uint32_t`
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
  return num.digits.size() == 0;
}

namespace details {

template <class C>
void check_zero(const z<C>& num) {
  if (is_zero(num)) {
    throw z_divided_by_zero_error{};
  }
}
void check_zero(const std::unsigned_integral auto& digit) {
  if (digit == 0) {
    throw z_divided_by_zero_error{};
  }
}

}  // namespace details

template <z_digit_container C = details::z_default_container>
constexpr z<C> identity() {
  return z<C>{.digits = {1u}};
};

// assumes: is_zero(num) == true && num.sign == false
template <z_digit_container C, std::integral T>
constexpr z<C>& init(z<C>& num, T val) {
  if (val == 0) return num;
  if constexpr (std::is_signed_v<T>) {
    num.sign = val < 0;
    val = num.sign ? -val : val;
  }
  using D = details::digit_t<C>;
  if constexpr (sizeof(T) <= sizeof(D)) {
    num.digits.push_back(val);
  } else {
    static_assert(sizeof(D) < sizeof(uint64_t));
    using B = std::conditional_t<sizeof(D) < sizeof(uint32_t), uint32_t, uint64_t>;
    constexpr auto base = static_cast<B>(1u) << (sizeof(D) * CHAR_BIT);
    while (val > 0) {
      num.digits.push_back(val % base);
      val /= base;
    }
  }
  return num;
}

// returns: the sign of `num`.
template <z_digit_container C>
constexpr bool sign_z(const z<C>& num) {
  return num.sign;
}

// ignores: the signs of `lhs` and `rhs`
// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <z_digit_container C>
constexpr int cmp_n(const z<C>& lhs, const z<C>& rhs) {
  if (lhs.digits.size() != rhs.digits.size()) return lhs.digits.size() < rhs.digits.size() ? -1 : 1;
  auto l = lhs.digits.rbegin();
  auto r = rhs.digits.rbegin();
  for (; l != lhs.digits.rend(); ++l, ++r) {
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
constexpr int cmp_z(const z<C>& lhs, const z<C>& rhs) {
  if (sign_z(lhs) == sign_z(rhs)) {
    auto abs_cmp = cmp_n(lhs, rhs);
    return sign_z(lhs) == false ? abs_cmp : -abs_cmp;
  } else {
    return sign_z(lhs) == false ? 1 : -1;
  }
}

// effects: num = -num.
// returns: ref to `num`.
template <z_digit_container C>
constexpr z<C>& neg_z(z<C>& num) {
  if (!num.digits.empty()) {
    num.sign = !num.sign;
  }
  return num;
}

// returns: -num.
template <z_digit_container C>
constexpr z<C> neg_z(const z<C>& num) {
  auto result = num;
  if (!num.digits.empty()) {
    result.sign = !result.sign;
  }
  return result;
}

// effects: remove redundant zeros in digits.
// returns: ref to `num`
template <z_digit_container C>
constexpr z<C>& norm_z(z<C>& num) {
  using D = details::digit_t<C>;
  auto pos = std::find_if(num.digits.rbegin(), num.digits.rend(), [](D x) { return x != 0; });
  num.digits.resize(std::size(num.digits) - std::distance(num.digits.rbegin(), pos));
  num.sign = num.digits.empty() == false ? num.sign : false;
  return num;
}

// effects: shift `num` with the `offset` digits towards msd or lsd.
// returns: ref to `num`.
template <z_digit_container C>
constexpr z<C>& shift_n(z<C>& num, size_t offset, bool lsd = false) {
  using D = details::digit_t<C>;
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
  return n < num.digits.size() ? num.digits[n] : details::digit_t<C>{0};
}

// ignores: the signs of `lhs` and `rhs`.
// returns: r = lhs + rhs.
template <z_digit_container C>
constexpr z<C> add_n(const z<C>& lhs, const z<C>& rhs) {
  using D = details::digit_t<C>;
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
  using D = details::digit_t<C>;
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
  norm_z(r);
  return r;
};

// ignores: the signs of `lhs` and `rhs`
// returns: r = lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul_n(const z<C>& lhs, const z<C>& rhs) {
  // using the long multiplication method, which is
  // the same one you learnt in grade school.
  // todo: use other fast muliplication algorithms.
  using D = details::digit_t<C>;
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
  norm_z(r);
  return r;
}

// short division
// ignores: the signs of `u` and `v`
// returns: the quotient of `u/v`, and output its remainder via `r`
template <z_digit_container C>
constexpr z<C> div_n(const z<C>& u, typename C::value_type v, typename C::value_type* r = nullptr) {
  using D = details::digit_t<C>;
  static_assert(sizeof(D) <= sizeof(details::z_max_digit_type));
  details::check_zero(v);
  D _r = 0u;
  z<C> q;
  auto& w = q.digits;
  const auto n = u.digits.size();
  w.resize(n);
  for (size_t i = 0; i < n; ++i) {
    auto j = n - i - 1;
    w[j] = static_cast<D>(details::div_2ul(_r, u.digits[j], v, &_r));
  }
  norm_z(q);
  if (r != nullptr) {
    *r = static_cast<D>(_r);
  }
  return q;
}

// long division
// ignores: the signs of `dividend` and `divisor`
// returns: the quotient of (dividend / divisor), and output its remainder
template <z_digit_container C>
constexpr z<C> div_n(z<C> u, z<C> v, z<C>* r = nullptr) {
  details::check_zero(v);
  using D = details::digit_t<C>;
  constexpr auto d_bits = sizeof(D) * CHAR_BIT;
  if (is_zero(u)) {
    if (r != nullptr) {
      *r = {};
    }
    return z<C>{};
  }
  auto c = cmp_n(u, v);
  if (c > 0) {
    if (v.digits.size() > 1) {
      const auto n = v.digits.size();
      const auto m = u.digits.size() - n;
      z<C> q;
      q.digits.resize(m + 1);

      // algorithm d (division of nonnegative integers) in TAOCP volume 2.
      // d1. [normalize]
      const auto s = details::nlz(v.digits.back());
      details::bit_shift(v.digits, s);
      u.digits.push_back(details::bit_shift(u.digits, s));
      using Q = decltype(details::div_2ul<D>(0, 0, 0, nullptr));
      static_assert((sizeof(Q) > sizeof(D)) && (alignof(Q) > alignof(D)));
      constexpr auto base = Q{1u} << d_bits;
      // d2. [initialize]
      for (size_t l = 0; l <= m; ++l)  // loop on j >= 0
      {
        auto j = m - l;
        // d3. [calculate q-hat]
        D r_hat_;
        auto q_hat = details::div_2ul(u.digits[j + n], u.digits[j + n - 1], v.digits[n - 1], &r_hat_);
        Q r_hat = r_hat_;
        for (;;) {
          if (q_hat >= base || q_hat * v.digits[n - 2] > base * r_hat + u.digits[j + n - 2]) {
            --q_hat;
            r_hat += v.digits[n - 1];
            if (r_hat < base) continue;  // loop: find q
          }
          break;
        }
        //  d4. [multiply and substract]
        using I = std::make_signed_t<Q>;
        I k = 0, t;
        for (size_t i = 0; i < n; ++i) {
          auto p = q_hat * v.digits[i];
          t = u.digits[i + j] - k - (p & (base - 1));
          u.digits[i + j] = static_cast<D>(t);
          k = (p >> d_bits) - (t >> d_bits);
        }
        t = I{u.digits[j + n]} - k;
        u.digits[j + n] = static_cast<D>(t);
        q.digits[j] = static_cast<D>(q_hat);

        // d5 & d6. [test remainder & add back]
        if (t < 0) {
          --q.digits[j];
          k = 0;
          for (size_t i = 0; i < n; ++i) {
            t = static_cast<Q>(u.digits[i + j]) + v.digits[i] + k;
            u.digits[i + j] = static_cast<D>(t);
            k = t >> d_bits;
          }
          u.digits[j + n] = static_cast<D>(u.digits[j + n] + k);
        }
      }  // d7. [loop on j]
      // d8. [unnormalize]
      if (r != nullptr) {
        *r = {};
        r->digits.resize(n);
        for (size_t i = 0; i < n; ++i)
          r->digits[i] = (u.digits[i] >> s) | (static_cast<Q>(u.digits[i + 1]) << (d_bits - s));
        r->digits[n - 1] = u.digits[n - 1] >> s;
        norm_z(*r);
      }
      norm_z(q);
      return q;
    } else {
      assert(v.digits.size() == 1);
      D r_;
      auto q = div_n(u, v.digits.front(), &r_);
      if (r != nullptr) {
        *r = {};
        init(*r, r_);
      }
      return q;
    }
  } else if (c < 0) {
    if (r != nullptr) {
      *r = u;
    }
    return z<C>{};
  } else {
    if (r != nullptr) {
      *r = {};
    }
    return identity<C>();
  }
}

// returns: num ^ exp
template <z_digit_container C>
constexpr z<C> pow_n(const z<C>& num, size_t exp) {
  z<C> res = identity<C>();
  for (size_t i = 0; i < exp; ++i) {
    res = mul_n(res, num);
  }
  return res;
}
template <z_digit_container C>
constexpr z<C> pow_z(const z<C>& num, size_t exp) {
  z<C> res = identity<C>();
  for (size_t i = 0; i < exp; ++i) {
    res = mul_z(res, num);
  }
  return res;
}

// returns: abs(num)
template <z_digit_container C>
constexpr z<C> abs_z(const z<C>& num) {
  return z<C>{.digits = num.digits};
}

// returns: lhs + rhs;
// notes: operands may be negative integers
template <z_digit_container C>
constexpr z<C> add_z(const z<C>& lhs, const z<C>& rhs) {
  z<C> r;
  if (sign_z(lhs) == sign_z(rhs)) {
    auto tmp_sign = sign_z(lhs);
    r = add_n<C>(lhs, rhs);
    r.sign = tmp_sign;
  } else {
    const auto* minuend = &lhs;
    const auto* substrahend = &rhs;
    if (cmp_n(lhs, rhs) < 0) {
      minuend = &rhs;
      substrahend = &lhs;
    }
    r = sub_n<C>(*minuend, *substrahend);
    r.sign = r.digits.empty() == false ? sign_z(*minuend) : false;
  }
  return r;
}

// returns: lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul_z(const z<C>& lhs, const z<C>& rhs) {
  z<C> r;
  r = mul_n<C>(lhs, rhs);
  r.sign = lhs.sign != rhs.sign;
  return r;
}

// requires: exp >= 0.
// effects: num = num * 2 ^ exp.
template <z_digit_container C>
constexpr void mul_2exp_z(z<C>& num, size_t exp) {
  using D = details::digit_t<C>;
  if (exp < sizeof(D) * CHAR_BIT) {
    auto cy = details::bit_shift(num.digits, static_cast<signed>(exp));
    if (cy > 0) {
      num.digits.push_back(cy);
    }
  } else {
    throw std::logic_error{"not implemented."};
  }
}

// requires: exp >= 0.
// returns: num * 2 ^ exp.
template <z_digit_container C>
constexpr z<C> mul_2exp_z(const z<C>& num, size_t exp) {
  auto res = num;
  mul_2exp_z(res, exp);
  return res;
}

// returns: floor(lhs / rhs), and outputs its remainder
template <z_digit_container C>
constexpr z<C> div_z(z<C> lhs, z<C> rhs, z<C>* remainder = nullptr) {
  auto lsign = lhs.sign, rsign = rhs.sign, qsign = lsign != rsign;
  auto q = qsign == false ? div_n(std::move(lhs), std::move(rhs), remainder) : div_n(std::move(lhs), rhs, remainder);
  if (remainder != nullptr) {
    if (qsign) *remainder = sub_n(rhs, *remainder);
    remainder->sign = rsign;
  }
  q.sign = qsign;
  return q;
}

// requires: exp >= 0
// effects: num = num / (2^exp), rounded to nearest
template <z_digit_container C>
constexpr void ndiv_2exp_z(z<C>& num, size_t exp) {
  using D = details::digit_t<C>;
  if (exp < sizeof(D) * CHAR_BIT) {
    auto rnd = num.digits.empty() ? false : ((1u << (exp - 1)) & num.digits.front()) > 0;
    details::bit_shift(num.digits, -static_cast<signed>(exp));
    if (rnd) {
      auto one = identity<C>();
      one.sign = num.sign;
      num = add_z(num, one);
    }
  } else {
    throw std::logic_error{"not implemented."};
  }
}

// requires: exp >= 0
// returns: num / (2^exp), rounded to nearest
template <z_digit_container C>
constexpr z<C> ndiv_2exp_z(const z<C>& num, size_t exp) {
  if (exp != 0) {
    auto res = num;
    ndiv_2exp_z(res, exp);
    return res;
  } else {
    return num;
  }
}

}  // namespace real

#endif  // !REALPACK_INC_Z_HPP
