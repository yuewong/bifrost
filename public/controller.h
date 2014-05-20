// Copyright 2014 Makepolo Inc. All Rights Reserved.
// Author: huangyue@makepolo.com (Yue Huang)

#ifndef UTIL_BIFROST_PUBLIC_CONTROLLER_H_
#define UTIL_BIFROST_PUBLIC_CONTROLLER_H_

// An RpcController mediates a single method call.  The primary purpose of
// the controller is to provide a way to manipulate settings specific to the
// RPC implementation and to find out about RPC-level errors.
//
// The methods provided by the RpcController interface are intended to be a
// "least common denominator" set of features which we expect all
// implementations to support.  Specific implementations may provide more
// advanced features (e.g. deadline propagation).

#include <string>

#include "base/basictypes.h"
#include "thirdparty/protobuf/include/google/protobuf/service.h"

namespace util {
namespace bifrost {
class Controller : public google::protobuf::RpcController {
 public:
  inline Controller() : cancel_(false), failed_(false), cancel_callback_(nullptr) {}
  virtual ~Controller();

  // Client-side methods ---------------------------------------------
  // These calls may be made from the client side only.  Their results
  // are undefined on the server side (may crash).

  // Resets the RpcController to its initial state so that it may be reused in
  // a new call.  Must not be called while an RPC is in progress.
  virtual void Reset() {
    reason_.clear();
    failed_ = cancel_ = false;
  }

  // After a call has finished, returns true if the call failed.  The possible
  // reasons for failure depend on the RPC implementation.  Failed() must not
  // be called before a call has finished.  If Failed() returns true, the
  // contents of the response message are undefined.
  virtual bool Failed() const {
    return failed_;
  }

  // If Failed() is true, returns a human-readable description of the error.
  virtual std::string ErrorText() const {
    return reason_;
  }

  // Advises the RPC system that the caller desires that the RPC call be
  // canceled.  The RPC system may cancel it immediately, may wait awhile and
  // then cancel it, or may not even cancel the call at all.  If the call is
  // canceled, the "done" callback will still be called and the RpcController
  // will indicate that the call failed at that time.
  virtual void StartCancel() {
    if (!cancel_ && cancel_callback_) {
      cancel_callback_->Run();
      cancel_ = true;
    }
  }

  // Server-side methods ---------------------------------------------
  // These calls may be made from the server side only.  Their results
  // are undefined on the client side (may crash).

  // Causes Failed() to return true on the client side.  "reason" will be
  // incorporated into the message returned by ErrorText().  If you find
  // you need to return machine-readable information about failures, you
  // should incorporate it into your response protocol buffer and should
  // NOT call SetFailed().
  virtual void SetFailed(const std::string& reason) {
    reason_ = reason;
    failed_ = true;
  }

  // If true, indicates that the client canceled the RPC, so the server may
  // as well give up on replying to it.  The server should still call the
  // final "done" callback.
  virtual bool IsCanceled() const {
    return cancel_;
  }

  // Asks that the given callback be called when the RPC is canceled.  The
  // callback will always be called exactly once.  If the RPC completes without
  // being canceled, the callback will be called after completion.  If the RPC
  // has already been canceled when NotifyOnCancel() is called, the callback
  // will be called immediately.
  //
  // NotifyOnCancel() must be called no more than once per request.
  virtual void NotifyOnCancel(google::protobuf::Closure* callback) {
    if (cancel_) {
      callback->Run();
    } else {
      cancel_callback_ = callback;
    }
  }

 private:
  std::string reason_;
  bool cancel_;
  bool failed_;

  google::protobuf::Closure* cancel_callback_;

 private:
  DISALLOW_COPY_AND_ASSIGN(Controller);
};
}  // namespace bifrost
}  // namespace util
#endif  // UTIL_BIFROST_PUBLIC_CONTROLLER_H_
