// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#include "base/logging.h"
#include "thirdparty/gtest/gtest/gtest.h"
#include "network/base/net.h"
#include "util/bifrost/public/blocking_socket.h"

TEST(BlockingSocketTest, SimpleTest) {
  net::NetAddr addr("192.168.100.200", 9527);
  util::bifrost::BlockingSocket socket(addr);
  util::bifrost::ConnectStatus status = socket.Open();
  socket.SetConnTimeout(2000);
  if (status == util::bifrost::SYS_ERROR) {
    LOG(ERROR) << net::GetErrorInfo(errno);
  }
}
