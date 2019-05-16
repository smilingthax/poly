Poly::poly
==========

Yet another C++11 polymorphic interface pointer.

* Does not heap-allocate.
* Allows owned and non-owned pointers.
* If USE_UNSAFE_DERIVED_TO_BASE is defined:
  * smaller size, but vtable / small-object problem as [described here](http://stlab.cc/tips/small-object-optimizations.html).  
    Runtime checks will verify that pointers match, but do nothing to prevent the compiler
    from further exploiting the undefined behavior at compile time...
  * otherwise, an additional `IBase *` will be stored.

Copyright (c) 2019 Tobias Hoffmann

License: http://opensource.org/licenses/MIT

