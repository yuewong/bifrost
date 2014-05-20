// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_BIFROST_H_
#define UTIL_BIFROST_PUBLIC_BIFROST_H_

#include "base/basictypes.h"
#include "network/base/net_addr.h"
#include "network/public/tcp_server.h"

namespace google {
namespace protobuf {
class Service;
}  // namespace protobuf
}  // namespace google

namespace net {
class Connection;
}  // namespace net

namespace util {
namespace bifrost {
class Bifrost {
 public:
  Bifrost(uint16 port, int thread_num) :
      tcp_service_(port, thread_num),
      service_(nullptr),
      use_http_monitor_(false) {}
  ~Bifrost() {}

 public:
  void RegisterService(google::protobuf::Service* service) {
    service_ = service;
  }

  void Connect(net::Connection* connection);

  void RecvMessage(net::Connection* connection, net::NetBuffer* buff);

  void Raise();

  void SetMonitorPort(uint16 port);

  bool IsOpenMonitor() {
    return use_http_monitor_;
  }

  uint16 GetPort() const {
    tcp_service_.GetPort();
  }

  void GetIP(std::string* ip) const {
    tcp_service_.GetIp(ip);
  }

 private:
  google::protobuf::Service* service_;
  net::TcpServer tcp_service_;

  bool use_http_monitor_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Bifrost);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_BIFROST_H_
