// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/rpc_channel.h"

#include "thirdparty/protobuf/include/google/protobuf/descriptor.h"

namespace util {
namespace bifrost {
void RpcChannel::CallMethod(
    const google::protobuf::MethodDescriptor* method,
    google::protobuf::RpcController* controller,
    const google::protobuf::Message* request,
    google::protobuf::Message* response,
    google::protobuf::Closure* done) {
}
}  // namespace bifrost
}  // namespace util
