#ifndef POLY_IANY_H_
#define POLY_IANY_H_

#include <typeinfo>

namespace Poly {

struct IAny {
  template <typename IBase> struct Iface;
  template <typename T, typename Base> struct Impl;
};

template <typename IBase>
struct IAny::Iface : IBase {
  template <typename T>
  T *get() {
    return reinterpret_cast<T *>(_get_impl(typeid(T&), nullptr));
  }

  template <typename T>
  T *release() {
    return reinterpret_cast<T *>(_release_impl(typeid(T&)));
  }

  template <typename T>
  bool is() {
    using Tref = typename std::add_lvalue_reference<T>::type;
    void *wrong = this + 1;
    return (_get_impl(typeid(Tref), wrong) != wrong);
  }

private:
  virtual void *_get_impl(const std::type_info &want, void *wrong) = 0;
  virtual void *_release_impl(const std::type_info &want) = 0;
};

template <typename T, typename Base>
struct IAny::Impl : Base {
  using Base::Base;
private:
  void *_get_impl(const std::type_info &want, void *wrong) override {
    auto ptr = Base::get();
    // NOTE: typeid(*ptr) would require ptr!=nullptr
    return (typeid(decltype(*ptr)) == want) ? ptr : wrong;
  }
  void *_release_impl(const std::type_info &want) override {
    auto ptr = Base::_release_ptr();
    return (typeid(decltype(*ptr)) == want) ? ptr : nullptr;
  }
};

template <typename Base>
struct IAny::Impl<void, Base> : Base {
  using Base::Base;
private:
  void *_get_impl(const std::type_info &want, void *wrong) override {
    return (typeid(void) == want) ? nullptr : wrong;
  }
  void *_release_impl(const std::type_info &want) override {
    return nullptr;
  }
};

} // namespace Poly

#endif
