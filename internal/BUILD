# Copyright 2014 Makepolo Inc. All Rights Reserved.
# Author: huangyue@makepolo.com (Yue Huang)

cc_library(
    name = 'rpc_channel_internal',
    srcs = [
           'rpc_channel.cc',
           ],
    deps = [
           '//base:base',
           '//thirdparty/protobuf/lib:protobuf',
           ],
)

cc_library(
    name = 'blocking_socket_internal',
    srcs = [
           'blocking_socket.cc',
           ],
    deps = [
           '//base:base',
           ],
)

cc_library(
    name = 'framed_transport_internal',
    srcs = [
           'framed_transport.cc',
           ],
    deps = [
           ':blocking_socket_internal',
           '//base:base',
           '//network/public:net_buffer',
           ],
)

cc_library(
    name = 'read_buffer_transport_internal',
    srcs = [
           'read_buffer_transport.cc',
           ],
    deps = [
           ':blocking_socket_internal',
           '//base:base',
           '//network/public:net_buffer',
           ],
)

cc_library(
    name = 'controller_internal',
    srcs = [
           'controller.cc',
           ],
    deps = [
           '//base:base',
           ],
)

cc_library(
    name = 'nonblocking_server_internal',
    srcs = [
           'nonblocking_server.cc',
           ],
    deps = [
           '//base:base',
           '//network/public:tcp_server',
           ],
)

cc_library(
    name = 'bifrost_internal',
    srcs = [
           'bifrost.cc',
           ],
    deps = [
           '//base:base',
           '//network/public:tcp_server',
           ],
)

cc_library(
    name = 'request_parser_internal',
    srcs = [
           'request_parser.cc',
           ],
    deps = [
           '//base:base',
           '//network/public:net_buffer',
           '//util/bifrost/proto:bifrost_proto',
           ],
)
