/*
 * Copyright (c) 2014-2015, Xerox Corporation (Xerox)and Palo Alto Research Center (PARC)
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
// This permits internal static functions to be visible to this Test Framework.
#include "../controlPlaneInterface.c"

#include <LongBow/unit-test.h>

#include <ccnx/api/control/cpi_ControlFacade.h>

#include <parc/algol/parc_Memory.h>
#include <parc/algol/parc_SafeMemory.h>

LONGBOW_TEST_RUNNER(controlPlaneInterface)
{
    parcMemory_SetInterface(&PARCSafeMemoryAsPARCMemory);

    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(controlPlaneInterface)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(controlPlaneInterface)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, cpi_CreateRequest);
    LONGBOW_RUN_TEST_CASE(Global, cpi_CreateResponse);
    LONGBOW_RUN_TEST_CASE(Global, cpi_ParseRequest);

    LONGBOW_RUN_TEST_CASE(Global, cpi_PauseInput);

    LONGBOW_RUN_TEST_CASE(Global, controlPlaneInterface_PauseInput);
}

LONGBOW_TEST_FIXTURE_SETUP(Global)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE_TEARDOWN(Global)
{
    uint32_t outstandingAllocations = parcSafeMemory_ReportAllocation(STDERR_FILENO);
    if (outstandingAllocations != 0) {
        printf("%s leaks memory by %d allocations\n", longBowTestCase_GetName(testCase), outstandingAllocations);
        return LONGBOW_STATUS_MEMORYLEAK;
    }

    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_CASE(Global, cpi_CreateRequest)
{
    const char key_looney[] = "looney";

    PARCJSON *operation = parcJSON_Create();
    parcJSON_AddString(operation, "bugs", "bunny");

    PARCJSON *request = cpi_CreateRequest(key_looney, operation);
    parcJSON_Release(&operation);

    PARCJSONValue *value = parcJSON_GetValueByName(request, cpiRequest);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiRequest,
                  parcJSON_ToString(request));
    assertTrue(parcJSONValue_IsJSON(value),
               "Key %s wrong type: %s",
               cpiRequest,
               parcJSON_ToString(request));
    PARCJSON *test_request_key = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(test_request_key, cpiSeqnum);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiSeqnum,
                  parcJSON_ToString(request));
    assertTrue(parcJSONValue_IsNumber(value),
               "Key %s wrong type: %s",
               cpiSeqnum,
               parcJSON_ToString(request));

    value = parcJSON_GetValueByName(test_request_key, key_looney);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  key_looney,
                  parcJSON_ToString(request));
    assertTrue(parcJSONValue_IsJSON(value),
               "Key %s wrong type: %s",
               key_looney,
               parcJSON_ToString(request));

    parcJSON_Release(&request);
}

LONGBOW_TEST_CASE(Global, cpi_CreateResponse)
{
    const char key_looney[] = "looney";

    PARCJSON *requestOperation = parcJSON_Create();
    parcJSON_AddString(requestOperation, "bugs", "bunny");

    PARCJSON *requestJson = cpi_CreateRequest(key_looney, requestOperation);
    parcJSON_Release(&requestOperation);

    //
    // we should now have an object that looks, like
    // { "CPI_REQUEST": { "SEQUENCE":1, "looney":{"bugs":"bunny"} } }
    //
    // The sequence number might not be exactly 1, depending on how the
    // unit tests are ordered and forked.
    //
    char *expectedStr1 = "{\"CPI_REQUEST\":{\"SEQUENCE\":";
    char *expectedStr2 = ",\"looney\":{\"bugs\":\"bunny\"}}}";
    char *jsonStr = parcJSON_ToCompactString(requestJson);
    size_t len = strlen(expectedStr1);
    assertTrue(strncmp(jsonStr, expectedStr1, len) == 0, "Expect JSON strings to be the same");
    assertTrue(strncmp(jsonStr + len + 1, expectedStr2, strlen(expectedStr2)) == 0, "Expect JSON strings to be the same");
    parcMemory_Deallocate(&jsonStr);

    CCNxControl *request = ccnxControl_CreateCPIRequest(requestJson);

    PARCJSON *responseOperation = parcJSON_Create();
    parcJSON_AddString(responseOperation, "donald", "duck");

    CCNxControl *response = cpi_CreateResponse(request, responseOperation);
    parcJSON_Release(&responseOperation);

    PARCJSON *responseJson = ccnxControl_GetJson(response);
    //
    // we should now have an object that looks, like
    // { "CPI_RESPONSE": { "SEQUENCE":1, "looney":{"donald":"duck"} } }
    //
    // The sequence number might not be exactly 1, depending on how the
    // unit tests are ordered and forked.  It should be the same as the request.
    //
    expectedStr1 = "{\"CPI_RESPONSE\":{\"SEQUENCE\":";
    expectedStr2 = ",\"looney\":{\"donald\":\"duck\"}}}";
    jsonStr = parcJSON_ToCompactString(responseJson);
    len = strlen(expectedStr1);
    assertTrue(strncmp(jsonStr, expectedStr1, len) == 0, "Expect JSON strings to be the same");
    assertTrue(strncmp(jsonStr + len + 1, expectedStr2, strlen(expectedStr2)) == 0, "Expect JSON strings to be the same");
    parcMemory_Deallocate(&jsonStr);

    PARCJSONValue *value = parcJSON_GetValueByName(responseJson, cpiResponse);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiResponse,
                  parcJSON_ToString(responseJson));
    assertTrue(parcJSONValue_IsJSON(value),
               "Key %s wrong type: %s",
               cpiResponse,
               parcJSON_ToString(responseJson));
    PARCJSON *test_response_key = parcJSONValue_GetJSON(value);

    value = parcJSON_GetValueByName(test_response_key, cpiSeqnum);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  cpiSeqnum,
                  parcJSON_ToString(test_response_key));
    assertTrue(parcJSONValue_IsNumber(value),
               "Key %s wrong type: %s",
               cpiSeqnum,
               parcJSON_ToString(responseJson));

    value = parcJSON_GetValueByName(test_response_key, key_looney);
    assertNotNull(value,
                  "JSON key not found %s: %s",
                  key_looney,
                  parcJSON_ToString(responseJson));
    assertTrue(parcJSONValue_IsJSON(value),
               "Key %s wrong type: %s",
               key_looney,
               parcJSON_ToString(responseJson));

    ccnxControl_Release(&response);
    ccnxControl_Release(&request);

    parcJSON_Release(&requestJson);
}

LONGBOW_TEST_CASE(Global, cpi_ParseRequest)
{
    char key_looney[] = "looney";
    const char value_looney[] = "{\"bugs\":\"bunny\"}";

    PARCJSON *operation = parcJSON_Create();
    parcJSON_AddString(operation, "bugs", "bunny");

    PARCJSON *request = cpi_CreateRequest(key_looney, operation);
    parcJSON_Release(&operation);

    //
    // we should now have an object that looks, like
    // { "CPI_REQUEST": { "SEQUENCE":1, "looney":{"bugs":"bunny"} } }
    //
    // The sequence number might not be exactly 1, depending on how the
    // unit tests are ordered and forked.
    //

    PARCJSONPair *parsedOpPair = cpi_ParseRequest(request);
    assertNotNull(parsedOpPair, "Got null parsed json from %s", parcJSON_ToString(request));

    PARCBuffer *key = parcJSONPair_GetName(parsedOpPair);
    PARCBuffer *test_key = parcBuffer_WrapCString(key_looney);
    assertTrue(parcBuffer_Equals(key, test_key),
               "Key name of parsed wrong, expected %s got %s in %s",
               key_looney,
               parcBuffer_ToString(key),
               parcJSONPair_ToString(parsedOpPair));
    parcBuffer_Release(&test_key);

    operation = parcJSONValue_GetJSON(parcJSONPair_GetValue(parsedOpPair));
    char *test_looney = parcJSON_ToCompactString(operation);
    assertTrue(strcmp(value_looney, test_looney) == 0,
               "Inner values did not match, expected %s got %s in %s",
               value_looney,
               test_looney,
               parcJSONPair_ToString(parsedOpPair));
    parcMemory_Deallocate((void **) &test_looney);

    parcJSON_Release(&request);
}

LONGBOW_TEST_CASE(Global, controlPlaneInterface_PauseInput)
{
    CCNxControl *message = ccnxControl_CreatePauseInputRequest();

    assertTrue(ccnxControlFacade_IsCPI(message), "Expected ccnxControlMessage_IsCPI to be true");

    PARCJSON *json = ccnxControlFacade_GetJson(message);
    assertTrue(ccnxControl_IsCPI(message), "Expected a CPI control message");
    assertTrue(cpi_getCPIOperation2(json) == CPI_PAUSE,
               "Expected opertaion %d got %d", CPI_PAUSE, cpi_getCPIOperation2(json));

    ccnxControl_Release(&message);
}

LONGBOW_TEST_CASE(Global, cpi_PauseInput)
{
    PARCJSON *pauseRequest = cpi_CreatePauseInputRequest();
    CCNxControl *request = ccnxControl_CreateCPIRequest(pauseRequest);
    parcJSON_Release(&pauseRequest);

    assertTrue(ccnxControl_IsCPI(request), "Is not a CPI message!");
    assertTrue(cpi_GetMessageType(request) == CPI_REQUEST,
               "Got wrong message type, expected %d got %d",
               CPI_REQUEST,
               cpi_GetMessageType(request));

    assertTrue(cpi_GetMessageOperation(request) == CPI_PAUSE,
               "got wrong operation, expected %d got %d",
               CPI_PAUSE,
               cpi_GetMessageOperation(request));

    ccnxControl_Release(&request);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(controlPlaneInterface);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
