// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_READ_BUFFER_TRANSPORT_H_
#define UTIL_BIFROST_PUBLIC_READ_BUFFER_TRANSPORT_H_

#include "base/basictypes.h"
#include "base/likely.h"
#include "network/public/net_buffer.h"

namespace util {
namespace bifrost {
class BlockingSocket;

class ReadBufferTransport {
 public:
  ReadBufferTransport(BlockingSocket* socket) : socket_(socket) {}
  ~ReadBufferTransport() {}

 public:
  // we must read len bytes data
  ssize_t ReadAll(uint8* buf, size_t len) {
    if (LIKELY(len <= buffer_.GetReadableSize())) {
      std::memcpy(buf, buffer_.Peek(), len);
      buffer_.Retrieve(len);
      return len;
    }

    ssize_t pre_read_bytes = PreReadAll(len - buffer_.GetReadableSize());
    if (UNLIKELY(pre_read_bytes <= 0)) {
      return pre_read_bytes;
    }
    return ReadAll(buf, len);
  }

  ssize_t ReadAllNonCopy(uint8** buf, size_t len) {
    if (LIKELY(len <= buffer_.GetReadableSize())) {
      *buf = reinterpret_cast<uint8*>(buffer_.Peek());
      // we can do it
      // buffer_.Retrieve will not modify memory content
      buffer_.Retrieve(len);
      return len;
    }

    ssize_t pre_read_bytes = PreReadAll(len - buffer_.GetReadableSize());
    if (UNLIKELY(pre_read_bytes <= 0)) {
      return pre_read_bytes;
    }
    return ReadAllNonCopy(buf, len);
  }

 private:
  ssize_t PreReadAll(size_t len);

 private:
  BlockingSocket* socket_;
  net::NetBuffer buffer_;

  static const int32 kExtraBuffSize = 65536;
  char extra_buff_[kExtraBuffSize];

 private:
  DISALLOW_COPY_AND_ASSIGN(ReadBufferTransport);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_READ_BUFFER_TRANSPORT_H_
