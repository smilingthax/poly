#ifndef POLY_STORAGE_H_
#define POLY_STORAGE_H_

#include <utility>

namespace Poly {

namespace detail {

template <bool Owned, typename T, typename Base>
struct Storage_Impl : Base {
  Storage_Impl(T *ptr) : _ptr(ptr) { }
  ~Storage_Impl() override {
    // if constexpr (Owned && !std::is_same_v<T,void>)
    _do_delete(_ptr);
  }

  Storage_Impl &operator=(Storage_Impl &&rhs) {
    std::swap(_ptr, rhs._ptr);
    return *this;
  }
protected:
  T *get() const { return _ptr; }
  T *_release_ptr() { T *ret = _ptr; _ptr = nullptr; return ret; }
private:
  void _move_impl(typename Base::Poly &dst) override {
    dst.template put<Owned>(_release_ptr());
  }

  template <typename U>
  static void _do_delete(U *ptr) {
    if (Owned) {
      delete ptr;
    }
  }
  static void _do_delete(void *) {
    static_assert(!Owned, "void * cannot be owned");
  }
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
  using Dummy = Storage_Impl<false, void, Storage_Iface>;
protected:
  Storage_Iface() = default;
private:
  friend Poly;
  virtual void _move_impl(Poly &dst) = 0;
};

} // namespace detail

} // namespace Poly

#endif
