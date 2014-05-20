// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_REQUEST_CONTEXT_H_
#define UTIL_BIFROST_PUBLIC_REQUEST_CONTEXT_H_

#include "base/basictypes.h"
#include "util/bifrost/proto/bifrost.pb.h"
#include "util/bifrost/public/rpc_channel.h"
#include "util/bifrost/public/request_parser.h"

namespace util {
namespace bifrost {
class RequestContext {
 public:
  RequestContext() : status_(RequestParser::REQUEST_PARSE_START) {}

  RpcChannel* GetRpcChannel() {
    return &channel_;
  }

  Request* GetRequest() {
    return &request_;
  }

 private:
  RpcChannel channel_;
  Request request_;

  RequestParser::RequestParseStatus status_;

 private:
  DISALLOW_COPY_AND_ASSIGN(RequestContext);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_REQUEST_CONTEXT_H_
