#ifndef POLY_STORAGE_H_
#define POLY_STORAGE_H_

#include <utility>

namespace Poly {

namespace detail {

template <bool Owned, typename T, typename Base>
struct Storage_Impl_Outer final : Base {
  using Base::Base;
  ~Storage_Impl_Outer() override {
    // if constexpr (Owned && !std::is_same_v<T,void>)
    _do_delete(Base::get());
//    _do_delete(Base::_release_ptr());
  }

private:
  template <typename U>
  static void _do_delete(U *ptr) {
    if (Owned) {
      delete ptr;
    }
  }
  static void _do_delete(void *) {
    static_assert(!Owned, "void * cannot be owned");
  }

  void _move_impl(typename Base::Poly &dst) override {
    dst.template put<Owned>(Base::_release_ptr());
  }
};

template <typename T, typename Base>
struct Storage_Impl_Inner : Base {
  Storage_Impl_Inner(T *ptr) : _ptr(ptr) { }

protected:
  T *get() const { return _ptr; }
  T *_release_ptr() { T *ret = _ptr; _ptr = nullptr; return ret; }
private:
  T *_ptr;
};

template <typename PolyT>
struct Storage_Iface {
  virtual ~Storage_Iface() = default;
  Storage_Iface(const Storage_Iface &) = delete;
  Storage_Iface(Storage_Iface &&) = delete;
  Storage_Iface &operator=(const Storage_Iface &) = delete;
  Storage_Iface &operator=(Storage_Iface &&) = delete;

  using Poly = PolyT;
  using Dummy = Storage_Impl_Outer<false, void, Storage_Impl_Inner<void, Storage_Iface>>;
protected:
  Storage_Iface() = default;
private:
  friend Poly;
  virtual void _move_impl(Poly &dst) = 0;
};

} // namespace detail

} // namespace Poly

#endif
