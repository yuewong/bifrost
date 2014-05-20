// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "util/bifrost/internal/framed_transport.h"

#include <zlib.h>

#include "network/base/net.h"
#include "thirdparty/protobuf/include/google/protobuf/message.h"
#include "util/bifrost/public/blocking_socket.h"

namespace util {
namespace bifrost {
const char* FramedTransport::kMagic = "03ec";

void FramedTransport::WriteSize(size_t s) {
  uint64 size = s;
  write_buffer_.SetPrepend(s);
}

void FramedTransport::WriteVersion() {
  COMPILE_ASSERT(sizeof(kVersion) == 4, uint32_must_be_4_byte);
  write_buffer_.Append(&kVersion, sizeof(kVersion));
}

void FramedTransport::WriteMagic() {
  write_buffer_.Append(kMagic, kMagicNumLen);
}

uint32 FramedTransport::CalChechSum(char* buf, int32 len) {
  return adler32(1,
                 reinterpret_cast<const Bytef*>(write_buffer_.Peek()),
                 write_buffer_.GetReadableSize());
}

bool FramedTransport::CheckSum(uint8* data, size_t len, uint32 expect_sum) {
  return adler32(1, data, len) == expect_sum;
}

bool FramedTransport::WriteMessage(const google::protobuf::Message* request) {
  int32 request_size = request->ByteSize();
  if (request_size + kOtherDataSize < write_buffer_.Capacity()) {
    write_buffer_.ClearAndResize((request_size + kOtherDataSize) * 2);
  }

  WriteMagic();
  WriteVersion();
  request->SerializeToArray(write_buffer_.Peek(), request_size);
  write_buffer_.WriteDataSize(request_size);
  uint32 check_sum = CalChechSum(write_buffer_.Peek(), write_buffer_.GetReadableSize());
  WriteUint32(check_sum);
  CHECK(write_buffer_.GetReadableSize() == (request_size + kOtherDataSize));
  WriteSize(write_buffer_.GetReadableSize());
  // TODO(Moon) : client support async
  bool write_success = Flush();
  write_buffer_.Clear();

  return write_success;
}

void FramedTransport::WriteUint32(uint32 value) {
  write_buffer_.Append(&value, sizeof(value));
}

bool FramedTransport::Flush() {
  return socket_->Write(
      reinterpret_cast<const uint8*>(write_buffer_.RawPeek()),
      write_buffer_.GetReadableSize() + net::NetBuffer::kPrependSize);
}

bool FramedTransport::ReadUint64(uint64* size) {
  ssize_t got = read_transport_.ReadAll(
      reinterpret_cast<uint8*>(size),
      sizeof(uint64));
  if (got <= 0) {
    LOG(ERROR) << net::GetErrorInfo(errno);
    return false;
  }
  CHECK_EQ(got, sizeof(uint64));
  return true;
}

bool FramedTransport::ReadMessage(google::protobuf::Message* reponse) {
  uint64* size = nullptr;
  ssize_t got = read_transport_.ReadAllNonCopy(
      reinterpret_cast<uint8**>(&size),
      sizeof(uint64));
  CHECK(got == sizeof(*size));
  uint8* data = nullptr;
  got = read_transport_.ReadAllNonCopy(&data, *size);
  CHECK(got == *size);

  uint8* magic = data + kMagicOffset;
  CHECK(ChechMagicNum(magic));

  uint32* version = reinterpret_cast<uint32*>(data + kVersionOffset);
  CHECK(CheckVersion(version));

  size_t check_sum_length = *size - sizeof(uint32);
  uint32* check_sum = reinterpret_cast<uint32*>(data + check_sum_length);
  CHECK(CheckSum(data, check_sum_length, *check_sum));

  // version + magic + checksum
  size_t payload_size = *size - sizeof(uint32) * 3;
  reponse->ParseFromArray(data + kPayloadOffset, payload_size);
  return true;
}
}  // namespace bifrost
}  // namespace util
