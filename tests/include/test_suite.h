#pragma once
#include "integration.h"

#ifndef CHERI_TEST_SUITE_GLOBAL
#	define CHERI_TEST_SUITE_GLOBAL extern
#endif

/**
 * Error handler behaviour.  If a CHERI exception is thrown, the handler
 * provided by the integration is responsible for implementing one of these
 * policies.
 */
typedef enum
{
	/**
	 * Fatal and unrecoverable error.  The test suite runner may either fail
	 * entirely or skip to the end.
	 */
	ErrorFailTest,
	/**
	 * The faulting instruction should be skipped.  The `skip_instruction`
	 * helper can be used with a tagged program counter capability to advance
	 * to the next instruction.
	 */
	ErrorSkipInstruction,
	/**
	 * The exception handler should jump out to the jump buffer provided by the
	 * integration.
	 */
	ErrorLongJmp,
} CHERIErrorBehaviour;

/**
 * Architecture-agnostic set of CHERI exception causes.
 */
typedef enum
{
	/// Invalid type.
	CHERICauseNone = 0,
	/// Bounds violation.
	CHERICauseBounds,
	/// Permission violation (any).
	CHERICausePermission,
	/// Tag violation.
	CHERICauseTag,
	/// Seal violation.
	CHERICauseSeal,
	/// Any other CHERI exception kind.
	CHERICauseOther,
} CHERIExceptionCause;

/// The number of individual checks that have passed.
CHERI_TEST_SUITE_GLOBAL int testPasses;

/// The number of individual checks that have failed.
CHERI_TEST_SUITE_GLOBAL int testFailures;

/// The number of complete tests that have run.
CHERI_TEST_SUITE_GLOBAL int testsRun;

/// The desired way for the next error to be handled.
CHERI_TEST_SUITE_GLOBAL CHERIErrorBehaviour nextErrorBehaviour;

/**
 * The number of CHERI exceptions that have been detected.  This is updated
 * only by `report_error`.
 */
CHERI_TEST_SUITE_GLOBAL _Atomic(int) exceptionCount;

/**
 * The cause of the most recent CHERI exception.  This is updated by
 * `report_error`, and initialised by tests.
 */
CHERI_TEST_SUITE_GLOBAL _Atomic(CHERIExceptionCause) lastExceptionCause;

/// The name of the current test.
CHERI_TEST_SUITE_GLOBAL const char *currentTestName;

#if defined(__cplusplus) && !defined(_Bool)
using _Bool = bool;
#endif

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Helper for each test.  Records pass or failure and reports using
	 * `description`.
	 */
	void pass(_Bool condition, const char *description);

	/**
	 * Helper for test suite integrations.  Reports the final result.
	 *
	 * This should be called after all tests have run.
	 */
	void report_final_result();

	/**
	 * Architecture-specific code to skip the instruction.  Called with a tagged
	 * and valid PCC.  This is a helper for integrations to use in their error
	 * handlers.
	 */
	void *skip_instruction(void *pcc);

	/**
	 * Report that a CHERI exception has been raised.
	 */
	void report_error(CHERIExceptionCause cause);

#ifdef __cplusplus
}
#endif

#define NEXT_ERROR_SETJMP()                                                    \
	({                                                                         \
		nextErrorBehaviour = ErrorLongJmp;                                     \
		int ret            = setjmp(*platform_jmpbuf());                       \
		if (ret != 0)                                                          \
		{                                                                      \
			nextErrorBehaviour = ErrorFailTest;                                \
		}                                                                      \
		ret;                                                                   \
	})


#ifndef BEGIN_TEST
/**
 * Macro to define the beginning of a test.  Integrations may replace this with
 * something else but they are responsible for ensuring that the output remains
 * consistent.
 */
#	define BEGIN_TEST(name)                                                   \
		void name()                                                            \
		{                                                                      \
			testsRun++;                                                        \
			int passesAtStart   = testPasses;                                  \
			int failuresAtStart = testFailures;
#	define END_TEST                                                           \
		write_log("==========================================================" \
		          "======================\n");                                 \
		write_log(currentTestName);                                            \
		write_log(" test finished: ");                                         \
		write_number(testPasses - passesAtStart);                              \
		write_log(" passes, ");                                                \
		write_number(testFailures - failuresAtStart);                          \
		write_log(" failures\n");                                              \
		write_log("==========================================================" \
		          "======================\n");                                 \
		}
#endif
