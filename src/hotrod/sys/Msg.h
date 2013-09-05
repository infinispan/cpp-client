#ifndef ISPN_HOTROD_MSG_H
#define ISPN_HOTROD_MSG_H

#include <sstream>
#include <iostream>

namespace infinispan {
namespace hotrod {
namespace sys {

/** A simple wrapper for std::ostringstream that allows
 * in place construction of a message and automatic conversion
 * to string.
 * E.g.
 *@code
 * void foo(const std::string&);
 * foo(Msg() << "hello " << 32);
 *@endcode
 * Will construct the string "hello 32" and pass it to foo()
 */
struct Msg {
    std::ostringstream os;
    Msg() {}
    Msg(const Msg& m) : os(m.str()) {}
    std::string str() const { return os.str(); }
    operator std::string() const { return str(); }

    Msg& operator<<(long n) { os << n; return *this; }
    Msg& operator<<(unsigned long n) { os << n; return *this; }
    Msg& operator<<(bool n) { os << n; return *this; }
    Msg& operator<<(short n) { os << n; return *this; }
    Msg& operator<<(unsigned short n) { os << n; return *this; }
    Msg& operator<<(int n) { os << n; return *this; }
    Msg& operator<<(unsigned int n) { os << n; return *this; }
#ifdef _GLIBCXX_USE_LONG_LONG
    Msg& operator<<(long long n) { os << n; return *this; }
    Msg& operator<<(unsigned long long n) { os << n; return *this; }
#endif
    Msg& operator<<(double n) { os << n; return *this; }
    Msg& operator<<(float n) { os << n; return *this; }
    Msg& operator<<(long double n) { os << n; return *this; }

    template <class T> Msg& operator<<(const T& t) { os <<t; return *this; }
};



inline std::ostream& operator<<(std::ostream& o, const Msg& m) {
    return o << m.str();
}

/** Construct a message using operator << and append (file:line) */
#define QUOTE_(x) #x
#define QUOTE(x) QUOTE_(x)
#define HOTROD_MSG(message) (::infinispan::hotrod::sys::Msg() << message << " (" __FILE__ ":" QUOTE(__LINE__) ")")

}}} // namespace

#endif  /* ISPN_HOTROD_MSG_H */
