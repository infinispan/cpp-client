#ifndef ISPN_HOTROD_VERSIONEDOPERATIONRESPONSE_H
#define ISPN_HOTROD_VERSIONEDOPERATIONRESPONSE_H



namespace infinispan {
namespace hotrod {

class VersionedOperationResponse
{
  public:
    enum RspCode {
        SUCCESS,
        NO_SUCH_KEY,
        MODIFIED_KEY
    };

    VersionedOperationResponse(const std::vector<char>& val, RspCode c) :
       value(val), code(c) {}

    bool isUpdated() {
        return code==SUCCESS;
    }

    const std::vector<char>& getValue() {
        return value;
    }

    RspCode getCode() {
        return code;
    }

    private:
        std::vector<char> value;
        RspCode code;
};

}} // namespace infinispan::hotrod

#endif // ISPN_HOTROD_VERSIONEDOPERATIONRESPONSE_H
