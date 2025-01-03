#ifndef REALPACK_INC_CORO_HPP
#define REALPACK_INC_CORO_HPP

#include <coroutine>
#include <optional>
#include <semaphore>
#include <stdexcept>

namespace real::coro {

template <class T>
class lazy_promise;

template <class T>
class lazy {
  friend class lazy_promise<T>;

 public:
  lazy(const lazy&) = delete;
  lazy(lazy&& rhs) noexcept : coro_(std::exchange(rhs.coro_, {})) {}
  ~lazy() {
    if (static_cast<bool>(coro_)) coro_.destroy();
  }

  class awaiter;
  awaiter operator co_await() noexcept;

 private:
  explicit lazy(std::coroutine_handle<lazy_promise<T>> coro) : coro_(coro) {}
  std::coroutine_handle<lazy_promise<T>> coro_;
};

template <class T>
class lazy_promise {
  friend class lazy<T>::awaiter;

 public:
  lazy<T> get_return_object() noexcept { return lazy<T>{std::coroutine_handle<lazy_promise>::from_promise(*this)}; }
  constexpr auto initial_suspend() noexcept { return std::suspend_always{}; }
  constexpr auto final_suspend() noexcept {
    struct final_awaiter : std::suspend_always {
      auto await_suspend(std::coroutine_handle<lazy_promise> handle) noexcept { return handle.promise().cont_; }
    };
    return final_awaiter{};
  }
  template <class U>
  void return_value(U&& value) {
    val_ = std::forward<U>(value);
  }
  void unhandled_exception() { std::terminate(); }

 private:
  std::coroutine_handle<> cont_;
  std::optional<T> val_;
};

template <class T>
class lazy<T>::awaiter : public std::suspend_always {
 public:
  explicit awaiter(std::coroutine_handle<lazy_promise<T>> handle) : coro_(handle) {}
  auto await_suspend(std::coroutine_handle<> cont) noexcept {
    coro_.promise().cont_ = cont;
    return coro_;
  }
  T await_resume() noexcept(std::is_nothrow_move_constructible_v<T>) { return std::move(*coro_.promise().val_); }

 private:
  std::coroutine_handle<lazy_promise<T>> coro_;
};
template <class T>
typename lazy<T>::awaiter lazy<T>::operator co_await() noexcept {
  return lazy<T>::awaiter{coro_};
}

struct forget {
  struct promise_type {
    constexpr forget get_return_object() noexcept { return forget{}; }
    constexpr auto initial_suspend() noexcept { return std::suspend_never{}; }
    constexpr auto final_suspend() noexcept { return std::suspend_never{}; }
    constexpr void return_void() noexcept {}
    void unhandled_exception() { std::terminate(); }
  };
};

template <class T>
T sync_get(lazy<T> awaitable) {
  std::optional<T> slot;
  [&]() -> forget { slot = co_await awaitable; }();
  return std::move(*slot);
}

}  // namespace real::coro

namespace std {

template <class T, class... Args>
struct coroutine_traits<real::coro::lazy<T>, Args...> {
  using promise_type = real::coro::lazy_promise<T>;
};

}  // namespace std

#endif  // !REALPACK_INC_CORO_HPP
