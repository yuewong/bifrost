// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)
//
#ifndef UTIL_BIFROST_PUBLIC_RPC_CHANNEL_H_
#define UTIL_BIFROST_PUBLIC_RPC_CHANNEL_H_

#include "base/basictypes.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace google {
namespace protobuf {
// Defined in this file.
class Service;
class RpcController;

// Defined in other files.
class Descriptor;            // descriptor.h
class ServiceDescriptor;     // descriptor.h
class MethodDescriptor;      // descriptor.h
class Message;               // message.h
class Closure;
}  // namespace protobuf
}  // namespace google

namespace util {
namespace bifrost {
class RpcChannel : public google::protobuf::RpcChannel {
 public:
  inline RpcChannel() {}
  virtual ~RpcChannel() {}

  // Call the given method of the remote service.  The signature of this
  // procedure looks the same as Service::CallMethod(), but the requirements
  // are less strict in one important way:  the request and response objects
  // need not be of any specific class as long as their descriptors are
  // method->input_type() and method->output_type().
  virtual void CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller,
                          const google::protobuf::Message* request,
                          google::protobuf::Message* response,
                          google::protobuf::Closure* done);

 private:
  DISALLOW_COPY_AND_ASSIGN(RpcChannel);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_RPC_CHANNEL_H_
