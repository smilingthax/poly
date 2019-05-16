#ifndef POLY_IEXTRA_H_
#define POLY_IEXTRA_H_

// Interface to reserve additional space in poly<>

namespace Poly {

template <int Count, typename TExtra=char>
struct IExtra {
  template <typename IBase>
  struct Iface : IBase {
    struct Dummy {
    private:
      typename IBase::Dummy orig;
      TExtra _extra[Count];
    };
  };
  template <typename T, typename Base>
  using Impl = Base;
};

} // namespace Poly

#endif
