// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/blocking_socket.h"

#include <sys/epoll.h>

#include "base/cycleclock.h"
#include "base/likely.h"
#include "network/base/net.h"
#include "network/base/net_addr.h"

namespace util {
namespace bifrost {
BlockingSocket::BlockingSocket(const net::NetAddr& addr) :
    peer_addr_(addr),
    fd_(kIllegalFileDescriptor),
    conn_timeout_(0),
    send_timeout_(0),
    recv_timeout_(0),
    no_delay_(true),
    max_recv_retries_(kDefaultMaxRecvRetries) {
}

BlockingSocket::~BlockingSocket() {
  Close();
}

ConnectStatus BlockingSocket::Open() {
  if (IsOpen()) {
    return CONNECTED;
  }
  return OpenConnection();
}

void BlockingSocket::Close() {
  if (fd_ != kIllegalFileDescriptor) {
    net::ShutDownWrite(fd_);
    close(fd_);
    fd_ = kIllegalFileDescriptor;
  }
}

ConnectStatus BlockingSocket::OpenConnection() {
  fd_ = CreateBlockingSocketOrDie();
  if (send_timeout_ > 0) {
    SetSendTimeoutInSocketOpt();
  }

  if (recv_timeout_ > 0) {
    SetRecvTimeoutInSocketOpt();
  }

  net::SetTcpNoDelay(fd_, no_delay_);
  if (conn_timeout_ > 0) {
    net::SetNonBlockSocket(fd_);
  }
  int32 connect_success = net::Connect(fd_, peer_addr_.GetSockAddrIn());
  if (connect_success == 0) {
    return CONNECT_SUCCESS;
  }

  int32 copy_errno = errno;
  if (copy_errno != EINPROGRESS && copy_errno != EINTR) {
    Close();
    return SYS_ERROR;
  }

  ConnectStatus ret_status = CONNECT_SUCCESS;

  // epoll
  ssize_t event_num = Poll(fd_);
  if (event_num > 0) {
    // connect success but
    // ensure the socket is connected and that there are no errors set
    int32 val;
    socklen_t len = sizeof(val);
    int32 ret = getsockopt(fd_, SOL_SOCKET, SO_ERROR, &val, &len);
    if (ret == -1) {
      CHECK(false) << "getsockopt fault : " << net::GetErrorInfo(errno);
    }
    if (val == 0) {
      return CONNECT_SUCCESS;
    }
  } else if (event_num == 0) {
    // timeout
    ret_status = CONNECT_TIMEOUT;
  } else {
    // error
    ret_status = SYS_ERROR;
  }

  Close();
  return ret_status;
}

ssize_t BlockingSocket::Poll(FileDescriptor fd) {
  FileDescriptor poll_fd = epoll_create(1);
  struct epoll_event event;
  event.events = EPOLLOUT;
  epoll_ctl(poll_fd, EPOLL_CTL_ADD, fd_, &event);
  ssize_t event_num = epoll_wait(
      poll_fd,
      &event,
      1,
      conn_timeout_);
  close(poll_fd);
  return event_num;
}

void BlockingSocket::SetSendTimeoutInSocketOpt() {
  struct timeval s = {
    static_cast<int32>(send_timeout_ / 1000),
    static_cast<int32>((send_timeout_ % 1000) * 1000)};
  int32 ret = setsockopt(
      fd_,
      SOL_SOCKET,
      SO_SNDTIMEO,
      &s,
      sizeof(s));
  CHECK(ret == 0) << "setsockopt fault in set send timeout : "
                  << net::GetErrorInfo(errno);
}

void BlockingSocket::SetRecvTimeoutInSocketOpt() {
  recv_timeval_.tv_sec = static_cast<int32>(recv_timeout_ / 1000);
  recv_timeval_.tv_usec = static_cast<int32>((recv_timeout_ % 1000) * 1000);

  int32 ret = setsockopt(
      fd_,
      SOL_SOCKET,
      SO_RCVTIMEO,
      &recv_timeval_,
      sizeof(recv_timeval_));
  CHECK(ret == 0) << "setsockopt fault in set recv timeout: "
                  << net::GetErrorInfo(errno);
}

FileDescriptor BlockingSocket::CreateBlockingSocketOrDie() {
  FileDescriptor fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  CHECK(fd != kIllegalFileDescriptor);
  net::SetCloseOnExec(fd);
  return fd;
}

ssize_t BlockingSocket::Write(const uint8* buf, uint32_t len) {
  ssize_t sent = 0;
  while (sent < len) {
    ssize_t write_bytes = WritePartial(buf + sent, len - sent);
    if (write_bytes == -1) {
      Close();
      return -1;
    }
    sent += write_bytes;
  }
}

ssize_t BlockingSocket::WritePartial(const uint8* buf, uint32_t len) {
  if (UNLIKELY(!IsOpen())) {
    return -1;
  }

  int32 flags = 0;
#ifdef MSG_NOSIGNAL
  // Requests not to send SIGPIPE on errors on stream oriented sockets
  // when the other end breaks the
  // connection.  The EPIPE  error  is  still  returned.
  flags |= MSG_NOSIGNAL;
#endif // ifdef MSG_NOSIGNAL

  int32 write_bytes = static_cast<int>(send(fd_, buf, len, flags));
  if (write_bytes < 0) {
    if (errno == EWOULDBLOCK || errno == EAGAIN) {
      return 0;
    }
    if (errno == EPIPE || errno == ECONNRESET || errno == ENOTCONN) {
      return -1;
    }
  } else if (write_bytes == 0) {
    return -1;
  }

  return write_bytes;
}

ssize_t BlockingSocket::Read(uint8* buf, uint32_t len) {
  int32 retries = 0;

  int64 read_start = 0;
  int64 read_end = 0;
  int64 start = 0;
  if (recv_timeout_ > 0) {
    read_start = base::CycleClock::FastGetTimeInMs();
  }

try_again:
  int32 errno_copy = 0;
  int32 got = static_cast<int>(recv(fd_, buf, len, 0));
  if (recv_timeout_ > 0 && got < 0 && ++retries < max_recv_retries_) {
    errno_copy = errno;
    if (errno_copy ==  EAGAIN || errno_copy == EINTR) {
      read_end = base::CycleClock::FastGetTimeInMs();
      if (read_end - read_start < recv_timeout_) {
        goto try_again;
      }
      return -1;
    }
  }

  switch (errno_copy) {
    case 0:
      break;
    case ECONNRESET:
      LOG(ERROR) << "read error : " << net::GetErrorInfo(errno_copy);
      break;
    case ENOTCONN:
      LOG(ERROR) << "read error : " << net::GetErrorInfo(errno_copy);
      break;
    case ETIMEDOUT:
      LOG(ERROR) << "read error : " << net::GetErrorInfo(errno_copy);
      break;
  }

  return got;
}
}  // namespace bifrost
}  // namespace util
