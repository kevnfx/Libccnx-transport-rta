/*
 * Copyright (c) 2013-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Patent rights are not granted under this agreement. Patent rights are
 *       available under FRAND terms.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL XEROX or PARC BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
// Include the file(s) containing the functions to be tested.
// This permits internal static functions to be visible to this Test Runner.
#include "../ccnx_ConnectionConfig.c"

#include <LongBow/unit-test.h>

#include <parc/algol/parc_SafeMemory.h>
#include <parc/testing/parc_MemoryTesting.h>
#include <parc/testing/parc_ObjectTesting.h>

LONGBOW_TEST_RUNNER(ccnx_ConnectionConfig)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified here, but every test must be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
    LONGBOW_RUN_TEST_FIXTURE(Static);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(ccnx_ConnectionConfig)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(ccnx_ConnectionConfig)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_Add);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_AssertValid);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_Equals);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_Copy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_CreateDestroy);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_Display);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_GetJson);
    LONGBOW_RUN_TEST_CASE(Global, ccnxConnectionConfig_IsValid);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    parcSafeMemory_ReportAllocation(STDOUT_FILENO);

    if (!parcMemoryTesting_ExpectedOutstanding(0, "%s leaked memory.", longBowTestCase_GetFullName(testCase))) {
        return LONGBOW_STATUS_MEMORYLEAK;
    }

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_Add)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();

    PARCJSONValue *val = parcJSONValue_CreateFromNULL();
    ccnxConnectionConfig_Add(config, "key", val);
    parcJSONValue_Release(&val);

    ccnxConnectionConfig_Destroy(&config);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_AssertValid)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();
    ccnxConnectionConfig_AssertValid(config);
    ccnxConnectionConfig_Destroy(&config);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_Equals)
{
    CCNxConnectionConfig *x = ccnxConnectionConfig_Create();
    CCNxConnectionConfig *y = ccnxConnectionConfig_Create();
    CCNxConnectionConfig *z = ccnxConnectionConfig_Create();
    CCNxConnectionConfig *u1 = ccnxConnectionConfig_Create();
    PARCJSONValue *val = parcJSONValue_CreateFromNULL();
    ccnxConnectionConfig_Add(u1, "key", val);
    parcJSONValue_Release(&val);

    parcObjectTesting_AssertEqualsFunction(ccnxConnectionConfig_Equals, x, y, z, u1);

    ccnxConnectionConfig_Destroy(&x);
    ccnxConnectionConfig_Destroy(&y);
    ccnxConnectionConfig_Destroy(&z);
    ccnxConnectionConfig_Destroy(&u1);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_Copy)
{
    CCNxConnectionConfig *x = ccnxConnectionConfig_Create();
    PARCJSONValue *val = parcJSONValue_CreateFromNULL();
    ccnxConnectionConfig_Add(x, "key", val);
    parcJSONValue_Release(&val);

    CCNxConnectionConfig *y = ccnxConnectionConfig_Copy(x);
    assertTrue(ccnxConnectionConfig_Equals(x, y), "Expected the copy to be equal to the original");
    ccnxConnectionConfig_Destroy(&x);
    ccnxConnectionConfig_Destroy(&y);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_CreateDestroy)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();
    assertNotNull(config, "Expected non-NULL result from ccnxConnectionConfig_Create.");
    ccnxConnectionConfig_Destroy(&config);
    assertNull(config, "Expected NULL result from ccnxConnectionConfig_Destroy");
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_Display)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();
    ccnxConnectionConfig_Display(config, 0);

    ccnxConnectionConfig_Destroy(&config);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_GetJson)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();

    PARCJSON *json = ccnxConnectionConfig_GetJson(config);

    assertNotNull(json, "Expected ccnxConnectionConfig_GetJson result to be non-null.");
    ccnxConnectionConfig_Destroy(&config);
}

LONGBOW_TEST_CASE(Global, ccnxConnectionConfig_IsValid)
{
    CCNxConnectionConfig *config = ccnxConnectionConfig_Create();
    assertTrue(ccnxConnectionConfig_IsValid(config), "Expected ccnxConnectionConfig_Create result to be valid.");

    ccnxConnectionConfig_Destroy(&config);
    assertFalse(ccnxConnectionConfig_IsValid(config), "Expected ccnxConnectionConfig_Destroy result to be invalid.");
}

LONGBOW_TEST_FIXTURE(Static)
{
}

LONGBOW_TEST_FIXTURE_SETUP(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Static)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(ccnx_ConnectionConfig);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
