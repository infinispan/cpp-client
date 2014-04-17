#ifndef ISPN_HOTROD_TYPES_H
#define ISPN_HOTROD_TYPES_H

#include "infinispan/hotrod/defs.h"
#include "infinispan/hotrod/ScopedBuffer.h"

#include <exception>
#include <memory>
#include <iostream>

namespace infinispan {
namespace hotrod {

/*
 * A limited functionality scoped_ptr.  Defined here because auto_ptr
 * deprecated, unique_ptr too new, shared_ptr too heavy, and boost not
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


/*
 * A Java byte[] substitute class.  Can be toggled smart or dumb.  Not
 * to be exposed as a public interface.  More performant version
 * surely possible.  Hopefully efficient with return value
 * optimization based on observed frequent use of Java code:
 *
 *   byte[] result = foo();
 *
 * Basic use: 
 *   hrbyes mybytes(addr, len); // no ownership
 *
 *   mybytes.setSmart(new char[len], len); // ownership
 *   mybytes.reserve(len);      // same as line above
 *
 * TODO: move to auto_ptr/unique_ptr platform neutral implementation
 * (good), add support to work with a buffer pool (better).
 *
 */

class hrbytes {
 public:
    struct HrbDeleter {
        void operator()(char* p) {
            if (p == 0) {
                // called via get_deleter from below
                disabled = true;
                return;
            }
                
            if (!disabled) {
                delete[] p;
            }
        }
        bool disabled;
    };

    unsigned int length() const { return len; }
    char* bytes() const { return dumbBytes ? dumbBytes : smartBytes.get(); }

    hrbytes() : dumbBytes(0), len(0) {}
    hrbytes(char *b, unsigned int l) :  dumbBytes(b), len(l) {}

    void set(char *b, unsigned int l) {
        smartBytes.reset();
        len = l;
        dumbBytes = b;
    }
    void setSmart(char *b, unsigned int l) {
        smartBytes.reset(b, HrbDeleter());
        len = l;
        dumbBytes = 0;
    }
    void reserve(unsigned int l) {
        dumbBytes = 0;
        len = l;
        smartBytes.reset(new char[len], HrbDeleter());
    }

    hrbytes(hrbytes const &other) {
        len = other.len;
        dumbBytes = other.dumbBytes;
        smartBytes = other.smartBytes;
        if (!dumbBytes) other.dumbBytes = smartBytes.get();
    }
    hrbytes& operator=(hrbytes const &other) {
        len = other.len;
        dumbBytes = other.dumbBytes;
        smartBytes = other.smartBytes;
        if (!dumbBytes) other.dumbBytes = smartBytes.get();
        return *this;
    }
        
    static void delayedDelete (ScopedBuffer *buf) {
        delete[] buf->getBytes();
    }
    void releaseTo(ScopedBuffer& buf) const {
        // TODO: assert smartBytes.unique()
	if (smartBytes) {
            buf.set(smartBytes.get(), len, &delayedDelete);
            (*std::tr1::get_deleter<HrbDeleter>(smartBytes))(0);
	}
	else 
	    buf.set(dumbBytes, len);
    }

  private:
    mutable char* dumbBytes;
    unsigned int len;
    HR_SHARED_PTR<char> smartBytes;
};

}} // namespace infinispan::hotrod


namespace std {

template<class T> struct less;

template<> struct less<infinispan::hotrod::hrbytes>
{
    bool operator() (
        const infinispan::hotrod::hrbytes& b1,
        const infinispan::hotrod::hrbytes& b2) const
    {
        return b1.bytes() < b2.bytes();
    }
};

} // namespace std

#endif  /* ISPN_HOTROD_TYPES_H */
