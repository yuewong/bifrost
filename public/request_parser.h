// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_REQUEST_PARSER_H_
#define UTIL_BIFROST_PUBLIC_REQUEST_PARSER_H_

#include "base/basictypes.h"
#include "network/public/net_buffer.h"
#include "util/bifrost/proto/bifrost.pb.h"

namespace util {
namespace bifrost {
class RequestParser {
 public:
  enum RequestParseStatus {
    REQUEST_PARSE_START = 0,
    REQUEST_PARSE_SUCCESS = 1,
    REQUEST_PARSE_CONTINUOUS = 2,
    REQUEST_PARSE_FAULT = 3,
  };

 public:
  RequestParser() {}
  ~RequestParser() {}

 public:
  static RequestParseStatus Parse(net::NetBuffer* buffer, Request* request);

 private:
  DISALLOW_COPY_AND_ASSIGN(RequestParser);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_REQUEST_PARSER_H_
