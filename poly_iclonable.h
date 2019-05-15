#ifndef POLY_ICLONABLE_H_
#define POLY_ICLONABLE_H_

//#include <new>

namespace Poly {

struct IClonable {
  template <typename IBase> struct Iface;
  template <typename T, typename Base> struct Impl;
};

template <typename IBase>
struct IClonable::Iface : IBase {
  virtual typename IBase::Poly clone() const = 0;
};

template <typename T, typename Base>
struct IClonable::Impl : Base {
  using Base::Base;

  typename Base::Poly clone() const override {
    // NOTE: could be (T*)0 !
    return Base::get()->clone();
  }
};

template <typename Base>
struct IClonable::Impl<void, Base> : Base {
  using Base::Base;

  typename Base::Poly clone() const override {
    return {};
//    throw std::bad_alloc();
  }
};

} // namespace Poly

#endif
