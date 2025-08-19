#pragma once
// Copyright David Chisnall
// SPDX-License-Identifier: MIT

#include <setjmp.h>

#ifdef __cplusplus
extern "C"
{
#endif

	/**
	 * Integrator hook for writing a message to some suitable log.  This may be
	 * a UART or some other output hook.
	 */
	void write_log(const char *);

	/**
	 * Integrator hook for writing a number to some suitable log.  This may be
	 * a UART or some other output hook.
	 */
	void write_number(int);

	/**
	 * Integrator hook called when a failure in a test cannot be recovered.  If
	 * tests are isolated, this may skip to the next one.  Alternatively, all
	 * tests are reported as failed.
	 */
	_Noreturn void unrecoverable_failure(void);

	/**
	 * Return the pointer to a jump buffer that can be used with `setjmp`.  The
	 * platform's error handler must jump to this if `nextErrorBehaviour` is
	 * `ErrorLongJmp`.
	 */
	jmp_buf *platform_jmpbuf(void);

#ifdef __cplusplus
}
#endif
