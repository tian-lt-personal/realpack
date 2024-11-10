#ifndef REALPACK_INC_Z_HPP
#define REALPACK_INC_Z_HPP

// std headers
#include <algorithm>
#include <cassert>
#include <climits>
#include <concepts>
#include <limits>
#include <optional>
#include <ranges>
#include <span>
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

template <std::unsigned_integral T>
constexpr T nlz(T x) {
  if (x == 0) return sizeof(T) * CHAR_BIT;
  unsigned count = 0;
  T mask = 1u << (sizeof(T) * CHAR_BIT - 1);
  while ((x & mask) == 0) {
    ++count;
    mask >>= 1;
  }
  return count;
}

template <std::unsigned_integral D>
constexpr D bit_shift(std::span<D> digits, signed offset) {
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
    for (auto& d : digits | std::views::reverse) {
      D t = (d >> offset) | cy;
      cy = (d & mask) << (sizeof(D) * CHAR_BIT - offset);
      d = t;
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
inline unsigned long long div_2ul_impl(unsigned long u1, unsigned long u0, unsigned long v, unsigned long& r) {
  auto u = ((u1 * 1ull) << 32 | u0);
  r = static_cast<unsigned long>(u % v);
  return u / v;
};
inline unsigned long div_2ul_impl(unsigned long u, unsigned short v, unsigned short& r) {
  r = static_cast<unsigned short>(u % v);
  return u / v;
};
template <std::unsigned_integral D>
auto div_2ul(D u1, D u0, D v, D& r) {
  if constexpr (sizeof(D) == sizeof(unsigned long)) {
    unsigned long _r;
    auto q = div_2ul_impl(u1, u0, v, _r);
    r = static_cast<D>(_r);
    return q;
  } else {
    static_assert(sizeof(D) * 2 <= sizeof(unsigned long));
    unsigned short _r;
    auto q = div_2ul_impl(u1 << (sizeof(D) * CHAR_BIT) | u0, v, _r);
    r = static_cast<D>(_r);
    return q;
  }
};

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

#if defined(_DEBUG) || defined(DEBUG) || defined(REAL_ENABLE_ZERO_CHECK)
#define _REAL_CHECK_ZERO(Z) \
  if (is_zero(Z)) throw z_divided_by_zero_error{};
#define _REAL_CHECK_ZERO_D(D) \
  if ((D) == 0) throw z_divided_by_zero_error{};
#else
#define _REAL_CHECK_ZERO(Z)
#define _REAL_CHECK_ZERO_D(D)
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

template <class D>
struct z_view {
  using digit_type = D;

  std::span<const D> digits;
  bool sign;

  template <class Z>
  constexpr z_view(Z&& z) : digits(z.digits), sign(z.sign) {}
  explicit constexpr z_view(std::span<const D> d, bool s) : digits(d), sign(s) {}
};
template <z_digit_container C>
z_view(z<C>) -> z_view<typename C::value_type>;

template <z_digit_container C>
constexpr bool is_zero(const z<C>& num) noexcept {
  if (num.digits.size() == 0) return true;
  return std::ranges::all_of(num.digits, [](typename z<C>::digit_type x) { return x == 0; });
}

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
  using D = typename z<C>::digit_type;
  if constexpr (sizeof(T) <= sizeof(D)) {
    num.digits.push_back(val);
  } else {
    static_assert(sizeof(D) < sizeof(unsigned long long));
    using B = std::conditional_t<sizeof(D) < sizeof(unsigned long), unsigned long, unsigned long long>;
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
template <class D>
constexpr bool sign_z(const z_view<D>& num) {
  return num.sign;
}

// ignores: the signs of `lhs` and `rhs`
// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <class D>
constexpr int cmp_n(z_view<D> lhs, z_view<D> rhs) {
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
template <z_digit_container C>
constexpr int cmp_n(const z<C>& lhs, const z<C>& rhs) {
  return cmp_n(z_view{lhs}, z_view{rhs});
}

// returns: 0 if lhs is exactly equal to rhs
//          + if lhs is greater than rhs
//          - if lhs is less than rhs
template <class D>
constexpr int cmp_z(z_view<D> lhs, z_view<D> rhs) {
  if (sign_z(lhs) == sign_z(rhs)) {
    auto abs_cmp = cmp_n(lhs, rhs);
    return sign_z(lhs) == false ? abs_cmp : -abs_cmp;
  } else {
    return sign_z(lhs) == false ? 1 : -1;
  }
}
template <z_digit_container C>
constexpr int cmp_z(const z<C>& lhs, const z<C>& rhs) {
  return cmp_z(z_view{lhs}, z_view{rhs});
}

// effects: num = -num.
// returns: ref to `num`.
template <z_digit_container C>
constexpr z<C>& neg_z(z<C>& num) {
  num.sign = !num.sign;
  return num;
}

// returns: -num.
template <z_digit_container C>
constexpr z<C> neg_z(const z<C>& num) {
  auto result = num;
  result.sign = !result.sign;
  return result;
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
template <class D>
constexpr auto digit_n(z_view<D> num, size_t n) {
  return n < num.digits.size() ? num.digits[n] : (D)0;
}
template <z_digit_container C>
constexpr auto digit_n(const z<C>& num, size_t n) {
  return digit_n(z_view{num}, n);
}

// ignores: the signs of `lhs` and `rhs`.
// returns: r = lhs + rhs.
template <z_digit_container C>
constexpr z<C> add_n(z_view<typename C::value_type> lhs, z_view<typename C::value_type> rhs) {
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
template <z_digit_container C>
constexpr z<C> add_n(const z<C>& lhs, const z<C>& rhs) {
  return add_n<C>(z_view{lhs}, z_view{rhs});
}

// requires: lhs >= rhs
// ignores: the signs of `lhs` and `rhs`
// returns: r = lhs - rhs;
template <z_digit_container C>
constexpr z<C> sub_n(z_view<typename C::value_type> lhs, z_view<typename C::value_type> rhs) {
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
template <z_digit_container C>
constexpr z<C> sub_n(const z<C>& lhs, const z<C>& rhs) {
  return sub_n<C>(z_view{lhs}, z_view{rhs});
};

// ignores: the signs of `lhs` and `rhs`
// returns: r = lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul_n(z_view<typename C::value_type> lhs, z_view<typename C::value_type> rhs) {
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
template <z_digit_container C>
constexpr z<C> mul_n(const z<C>& lhs, const z<C>& rhs) {
  return mul_n<C>(z_view{lhs}, z_view{rhs});
}

// short division
// ignores: the signs of `u` and `v`
// returns: the quotient of `u/v`, and output its remainder via `r`
template <z_digit_container C>
constexpr z<C> div_n(z_view<typename C::value_type> u, typename C::value_type v, typename C::value_type* r = nullptr) {
  using D = typename C::value_type;
  static_assert(sizeof(D) <= sizeof(details::z_max_digit_type));
  _REAL_CHECK_ZERO_D(v);
  D _r = 0u;
  z<C> q;
  auto& w = q.digits;
  const auto n = u.digits.size();
  w.resize(n);
  for (size_t i = 0; i < n; ++i) {
    auto j = n - i - 1;
    w[j] = static_cast<D>(details::div_2ul(_r, u.digits[j], v, _r));
  }
  norm_n(q);
  if (r != nullptr) {
    *r = static_cast<D>(_r);
  }
  return q;
}
template <z_digit_container C>
constexpr z<C> div_n(z<C> u, typename C::value_type v, typename C::value_type* r = nullptr) {
  using D = typename C::value_type;
  return div_n<C>(z_view<D>{u}, v, r);
}

// long division
// ignores: the signs of `dividend` and `divisor`
// returns: the quotient of (dividend / divisor), and output its remainder
template <z_digit_container C>
constexpr z<C> div_n(z<C> u, z<C> v, z<C>* r = nullptr) {
  _REAL_CHECK_ZERO(v);
  using D = typename C::value_type;
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
      details::bit_shift<D>(v.digits, s);
      u.digits.push_back(details::bit_shift<D>(u.digits, s));
      using Q = decltype(details::div_2ul(std::declval<D>(), std::declval<D>(), std::declval<D>(), *(D*)nullptr));
      // d2. [initialize]
      for (size_t l = 0; l <= m; ++l)  // loop on j >= 0
      {
        auto j = m - l;
        // d3. [calculate q-hat]
        D r_hat_;
        auto q_hat = details::div_2ul(u.digits[j + n], u.digits[j + n - 1], v.digits[n - 1], r_hat_);
        const auto base = static_cast<Q>(1u) << (sizeof(D) * CHAR_BIT);
        Q r_hat = r_hat_;
      LOOP_D3_FIND_Q:
        if (q_hat == base || q_hat * v.digits[n - 2] > base * r_hat + u.digits[j + n - 2]) {
          --q_hat;
          r_hat += v.digits[n - 1];
          if (r_hat < base) goto LOOP_D3_FIND_Q;
        }
        // d4. [multiply and substract]
        using I = std::make_signed_t<Q>;
        I k = 0, t;
        for (size_t i = 0; i < n; ++i) {
          auto p = q_hat * v.digits[i];
          t = u.digits[i + j] - k - (p & (base - 1));
          u.digits[i + j] = static_cast<D>(t);
          k = (p >> (sizeof(D) * CHAR_BIT)) - (t >> (sizeof(D) * CHAR_BIT));
        }
        t = u.digits[j + n] - k;
        u.digits[j + n] = static_cast<D>(t);
        q.digits[j] = static_cast<D>(q_hat);

        // d5 & d6. [test remainder & add back]
        if (t < 0) {
          --q.digits[j];
          k = 0;
          for (size_t i = 0; i < n; ++i) {
            t = static_cast<Q>(u.digits[i + j]) + v.digits[i] + k;
            u.digits[i + j] = static_cast<D>(t);
            k = t >> (sizeof(D) * CHAR_BIT);
          }
          u.digits[j + n] = static_cast<D>(u.digits[j + n] + k);
        }
      }  // d7. [loop on j]
      // d8. [unnormalize]
      if (r != nullptr) {
        *r = {};
        r->digits.resize(n);
        for (size_t i = 0; i < n; ++i)
          r->digits[i] = (u.digits[i] >> s) | (static_cast<Q>(u.digits[i + 1]) << ((sizeof(D) * CHAR_BIT) - s));
        r->digits[n - 1] = u.digits[n - 1] >> s;
        norm_n(*r);
      }
      norm_n(q);
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

//
template <z_digit_container C>
constexpr z<C> abs_z(z_view<typename C::value_type> num) {
  return z<C>{.digits = num.digits};
}
template <z_digit_container C>
constexpr z<C> abs_z(const z<C>& num) {
  return abs<C>(z_view{num});
}

// returns: lhs + rhs;
// notes: operands may be negative integers
template <z_digit_container C>
constexpr z<C> add_z(z_view<typename C::value_type> lhs, z_view<typename C::value_type> rhs) {
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
template <z_digit_container C>
constexpr z<C> add_z(const z<C>& lhs, const z<C>& rhs) {
  return add_z<C>(z_view{lhs}, z_view{rhs});
}

// returns: lhs * rhs;
template <z_digit_container C>
constexpr z<C> mul_z(z_view<typename C::value_type> lhs, z_view<typename C::value_type> rhs) {
  z<C> r;
  r = mul_n<C>(lhs, rhs);
  r.sign = lhs.sign != rhs.sign;
  return r;
}
template <z_digit_container C>
constexpr z<C> mul_z(const z<C>& lhs, const z<C>& rhs) {
  return mul_z<C>(z_view{lhs}, z_view{rhs});
}

// requires: exp >= 0.
// effects: num = num * 2 ^ exp.
template <z_digit_container C>
constexpr void mul_2exp_z(z<C>& num, size_t exp) {
  using D = typename z<C>::digit_type;
  if (exp < sizeof(D) * CHAR_BIT) {
    auto cy = details::bit_shift<D>(num.digits, static_cast<signed>(exp));
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
  using D = typename z<C>::digit_type;
  if (exp < sizeof(D) * CHAR_BIT) {
    auto rnd = num.digits.empty() ? false : ((1u << (exp - 1)) & num.digits.front()) > 0;
    details::bit_shift<D>(num.digits, -static_cast<signed>(exp));
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
#undef _REAL_CHECK_ZERO_D
#endif  // !REALPACK_INC_Z_HPP
