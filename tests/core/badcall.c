/*-
 * Copyright (c) 2015 David Chisnall
 * All rights reserved.
 *
 * This software was developed by SRI International and the University of
 * Cambridge Computer Laboratory under DARPA/AFRL contract (FA8750-10-C-0237)
 * ("CTSRD"), as part of the DARPA CRASH research programme.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <test_suite.h>
#include <cheri-builtins.h>
#include <setjmp.h>
#include <stdbool.h>

typedef void(*fnptr)(void);
extern int a;
extern fnptr not_a_function;
int a;
fnptr not_a_function = (fnptr)&a;

void tryInvalidFunctionPointer(fnptr fptr)
{
	volatile bool trapped = false;
	if (NEXT_ERROR_SETJMP() == 0)
	{
		lastExceptionCause = CHERICauseNone;
		fptr();
	}
	else
	{
		trapped = true;
	}
	pass(trapped, "Calling invalid function pointer trapped");
}

volatile int calls = 12;

void validCall()
{
	calls++;
}

BEGIN_TEST(badcall)
	// Try calling something that is not a function pointer
	fnptr x = (fnptr)(__uintcap_t)42;
	// Function pointers derived from intcap_t should not be valid and should
	// trap.
	tryInvalidFunctionPointer(x);
	// Function pointers default-initialised to 0 should trap.
	tryInvalidFunctionPointer(not_a_function);
	x = validCall;
	x();
	pass(calls == 13, "Valid function pointer was called");
	x = cheri_tag_clear(x);
	tryInvalidFunctionPointer(x);
	x = validCall;
	x = cheri_permissions_and(x, 0);
	tryInvalidFunctionPointer(x);
	pass(calls == 13, "Invalid function pointer was not called");
END_TEST
