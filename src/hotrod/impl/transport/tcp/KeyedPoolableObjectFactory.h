#ifndef ISPN_HOTROD_TRANSPORT_KEYEDPOOLABLEOBJECTFACTORY_H
#define ISPN_HOTROD_TRANSPORT_KEYEDPOOLABLEOBJECTFACTORY_H



namespace infinispan {
namespace hotrod {
namespace transport {

template <class K, class V> class KeyedPoolableObjectFactory
{
  public:
    virtual void activateObject(const K& key, V& val) = 0;
    virtual void destroyObject(const K& key, V& val) = 0;
    virtual V& makeObject(const K& key) = 0;
    virtual void passivateObject(const K& key, V& val) = 0;
    virtual bool validateObject(const K& key, V& val) = 0;

    virtual ~KeyedPoolableObjectFactory() {}
};

}}} // namespace infinispan::hotrod::transport

#endif  /* ISPN_HOTROD_TRANSPORT_KEYEDPOOLABLEOBJECTFACTORY_H */
