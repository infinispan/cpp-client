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
    size_t size() const { return smartBytes.size(); }
    char* data() const { return const_cast<char*>(smartBytes.data()); }

    hrbytes()  {}
    hrbytes(char *b, size_t l) :  smartBytes(b,b+l) {}
    hrbytes(std::string s) : smartBytes(s.begin(),s.end()) {}

    hrbytes(hrbytes const &other) {
        smartBytes = other.smartBytes;
    }
    hrbytes& operator=(hrbytes const &other) {
        smartBytes = other.smartBytes;
        return *this;
    }
    operator const std::vector<char>& () const
    {
        return smartBytes;
    }
    std::vector<char> getVecto() const
    {
        return smartBytes;
    }
    std::vector<char>::const_iterator begin()
    {
        return smartBytes.begin();
    }
    std::vector<char>::const_iterator end()
    {
        return smartBytes.end();
    }

  private:
    std::vector<char> smartBytes;
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
        return b1.data() < b2.data();
    }
};

} // namespace std

#endif  /* ISPN_HOTROD_TYPES_H */
