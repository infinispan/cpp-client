#ifndef ISPN_HOTROD_TYPES_H
#define ISPN_HOTROD_TYPES_H

#include "infinispan/hotrod/defs.h"

#include <exception>
#include <memory>
#include <iostream>
#include <vector>
namespace infinispan {
namespace hotrod {

/*
 * A limited functionality scoped_ptr.  Defined here because auto_ptr
 * deprecated, unique_ptr too new, std::shared_ptr too heavy, and boost not
 * available.  Main use is to allow C++ code to resemble the original
 * Java code in program flow for short lived objects.  Replace with
 * another solution if it instead promotes contorted code.
 *
 * Single use, non-copyable.
 */
template <class T> class hr_scoped_ptr {
 public:
    hr_scoped_ptr (T* p) : px(p) {
        // TODO: replace with hotrod exception
        if (!p) throw std::exception();
    }
    ~hr_scoped_ptr() { delete(px); }

    T& operator*() {return *px ; }
    T* operator->() { return px; }

  private:
    hr_scoped_ptr(hr_scoped_ptr const &);
    hr_scoped_ptr& operator=(hr_scoped_ptr const &);

    T* px;
};

}} // namespace infinispan::hotrod


#endif  /* ISPN_HOTROD_TYPES_H */
