/* Copyright (C) 2014-2019 BMW Group
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <iostream>
#include "CMMethodCallsStub.hpp"
#include <thread>
#include <chrono>

namespace v1 {
namespace commonapi {
namespace communication {

CMMethodCallsStub::CMMethodCallsStub() {
}

CMMethodCallsStub::~CMMethodCallsStub() {
}

void CMMethodCallsStub::testMethod(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _x,
                                   testMethodReply_t _reply) {
    (void)_client;

    uint8_t y = _x;
    _reply(y);
}

void CMMethodCallsStub::testMethodTimeout(const std::shared_ptr<CommonAPI::ClientId> _client,
                                          testMethodReply_t _reply) {
    (void)_client;
    (void)_reply;
}

void CMMethodCallsStub::testDontCare(const std::shared_ptr<CommonAPI::ClientId> _client, uint8_t _x) {
	(void)_client;
	(void)_x;
	fireBTestEvent(1);
}

void CMMethodCallsStub::testMethodBlocking(
        const std::shared_ptr<CommonAPI::ClientId> _client, uint32_t _blockTime,
        testMethodBlockingReply_t _reply) {
    (void)_client;
    std::this_thread::sleep_for(std::chrono::milliseconds(_blockTime));
    _reply();
}

} /* namespace v1 */
} /* namespace communication */
} /* namespace commonapi */

