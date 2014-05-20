// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/bifrost.h"

namespace util {
namespace bifrost {
void Bifrost::SetMonitorPort(uint16 port) {
  use_http_monitor_ = true;
}

void Bifrost::Raise() {
  CHECK(service_) << "you need register protobuf service";
  tcp_service_.Serve();
}

void Bifrost::Connect(net::Connection* connection) {
}

void Bifrost::RecvMessage(net::Connection* connection, net::NetBuffer* buff) {
}
}  // namespace bifrost
}  // namespace util
