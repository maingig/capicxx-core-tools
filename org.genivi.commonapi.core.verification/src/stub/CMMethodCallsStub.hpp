/* Copyright (C) 2014-2019 BMW Group
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMMETHODCALLSSTUB_HPP_
#define CMMETHODCALLSSTUB_HPP_

#include "v1/commonapi/communication/TestInterfaceStubDefault.hpp"

namespace v1 {
namespace commonapi {
namespace communication {

class CMMethodCallsStub : public v1_0::commonapi::communication::TestInterfaceStubDefault {
public:
    CMMethodCallsStub();
    virtual ~CMMethodCallsStub();

    void testMethod(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _x, testMethodReply_t _reply);

    void testMethodTimeout(const std::shared_ptr<CommonAPI::ClientId> _client, testMethodReply_t _reply);

    void testDontCare(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _x);

    void testMethodBlocking(const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _blockTime, testMethodBlockingReply_t _reply);
};

} /* namespace v1 */
} /* namespace communication */
} /* namespace commonapi */

#endif /* CMMETHODCALLSSTUB_H_ */
