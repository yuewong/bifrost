// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_NONBLOCKING_SERVER_H_
#define UTIL_BIFROST_PUBLIC_NONBLOCKING_SERVER_H_

#include "base/basictypes.h"
#include "network/public/tcp_server.h"

namespace google {
namespace protobuf {
class Service;
}  // namespace protobuf
}  // namespace google

namespace util {
namespace bifrost {
class NonblockingServer {
 public:
  NonblockingServer(uint16 port, int32 threads_num = 1) :
      server_(port, threads_num),
      rpc_server_(nullptr) {
    Init();
  }

  NonblockingServer(const std::string& ip, uint16 port, int32 threads_num = 1) :
      server_(ip, port, threads_num),
      rpc_server_(nullptr) {
    Init();
  }

  ~NonblockingServer() {}

 void Serve();

 void SetRpcServer(google::protobuf::Service* rpc_server) {
  rpc_server_ = rpc_server;
 }

 private:
  void Init();

  void NewConnection(net::Connection* connection);

 private:
  net::TcpServer server_;
  google::protobuf::Service* rpc_server_;

 private:
  DISALLOW_COPY_AND_ASSIGN(NonblockingServer);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_NONBLOCKING_SERVER_H_
