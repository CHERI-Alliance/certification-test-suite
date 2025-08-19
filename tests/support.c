#define CHERI_TEST_SUITE_GLOBAL
#include <test_suite.h>

CHERIErrorBehaviour nextErrorBehaviour;

_Atomic(int)                 exceptionCount;
_Atomic(CHERIExceptionCause) lastExceptionCause;
const char                  *currentTestName;

int testPasses;
int testFailures;

int testsRun;

void pass(_Bool condition, const char *description)
{
#ifdef CHERI_TESTS_QUIET
#	define WRITE_LOG(x)
#else
#	define WRITE_LOG(x) write_log(x)
#endif
	WRITE_LOG("[");
	WRITE_LOG(currentTestName);
	WRITE_LOG("] ");
	WRITE_LOG(description);
	WRITE_LOG(": ");
	if (condition)
	{
		WRITE_LOG("PASSED\n");
		testPasses++;
	}
	else
	{
		WRITE_LOG("FAILED\n");
		testFailures++;
	}
}

void report_final_result(void)
{
	write_log("\nTests completed:\n\t");
	write_number(testsRun);
	write_log(" tests run.\n\t");
	write_number(testPasses);
	write_log(" checks passed.\n\t");
	write_number(testFailures);
	write_log(" checks failed.\n");
	int expectedPasses = 118;
	// Capability overlap tests run tests for each subset write.
	if (sizeof(void*) == 16)
	{
		expectedPasses += 42;
	}
	write_log("Full test suite is 12 tests with ");
	write_number(expectedPasses);
	write_log(" checks\n");
	int maximumLevel   = 3;
#if defined(EXCLUDE_TEMPORAL_SAFETY)
	maximumLevel = 1;
	expectedPasses -= 3;
	write_log("\tTemporal safety checks skipped (required for CHERI Level 2 "
	          "certification)\n");
#elif defined(EXCLUDE_USE_AFTER_FREE)
	maximumLevel = 2;
	expectedPasses -= 1;
	write_log("\tUse-after-free checks skipped (required for CHERI Level 3 "
	          "certification)\n");
#endif
	if ((testFailures == 0) && (expectedPasses == testPasses))
	{
		write_log("\nCHERI Alliance Certification Level: ");
		write_number(maximumLevel);
		write_log("\n\n\n");
	}
}

void *skip_instruction(void *pccAsVoid)
{
	char *pcc = pccAsVoid;
#ifdef __aarch64__
	return pcc + 4;
#elif defined(__riscv)
	if (pcc[0] & 0b11)
	{
		return pcc + 4;
	}
	return pcc + 2;
#else
#error Architecture-specific code for skipping faulting instructions not defined.
#endif
}

void report_error(CHERIExceptionCause cause)
{
	lastExceptionCause = cause;
	exceptionCount++;
}
