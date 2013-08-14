/*
 * Builder.h
 *
 *  Created on: Jul 18, 2013
 *      Author: samuele
 */

#ifndef BUILDER_H_
#define BUILDER_H_

namespace infinispan {
namespace hotrod {


template <class T> class Builder
{
  public:
    virtual void validate() {};
    virtual T create() = 0;
    virtual Builder<T>& read(T& bean) = 0;
    virtual ~Builder() {};
};

}} //namespace

#endif /* BUILDER_H_ */
