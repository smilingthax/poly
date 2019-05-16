#ifndef POLY_H_
#define POLY_H_

#include "poly_storage.h"
#include "poly_iany.h"
#include "poly_iclonable.h"

//#define USE_UNSAFE_DERIVED_TO_BASE

#include <new>

namespace Poly {

template <typename... Ifaces>
class poly;

namespace detail {
#if __cplusplus >= 201703L   // #if __cpp_lib_launder >= 201606L
  using std::launder;
#else
  template <typename T>
  constexpr T* launder(T* p) noexcept {
    // ...static assert to exclude e.g. function pointers
    return p;
  }
#endif

template <typename Storage, typename...>
struct IBase {
  using type = Storage;
};

template <typename Storage, typename Iface, typename... Ifaces>
struct IBase<Storage, Iface, Ifaces...> {
  using type = typename Iface::template Iface<
    typename IBase<Storage, Ifaces...>::type
  >;
};

template <typename... Ifaces>
using IBases = typename IBase<
  Storage_Iface<poly<Ifaces...>>, Ifaces...
>::type;

template <typename T, typename...>
struct Derive;

template <typename T, typename Storage>
struct Derive<T, Storage> {
  using type = Storage;
};

template <typename T, typename Iface, typename... Ifaces>
struct Derive<T, Iface, Ifaces...> {
  using type = typename Iface::template Impl<
    T, typename Derive<T, Ifaces...>::type
  >;
};

template <bool Owned, typename T, typename IBase, typename... Ifaces>
using Derived = Storage_Impl_Outer<Owned, T, typename Derive<
  T, Ifaces..., Storage_Impl_Inner<T, IBase>
>::type>;

} // namespace detail

template <typename... Ifaces>
class poly {
public:
  typedef typename detail::IBases<Ifaces...> IBase;

  poly() {
    put<false>((void *)0);
  }

  template <typename T>
  poly(T *ptr) {
    put<true>(ptr);
  }

  template <typename T>
  poly(T *ptr, bool take) {
    put(ptr, take);
  }

  ~poly() {
    IBase *base = this->operator->();
    // should be virtual
    base->~IBase();
  }

  poly(poly &&rhs) {
    rhs->_move_impl(*this);
    // rhs.reset();  // already released. keep old type/.is() ...?
  }

  poly &operator=(poly &&rhs) {
    rhs->_move_impl(*this);
    // rhs.reset();
    return *this;
  }

  void reset() {
    destroy();
    put<false>((void *)0);
  }

  template <typename T>
  void reset(T *ptr, bool take=true) {
    destroy();
    put(ptr, take);
  }

  IBase *operator->() {
#ifdef USE_UNSAFE_DERIVED_TO_BASE
    return detail::launder(reinterpret_cast<IBase *>(&data));
#else
    return ptr;
#endif
  }

#if 0
  const IBase *operator->() const {
#ifdef USE_UNSAFE_DERIVED_TO_BASE
    return detail::launder(reinterpret_cast<const IBase *>(&data));
#else
    return ptr;
#endif
  }
#endif

  // explicit operator bool() const noexcept { ... is void ? ... } ?
  // dynamic typeid would have to check both Owned=true/false

private:
  template <bool Owned, typename T, typename Base>
  friend struct detail::Storage_Impl_Outer;

  using Dummy = typename IBase::Dummy;
  // typename detail::Derived<false, void, IBase, Ifaces...> dummy;

  template <bool Owned, typename T>
  void put(T *ptr) {
    using Impl = typename detail::Derived<Owned, T, IBase, Ifaces...>;

    static_assert(alignof(Dummy)==alignof(Impl), "alignment mismatch");
    static_assert(sizeof(dummy)==sizeof(Impl), "implementation too big");
    IBase *base = ::new((void *)&data) Impl{ptr};
#ifdef USE_UNSAFE_DERIVED_TO_BASE
    // NOTE: neither IBase nor Impl is standard layout!
    if (base != (void *)&data) {
      throw std::bad_alloc(); // or something else. undefined behavior ...
    }
#else
    this->ptr = base;
#endif
  }

  template <typename T>
  void put(T *ptr, bool take) {
    if (take) {
      put<true>(ptr);
    } else {
      put<false>(ptr);
    }
  }

  void destroy() {
    IBase *base = this->operator->();
    // should be virtual
    base->~IBase();
  }

private:
#ifndef USE_UNSAFE_DERIVED_TO_BASE
  IBase *ptr;
#endif
  union {
    Dummy dummy;
    unsigned char data[];
  };
};

} // namespace Poly

#endif
