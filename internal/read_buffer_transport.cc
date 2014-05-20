// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/read_buffer_transport.h"

#include <sys/uio.h>
#include "util/bifrost/public/blocking_socket.h"

namespace util {
namespace bifrost {
ssize_t ReadBufferTransport::PreReadAll(size_t len) {
  if (len <= buffer_.GetWritableSize()) {
    ssize_t got = socket_->Read(
        reinterpret_cast<uint8*>(buffer_.WritePos()),
        buffer_.GetWritableSize());
    if (UNLIKELY(got <= 0)) {
      return -1;
    }
    buffer_.WriteDataSize(got);
    return got;
  }

  // buffer_.GetWritableSize > len
  // we read more data from extra_buff_
  // TODO(Moon) : Resize buffer
  while (len > 0) {
    ssize_t got = socket_->Read(
        reinterpret_cast<uint8*>(extra_buff_),
        kExtraBuffSize);
    if (UNLIKELY(got <= 0)) {
      return got;
    }
    len -= got;
    buffer_.Append(extra_buff_, got);
  }
}
}  // namespace bifrost
}  // namespace util
