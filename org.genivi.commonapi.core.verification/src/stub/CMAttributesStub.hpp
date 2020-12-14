/* Copyright (C) 2014-2019 BMW Group
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef CMATTRIBUTESSTUB_HPP_
#define CMATTRIBUTESSTUB_HPP_

#include "v1/commonapi/communication/TestInterfaceStubDefault.hpp"

namespace v1 {
namespace commonapi {
namespace communication {

class CMAttributesStub : public v1_0::commonapi::communication::TestInterfaceStubDefault {
public:
    CMAttributesStub();
    virtual ~CMAttributesStub();
    virtual void setTestValues(uint8_t);
};

} /* namespace v1 */
} /* namespace communication */
} /* namespace commonapi */

#endif /* CMATTRIBUTESSTUB_HPP_ */
