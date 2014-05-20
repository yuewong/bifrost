// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "base/logging.h"
#include "util/bifrost/internal/nonblocking_server.h"

namespace util {
namespace bifrost {
void NonblockingServer::Serve() {
  CHECK(rpc_server_) << "you must set rpc server";
  server_.Serve();
}

void NonblockingServer::NewConnection(net::Connection* connection) {
}

void NonblockingServer::Init() {
}
}  // namespace bifrost
}  // namespace util
