#ifndef ISPN_HOTROD_MARSHALLER_H
#define ISPN_HOTROD_MARSHALLER_H

#include <vector>

namespace infinispan {
namespace hotrod {

/**
 *  Marshaller is the interface definition for a generic Marshaller
 */
template <class T> class Marshaller
{
  public:
    /**
     * Given an object of type T populate the vector<char> with the equivalent representation.
     *
     * \param obj the initial object
     * \param buff the vector to be populated
     */
    virtual void marshall(const T& obj, std::vector<char>& buff) = 0;
    /**
     * Given vector<char> return a new type T object
     *
     * \param buff the vector representing the object
     * \return the object equivalent to the vector
     */
    virtual T* unmarshall(const std::vector<char>& buff) = 0;

    virtual ~Marshaller() {}
    static void destroy(Marshaller<T> *marshaller) { delete marshaller; }
};

}} // namespace

#endif  /* ISPN_HOTROD_MARSHALLER_H */
