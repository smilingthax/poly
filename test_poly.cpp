#include "poly.h"
#include "poly_iextra.h"
#include <stdio.h>

struct IOther {
  template <typename IBase>
  struct Iface : IBase {
    virtual void more() = 0;
  };
  template <typename T, typename Base>
  struct Impl : Base {
    using Base::Base;
    void more() override {
      auto self = Base::get(); // could be nullptr, could be void *
      printf("more called %p\n", self);
    }
  };
};

template <typename Derived, typename Base=void>
struct Clonable : Base {
  using Base::Base;
  Derived *clone() const {
    return static_cast<Derived *>(clone_impl());
  }
private:
  virtual Clonable *clone_impl() const {
    return new Derived(*static_cast<const Derived *>(this));
  }
};

template <typename Derived>
struct Clonable<Derived, void> {
  virtual ~Clonable() = default;
  Derived *clone() const {
    return static_cast<Derived *>(clone_impl());
  }
private:
  virtual Clonable *clone_impl() const {
    return new Derived(*static_cast<const Derived *>(this));
  }
};

struct Foo : Clonable<Foo> {
  ~Foo() { printf("Foo dtor\n"); }
//  int x;
};

void test_extra()
{
  Poly::poly<> p;
  Poly::poly<Poly::IExtra<5>> pe;
//  Poly::poly<Poly::IExtra<1,void*>> pe;
  printf("%d %d\n", sizeof(p), sizeof(pe));
}

int main()
{
//  int i1=5;
//  Poly::poly<Poly::IAny> pi(&i1, false);
  Poly::poly<Poly::IAny> pi(new int{6});

//  Poly::poly<Poly::IAny> p;
//  Poly::poly<Poly::IAny, IOther> p;
  Poly::poly<IOther, Poly::IAny> p;

//  Poly::poly<Poly::IClonable, Poly::IAny> pf{new Foo};
//  Poly::poly<Poly::IAny, Poly::IClonable> p;

//  p.reset(pf->release<Foo>());

//Foo f; delete f.clone();

//p.reset((Foo*)0, false);
//  p.reset(new Foo);
//  p->clone();
p->more();

//p=std::move(pi);

/*
*/
  printf("%d\n", p->is<void>());
  printf("%d\n", p->is<int>());
  printf("%d\n", p->is<char>());

//  test_extra();

  return 0;
}
