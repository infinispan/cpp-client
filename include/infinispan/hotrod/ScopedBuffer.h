#ifndef ISPN_HOTROD_SCOPEDBUFFER_H
#define ISPN_HOTROD_SCOPEDBUFFER_H



#include "infinispan/hotrod/exceptions.h"

namespace infinispan {
namespace hotrod {

/*
 * A semi-smart ptr object to hold intermediate marshalled data in the
 * lifetime of a block.  For default case, the bytes are "owned" by
 * the scope and freed when the dtor is called on block exit.  Not for
 * general use.  Intended for short lived serialized bytes passed
 * between a marshaller and Hot Rod.  TODO: better name.
 */

class ScopedBuffer
{
  public:
    typedef void (*ReleaseFunc)(ScopedBuffer *);
    ScopedBuffer() : bytes(0), len(0), release(0) {}
    ~ScopedBuffer() {
        if (release)
            (*release)(this);
        else
            delete[] bytes;
    }
    void set(char *b, size_t l, ReleaseFunc m = 0) {
        if (bytes || len || release) throw HotRodClientException("ScopedBuffer reuse");
        bytes = b; len = l; release = m;
    }
    char* getBytes() const { return bytes; }
    size_t getLength() const { return len; }
    ReleaseFunc getRelease() const { return release; }
    
  private:
    ScopedBuffer(const ScopedBuffer &);
    ScopedBuffer& operator=(ScopedBuffer const &);

    char* bytes;
    size_t len;
    ReleaseFunc release;
};

}} // namespace

#endif  /* ISPN_HOTROD_SCOPEDBUFFER_H */
