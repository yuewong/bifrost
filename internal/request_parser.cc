// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/request_parser.h"

namespace util {
namespace bifrost {
RequestParser::RequestParseStatus RequestParser::Parse(
    net::NetBuffer* buffer,
    Request* request) {
  return RequestParser::REQUEST_PARSE_FAULT;
}
}  // namespace bifrost
}  // namespace util
