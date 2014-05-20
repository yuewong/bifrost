// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_FRAMED_TRANSPORT_H_
#define UTIL_BIFROST_PUBLIC_FRAMED_TRANSPORT_H_

#include "base/basictypes.h"
#include "network/public/net_buffer.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"
#include "util/bifrost/public/read_buffer_transport.h"

namespace util {
namespace bifrost {
class BlockingSocket;

// wire format
// Field    Length    Content
// size     8-byte    N + 8 (N-byte + 4-byte checksum)
// magic    4-byte    "03ec"
// version  4-byte    0x80010000
// payload  N-byte    payload
// checksum 4-byte    adler32 of (magic + version + payload)

class FramedTransport {
 public:
  FramedTransport(BlockingSocket* socket) :
      socket_(socket),
      read_transport_(socket) {}
  ~FramedTransport() {}

 public:
  ssize_t Read(uint8* buf, int32 len);

  bool WriteMessage(const google::protobuf::Message* request);

  bool ReadMessage(google::protobuf::Message* reponse);

 private:
  bool ReadUint64(uint64* size);

  void WriteSize(size_t s);

  void WriteVersion();

  void WriteMagic();

  void WriteUint32(uint32 value);

  uint32 CalChechSum(char* buf, int32 len);

  bool Flush();

  bool ChechMagicNum(uint8* data) {
    return std::strncmp(
        reinterpret_cast<const char*>(data),
        kMagic,
        kMagicNumLen) == 0;
  }

  bool CheckVersion(uint32* version) {
    return *version == kVersion;
  }

  bool CheckSum(uint8* data, size_t len, uint32 expect_sum);

 private:
  static const uint32 kVersion = 0x80010000;
  static const char* kMagic;
  static const int32 kMagicNumLen = 4;
  static const int32 kOtherDataSize = 8 + 4 + 4 + 4;
  static const int32 kMagicOffset = 0;
  static const int32 kVersionOffset = 4;
  static const int32 kPayloadOffset = 8;

 private:
  net::NetBuffer write_buffer_;
  net::NetBuffer read_buffer_;

  BlockingSocket* socket_;
  ReadBufferTransport read_transport_;

 private:
  DISALLOW_COPY_AND_ASSIGN(FramedTransport);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_FRAMED_TRANSPORT_H_
