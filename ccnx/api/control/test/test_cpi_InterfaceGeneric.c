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

#include "../cpi_InterfaceGeneric.c"
#include <LongBow/unit-test.h>
#include <parc/algol/parc_SafeMemory.h>


LONGBOW_TEST_RUNNER(cpi_InterfaceGeneric)
{
    // The following Test Fixtures will run their corresponding Test Cases.
    // Test Fixtures are run in the order specified, but all tests should be idempotent.
    // Never rely on the execution order of tests or share state between them.
    LONGBOW_RUN_TEST_FIXTURE(Global);
}

// The Test Runner calls this function once before any Test Fixtures are run.
LONGBOW_TEST_RUNNER_SETUP(cpi_InterfaceGeneric)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

// The Test Runner calls this function once after all the Test Fixtures are run.
LONGBOW_TEST_RUNNER_TEARDOWN(cpi_InterfaceGeneric)
{
    return LONGBOW_STATUS_SUCCEEDED;
}

LONGBOW_TEST_FIXTURE(Global)
{
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_Copy);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_Create_Destroy);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_GetAddresses);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_GetIndex);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_GetState);
    LONGBOW_RUN_TEST_CASE(Global, cpiInterfaceGeneric_BuildString);
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

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_Copy)
{
    CPIAddress *addr = cpiAddress_CreateFromInterface(5);
    CPIAddressList *list = cpiAddressList_Append(cpiAddressList_Create(), addr);
    CPIInterfaceGeneric *Generic = cpiInterfaceGeneric_Create(1, list);

    CPIInterfaceGeneric *copy = cpiInterfaceGeneric_Copy(Generic);

    assertTrue(copy->ifidx == Generic->ifidx, "ifidx did not match, expected %u got %u", Generic->ifidx, copy->ifidx);
    assertTrue(copy->state == Generic->state, "states did not match, expected %d got %d", Generic->state, copy->state);
    assertTrue(cpiAddressList_Equals(copy->addresses, Generic->addresses), "did not get same addresses");

    cpiInterfaceGeneric_Destroy(&copy);
    cpiInterfaceGeneric_Destroy(&Generic);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_Create_Destroy)
{
    CPIInterfaceGeneric *Generic = cpiInterfaceGeneric_Create(1, cpiAddressList_Create());
    cpiInterfaceGeneric_Destroy(&Generic);

    assertTrue(parcMemory_Outstanding() == 0, "Imbalance after destroying");
}

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_GetAddresses)
{
    CPIAddress *addr = cpiAddress_CreateFromInterface(5);
    CPIAddressList *list = cpiAddressList_Append(cpiAddressList_Create(), addr);
    CPIInterfaceGeneric *Generic = cpiInterfaceGeneric_Create(1, list);

    const CPIAddressList *test = cpiInterfaceGeneric_GetAddresses(Generic);
    assertTrue(cpiAddressList_Equals(list, test), "Address lists did not match");

    cpiInterfaceGeneric_Destroy(&Generic);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_GetIndex)
{
    CPIAddress *addr = cpiAddress_CreateFromInterface(5);
    CPIAddressList *list = cpiAddressList_Append(cpiAddressList_Create(), addr);
    CPIInterfaceGeneric *Generic = cpiInterfaceGeneric_Create(1, list);

    assertTrue(cpiInterfaceGeneric_GetIndex(Generic) == 1, "ifidx did not match");

    cpiInterfaceGeneric_Destroy(&Generic);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_GetState)
{
    CPIAddress *addr = cpiAddress_CreateFromInterface(5);
    CPIAddressList *list = cpiAddressList_Append(cpiAddressList_Create(), addr);
    CPIInterfaceGeneric *Generic = cpiInterfaceGeneric_Create(1, list);

    assertTrue(cpiInterfaceGeneric_GetState(Generic) == CPI_IFACE_UNKNOWN, "state did not match");

    cpiInterfaceGeneric_SetState(Generic, CPI_IFACE_UP);
    assertTrue(cpiInterfaceGeneric_GetState(Generic) == CPI_IFACE_UP, "state did not match");

    cpiInterfaceGeneric_SetState(Generic, CPI_IFACE_DOWN);
    assertTrue(cpiInterfaceGeneric_GetState(Generic) == CPI_IFACE_DOWN, "state did not match");

    cpiInterfaceGeneric_Destroy(&Generic);
}

LONGBOW_TEST_CASE(Global, cpiInterfaceGeneric_BuildString)
{
    CPIAddressList *addrs = cpiAddressList_Create();
    cpiAddressList_Append(addrs, cpiAddress_CreateFromInterface(1));
    cpiAddressList_Append(addrs, cpiAddress_CreateFromInterface(2));

    CPIInterfaceGeneric *generic = cpiInterfaceGeneric_Create(1, addrs);

    uint32_t beforeBalance = parcMemory_Outstanding();
    PARCBufferComposer *composer = cpiInterfaceGeneric_BuildString(generic, parcBufferComposer_Create());
    parcBufferComposer_Release(&composer);
    uint32_t afterBalance = parcMemory_Outstanding();

    cpiInterfaceGeneric_Destroy(&generic);

    assertTrue(beforeBalance == afterBalance, "Memory leak in BuildString: %d", (int) (afterBalance - beforeBalance));
}

int
main(int argc, char *argv[])
{
    LongBowRunner *testRunner = LONGBOW_TEST_RUNNER_CREATE(cpi_InterfaceGeneric);
    int exitStatus = longBowMain(argc, argv, testRunner, NULL);
    longBowTestRunner_Destroy(&testRunner);
    exit(exitStatus);
}
