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
#include "../rta_CommandTransmitStatistics.c"

#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>

#include <sys/time.h>

#define MAX_FILENAME 1024

typedef struct test_data {
    struct timeval period;
    char filename[MAX_FILENAME];

    RtaCommandTransmitStatistics *transmitStats;
} TestData;

static TestData *
_createTestData(void)
{
    TestData *data = parcMemory_AllocateAndClear(sizeof(TestData));
    assertNotNull(data, "parcMemory_AllocateAndClear(%zu) returned NULL", sizeof(TestData));

    data->period = (struct timeval) { .tv_sec = 1234, .tv_usec = 2389484 };
    snprintf(data->filename, MAX_FILENAME, "Miss Piggy");

    data->transmitStats = rtaCommandTransmitStatistics_Create(data->period, data->filename);

    return data;
}

static void
_destroyTestData(TestData **dataPtr)
{
    TestData *data = *dataPtr;
    rtaCommandTransmitStatistics_Release(&data->transmitStats);
    parcMemory_Deallocate((void **) &data);
    *dataPtr = NULL;
}

// =============================================================
LONGBOW_TEST_RUNNER(rta_CommandTransmitStatistics)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(rta_CommandTransmitStatistics)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(rta_CommandTransmitStatistics)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, rtaCommandTransmitStatistics_Acquire);
    LONGBOW_RUN_TEST_CASE(Global, rtaCommandTransmitStatistics_Create);
    LONGBOW_RUN_TEST_CASE(Global, rtaCommandTransmitStatistics_GetFilename);
    LONGBOW_RUN_TEST_CASE(Global, rtaCommandTransmitStatistics_GetPeriod);
    LONGBOW_RUN_TEST_CASE(Global, rtaCommandTransmitStatistics_Release);
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

LONGBOW_TEST_CASE(Global, rtaCommandTransmitStatistics_Acquire)
{
    TestData *data = _createTestData();

    size_t firstRefCount = parcObject_GetReferenceCount(data->transmitStats);

    RtaCommandTransmitStatistics *second = rtaCommandTransmitStatistics_Acquire(data->transmitStats);
    size_t secondRefCount = parcObject_GetReferenceCount(second);

    assertTrue(secondRefCount == firstRefCount + 1, "Wrong refcount after acquire, got %zu expected %zu", secondRefCount, firstRefCount + 1);

    rtaCommandTransmitStatistics_Release(&second);
    _destroyTestData(&data);
}

LONGBOW_TEST_CASE(Global, rtaCommandTransmitStatistics_Create)
{
    TestData *data = _createTestData();
    assertNotNull(data->transmitStats, "Got null from create");

    assertTrue(timercmp(&data->period, &data->transmitStats->period, ==), "Period values not equal");
    assertTrue(strcmp(data->filename, data->transmitStats->filename) == 0, "Filenames not equal");
    _destroyTestData(&data);
}

LONGBOW_TEST_CASE(Global, rtaCommandTransmitStatistics_GetFilename)
{
    TestData *data = _createTestData();

    const char *testFilename = rtaCommandTransmitStatistics_GetFilename(data->transmitStats);
    assertTrue(strcmp(data->filename, testFilename) == 0, "Filenames not equal");

    _destroyTestData(&data);
}

LONGBOW_TEST_CASE(Global, rtaCommandTransmitStatistics_GetPeriod)
{
    TestData *data = _createTestData();

    struct timeval testPeriod = rtaCommandTransmitStatistics_GetPeriod(data->transmitStats);
    assertTrue(timercmp(&data->period, &testPeriod, ==), "Period values not equal");

    _destroyTestData(&data);
}

LONGBOW_TEST_CASE(Global, rtaCommandTransmitStatistics_Release)
{
    TestData *data = _createTestData();

    RtaCommandTransmitStatistics *second = rtaCommandTransmitStatistics_Acquire(data->transmitStats);
    size_t secondRefCount = parcObject_GetReferenceCount(second);

    rtaCommandTransmitStatistics_Release(&second);
    size_t thirdRefCount = parcObject_GetReferenceCount(data->transmitStats);

    assertTrue(thirdRefCount == secondRefCount - 1, "Wrong refcount after release, got %zu expected %zu", thirdRefCount, secondRefCount - 1);

    _destroyTestData(&data);
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(rta_CommandTransmitStatistics);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
