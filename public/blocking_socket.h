// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_BLOCKING_SOCKET_H_
#define UTIL_BIFROST_PUBLIC_BLOCKING_SOCKET_H_

#include "base/basictypes.h"
#include "network/base/net_addr.h"

namespace util {
namespace bifrost {
enum  ConnectStatus {
  CONNECT_SUCCESS = 0,
  CONNECT_TIMEOUT = 1,
  CONNECTED = 2,
  SYS_ERROR = 3,
};

class BlockingSocket {
 public:
  BlockingSocket(const net::NetAddr& addr);
  ~BlockingSocket();

 public:
  bool IsOpen() const {
    return fd_ != kIllegalFileDescriptor;
  }

  ConnectStatus Open();

  void Close();

  // write date to socket loops until done or fail.
  ssize_t Write(const uint8* buf, uint32_t len);

  // read data from socket
  ssize_t Read(uint8* buf, uint32_t len);

  // whether to enable/disable Nagle's algorithm.
  void SetNoDelay(bool no_delay) {
    no_delay_ = no_delay;
  }

  void SetConnTimeout(int32 ms) {
    conn_timeout_ = ms;
  }

  void SetSendTimeout(int32 ms) {
    send_timeout_ = ms;
  }

  void SetRecvTimeout(int32 ms) {
    recv_timeout_ = ms;
  }

  // set the max number of recv retries
  void SetMaxRecvRetries(int32 num) {
    max_recv_retries_ = num;
  }

  FileDescriptor GetFd() const {
    return fd_;
  }

 private:
  ConnectStatus OpenConnection();

  FileDescriptor CreateBlockingSocketOrDie();

  void SetSendTimeoutInSocketOpt();

  void SetRecvTimeoutInSocketOpt();

  ssize_t Poll(FileDescriptor fd);

  ssize_t WritePartial(const uint8* buf, uint32_t len);

 private:
  static const int32 kDefaultMaxRecvRetries = 3;

 private:
  net::NetAddr peer_addr_;
  FileDescriptor fd_;

  /** connect timeout in ms */
  int32 conn_timeout_;

  /** send timeout in ms */
  int32 send_timeout_;

  /** recv timeout in ms */
  int32 recv_timeout_;

  bool no_delay_;

  int32 max_recv_retries_;

  struct timeval recv_timeval_;

 private:
  DISALLOW_COPY_AND_ASSIGN(BlockingSocket);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_BLOCKING_SOCKET_H_
