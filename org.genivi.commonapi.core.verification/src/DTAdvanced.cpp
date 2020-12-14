/* Copyright (C) 2014-2019 BMW Group
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/**
* @file DataTypes
*/

#include <functional>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <fstream>
#include <atomic>

#include <gtest/gtest.h>

#include "CommonAPI/CommonAPI.hpp"

#include "v1/commonapi/datatypes/advanced/TestInterfaceProxy.hpp"
#include "stub/DTAdvancedStub.hpp"

const std::string domain = "local";
const std::string testAddress = "commonapi.datatypes.advanced.TestInterface";
const std::string connectionIdService = "service-sample";
const std::string connectionIdClient = "client-sample";

const int tasync = 10000;

class Environment: public ::testing::Environment {
public:
    virtual ~Environment() {
    }

    virtual void SetUp() {
    }

    virtual void TearDown() {
    }
};

class DTAdvanced: public ::testing::Test {

public:
    void recvEnumValue(const CommonAPI::CallStatus& callStatus,
             ::v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration responseValue) {
        (void)responseValue;
        EXPECT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    }
    void recvInvalidEnumValue(const CommonAPI::CallStatus& callStatus,
             ::v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration responseValue) {
        (void)responseValue;
        EXPECT_EQ(callStatus, CommonAPI::CallStatus::INVALID_VALUE);
    }


protected:
    void SetUp() {
        runtime_ = CommonAPI::Runtime::get();
        ASSERT_TRUE((bool)runtime_);

        testStub_ = std::make_shared<v1_0::commonapi::datatypes::advanced::DTAdvancedStub>();
        serviceRegistered_ = runtime_->registerService(domain, testAddress, testStub_, connectionIdService);
        ASSERT_TRUE(serviceRegistered_);

        testProxy_ = runtime_->buildProxy<v1_0::commonapi::datatypes::advanced::TestInterfaceProxy>(domain, testAddress, connectionIdClient);
        int i = 0;
        while(!testProxy_->isAvailable() && i++ < 100) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        ASSERT_TRUE(testProxy_->isAvailable());
    }

    void TearDown() {
        ASSERT_TRUE(runtime_->unregisterService(domain, v1_0::commonapi::datatypes::advanced::DTAdvancedStub::StubInterface::getInterface(), testAddress));
        
        // wait that proxy is not available
        int counter = 0;  // counter for avoiding endless loop
        while ( testProxy_->isAvailable() && counter < 100 ) {
            std::this_thread::sleep_for(std::chrono::microseconds(tasync));
            counter++;
        }

        ASSERT_FALSE(testProxy_->isAvailable());
    }

    std::atomic<bool> received_;
    bool serviceRegistered_;
    std::shared_ptr<CommonAPI::Runtime> runtime_;

    std::shared_ptr<v1_0::commonapi::datatypes::advanced::TestInterfaceProxy<>> testProxy_;
    std::shared_ptr<v1_0::commonapi::datatypes::advanced::DTAdvancedStub> testStub_;

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefTestValue;

    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayIn;
};

/*
* @test Test function call with advanced types
*   - Advanced types are: arrays, enumerations, structs, unions, maps, typedefs
*   - Function call of a function that has for each advanced type one argument (test values) and one return value
*   - The stub copies the test values to the return values
*   - On client side the test values are compared with the return values
*/
TEST_F(DTAdvanced, SendAndReceive) {

    CommonAPI::CallStatus callStatus;

    arrayTestValue.push_back("Test1");
    arrayTestValue.push_back("Test2");
    arrayTestValue.push_back("Test3");

    enumerationTestValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2;

    structTestValue.setBooleanMember(true);
    structTestValue.setUint8Member(42);
    structTestValue.setStringMember("Hello World");

    uint8_t u = 53;
    unionTestValue = u;

    mapTestValue[1] = "Hello";
    mapTestValue[2] = "World";

    typedefTestValue = 64;

    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayIn;
    enumArrayIn.push_back(enumerationTestValue);

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefResultValue;
    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayOut;

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );

    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(arrayTestValue, arrayResultValue);
    EXPECT_EQ(enumerationTestValue, enumerationResultValue);
    EXPECT_EQ(structTestValue, structResultValue);
    EXPECT_EQ(unionTestValue, unionResultValue);
    EXPECT_EQ(mapTestValue, mapResultValue);
    EXPECT_EQ(typedefTestValue, typedefResultValue);
    EXPECT_EQ(enumArrayIn, enumArrayOut);
}

/*
* @test Test function call with an invalid type
* Try to pass an invalid value. Check that it failed.
*/
TEST_F(DTAdvanced, SendAndReceiveInvalid) {

    CommonAPI::CallStatus callStatus;

    arrayTestValue.push_back("Test1");
    arrayTestValue.push_back("Test2");
    arrayTestValue.push_back("Test3");

    // put a deliberately invalid value to the enum data value
    enumerationTestValue = static_cast<const v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::Literal>(124);

    structTestValue.setBooleanMember(true);
    structTestValue.setUint8Member(42);
    structTestValue.setStringMember("Hello World");

    uint8_t u = 53;
    unionTestValue = u;

    mapTestValue[1] = "Hello";
    mapTestValue[2] = "World";

    typedefTestValue = 64;

    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayIn;
    enumArrayIn.push_back(v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2);

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefResultValue;
    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayOut;

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );

    ASSERT_EQ(callStatus, CommonAPI::CallStatus::INVALID_VALUE);
}

/*
* @test Test function call with advanced types
*   - Add more elements to a map than deployment allows -> Not succeed
*   - Add less elements to a map than deployment allows -> Not succeed
*/
TEST_F(DTAdvanced, DISABLED_SendAndReceiveMapInvalid) {

    CommonAPI::CallStatus callStatus;

    arrayTestValue.push_back("Test1");
    arrayTestValue.push_back("Test2");
    arrayTestValue.push_back("Test3");

    enumerationTestValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2;

    structTestValue.setBooleanMember(true);
    structTestValue.setUint8Member(42);
    structTestValue.setStringMember("Hello World");

    uint8_t u = 53;
    unionTestValue = u;

    mapTestValue[1] = "Hello";
    mapTestValue[2] = "World";
    mapTestValue[3] = "World";
    mapTestValue[4] = "World";
    mapTestValue[5] = "World";

    typedefTestValue = 64;

    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayIn;
    enumArrayIn.push_back(enumerationTestValue);

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefResultValue;
    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayOut;

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::OUT_OF_MEMORY);

    mapTestValue.clear();
    mapTestValue[1] = "Hello";

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::OUT_OF_MEMORY);

}

/**
* @test Test attribute functions with invalid values
*   - Call set function of attributes with invalid types
*   - Check that the attribute's value has not changed
*/
TEST_F(DTAdvanced, AttributeSetInvalid) {

    CommonAPI::CallStatus callStatus;

    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationTestValue;

    // put a deliberately invalid value to the enum data value
    enumerationTestValue = static_cast<const v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::Literal>(124);

    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;

    testProxy_->getAEnumerationAttribute().setValue(enumerationTestValue, callStatus, enumerationResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::INVALID_VALUE);
}

/**
* @test Test attribute functions with invalid map length
*   - Call set function of attributes with map length
*   - Check that an error returns
*/
TEST_F(DTAdvanced, DISABLED_AttributeSetInvalidMapLength) {

    CommonAPI::CallStatus callStatus;

    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapTestValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapTestResonseValue;

    mapTestValue.clear();
    mapTestValue[1] = "Hello";

    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;

    testProxy_->getAMapAttribute().setValue(mapTestValue, callStatus, mapTestResonseValue);

    ASSERT_EQ(callStatus, CommonAPI::CallStatus::OUT_OF_MEMORY);
}

/**
* @test Test attribute asynchronous functions with invalid values
*   - Call set asynch function of attributes with invalid types
*   - Callback should be called with error status
*   - Check that attribute value has not changed
*/
TEST_F(DTAdvanced, AttributeSetAsyncInvalid) {

    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationTestValue;

    std::function<void (const CommonAPI::CallStatus&,
        v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration)> myCallback =
            std::bind(&DTAdvanced::recvInvalidEnumValue, this, std::placeholders::_1, std::placeholders::_2);

    // put a deliberately invalid value to the enum data value
    enumerationTestValue = static_cast<const v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::Literal>(124);

    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;

    testProxy_->getAEnumerationAttribute().setValueAsync(enumerationTestValue, myCallback);
    std::this_thread::sleep_for(std::chrono::microseconds(tasync));
}


/**
* @test Test attribute functions with advanced types
*   - Call set function of attributes with advanced types
*   - Call get function and check if the return value is the same
*/
TEST_F(DTAdvanced, AttributeSet) {

    CommonAPI::CallStatus callStatus;

    arrayTestValue.push_back("Test1");
    arrayTestValue.push_back("Test2");
    arrayTestValue.push_back("Test3");

    enumerationTestValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2;

    structTestValue.setBooleanMember(true);
    structTestValue.setUint8Member(42);
    structTestValue.setStringMember("Hello World");

    uint8_t u = 53;
    unionTestValue = u;

    mapTestValue[1] = "Hello";
    mapTestValue[2] = "World";

    typedefTestValue = 64;

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefResultValue;

    testProxy_->getAArrayAttribute().setValue(arrayTestValue, callStatus, arrayResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(arrayTestValue, arrayResultValue);

    // check initial value of enumeration attribute
    enumerationTestValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE1; // this is the expected default value
    EXPECT_EQ(enumerationTestValue, enumerationResultValue); // the uninitialized enumerationResultValue should have the default value
    enumerationResultValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2; // set to some other value
    testProxy_->getAEnumerationAttribute().getValue(callStatus, enumerationResultValue); // get value of attribute
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(enumerationTestValue, enumerationResultValue); // attribute value should default to the initial default value

    testProxy_->getAEnumerationAttribute().setValue(enumerationTestValue, callStatus, enumerationResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(enumerationTestValue, enumerationResultValue);

    testProxy_->getAStructAttribute().setValue(structTestValue, callStatus, structResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(structTestValue, structResultValue);

    testProxy_->getAUnionAttribute().setValue(unionTestValue, callStatus, unionResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(unionTestValue, unionResultValue);

    testProxy_->getAMapAttribute().setValue(mapTestValue, callStatus, mapResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(mapTestValue, mapResultValue);

    testProxy_->getATypedefAttribute().setValue(typedefTestValue, callStatus, typedefResultValue);
    ASSERT_EQ(callStatus, CommonAPI::CallStatus::SUCCESS);
    EXPECT_EQ(typedefTestValue, typedefResultValue);
}

/**
* @test Test broadcast with advanced types
*   - Subscribe to broadcast which contains advanced types
*   - Call function to cause the stub to fire broadcast event with the same content
*   - Check if the values in the callback function are as expected
*/
TEST_F(DTAdvanced, BroadcastReceive) {

    std::mutex m;
    CommonAPI::CallStatus callStatus;
    std::atomic<CommonAPI::CallStatus> subStatus(CommonAPI::CallStatus::UNKNOWN);

    {
        std::lock_guard<std::mutex> itsLock(m);

        arrayTestValue.push_back("Test1");
        arrayTestValue.push_back("Test2");
        arrayTestValue.push_back("Test3");

        enumerationTestValue = v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration::VALUE2;

        structTestValue.setBooleanMember(true);
        structTestValue.setUint8Member(42);
        structTestValue.setStringMember("Hello World");

        uint8_t u = 53;
        unionTestValue = u;

        mapTestValue[1] = "Hello";
        mapTestValue[2] = "World";

        typedefTestValue = 64;

        enumArrayIn.push_back(enumerationTestValue);
    }

    v1_0::commonapi::datatypes::advanced::TestInterface::tArray arrayResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration enumerationResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tStruct structResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tUnion unionResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tMap mapResultValue;
    v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef typedefResultValue;
    std::vector<v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration> enumArrayOut;

    received_ = false;
    testProxy_->getBTestEvent().subscribe([&](
            const v1_0::commonapi::datatypes::advanced::TestInterface::tArray& arrayResultValue,
            const v1_0::commonapi::datatypes::advanced::TestInterface::tEnumeration& enumerationResultValue,
            const v1_0::commonapi::datatypes::advanced::TestInterface::tStruct& structResultValue,
            const v1_0::commonapi::datatypes::advanced::TestInterface::tUnion& unionResultValue,
            const v1_0::commonapi::datatypes::advanced::TestInterface::tMap& mapResultValue,
            const v1_0::commonapi::datatypes::advanced::TestInterface::tTypedef& typedefResultValue
            ) {
        std::lock_guard<std::mutex> itsLock(m);
        EXPECT_EQ(arrayTestValue, arrayResultValue);
        EXPECT_EQ(enumerationTestValue, enumerationResultValue);
        EXPECT_EQ(structTestValue, structResultValue);
        EXPECT_EQ(unionTestValue, unionResultValue);
        EXPECT_EQ(mapTestValue, mapResultValue);
        EXPECT_EQ(typedefTestValue, typedefResultValue);
        received_ = true;
    },
    [&](
        const CommonAPI::CallStatus &status
    ) {
        subStatus = status;
    });

    // check that subscription has succeeded
    for (int i = 0; i < 100; i++) {
        if (subStatus == CommonAPI::CallStatus::SUCCESS) break;
        std::this_thread::sleep_for(std::chrono::microseconds(tasync));
    }
    EXPECT_EQ(CommonAPI::CallStatus::SUCCESS, subStatus);

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );

    for (int i = 0; i < 100; ++i) {
        if (received_) break;
        std::this_thread::sleep_for(std::chrono::microseconds(tasync));
    }
    ASSERT_TRUE(received_);
    received_ = false;

    {
        std::lock_guard<std::mutex> itsLock(m);
        mapTestValue.clear();
        mapTestValue[4] = "Test";
        mapTestValue[5] = "123";
    }

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );

    for (int i = 0; i < 100; ++i) {
        if (received_) break;
        std::this_thread::sleep_for(std::chrono::microseconds(tasync));
    }
    ASSERT_TRUE(received_);
    received_ = false;

    {
        std::lock_guard<std::mutex> itsLock(m);
        mapTestValue.clear();
        mapTestValue[11] = "blub";
        mapTestValue[22] = "blah";
    }

    testProxy_->fTest(
            arrayTestValue,
            enumerationTestValue,
            structTestValue,
            unionTestValue,
            mapTestValue,
            typedefTestValue,
            enumArrayIn,
            callStatus,
            arrayResultValue,
            enumerationResultValue,
            structResultValue,
            unionResultValue,
            mapResultValue,
            typedefResultValue,
            enumArrayOut
    );

    for (int i = 0; i < 100; ++i) {
        if (received_) break;
        std::this_thread::sleep_for(std::chrono::microseconds(tasync));
    }
    ASSERT_TRUE(received_);
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    ::testing::AddGlobalTestEnvironment(new Environment());
    return RUN_ALL_TESTS();
}
