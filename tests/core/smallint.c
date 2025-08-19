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
#include <cheri-builtins.h>
#include <stdint.h>
#include <test_suite.h>

/* This use of the offset field works fine -> silence warning */
#ifdef __clang__
#	pragma clang diagnostic ignored "-Wcheri-bitwise-operations"
#endif

_Static_assert((sizeof(void *) == 8) || (sizeof(void *) == 16),
               "This test assumes pointers are 64 or 128 bits and will need to "
               "be modified for other CHERI targets");
const ptraddr_t TagBits = sizeof(void *) == 8 ? 3 : 4;
const ptraddr_t TagMask = (1 << TagBits) - 1;

typedef union
{
	void     *ptr;
	uintptr_t intptr;
} PtrIntPair;

__attribute__((noinline)) static unsigned get_int(PtrIntPair p)
{
	return p.intptr & TagMask;
}

__attribute__((noinline)) static PtrIntPair set_int(PtrIntPair p, int val)
{
	val &= TagMask;
	p.intptr &= ~TagMask;
	p.intptr ^= val;
	return p;
}

__attribute__((noinline)) static void *get_pointer(PtrIntPair p)
{
	return (void *)(p.intptr & ~TagMask);
}

BEGIN_TEST(smallint)
	nextErrorBehaviour     = ErrorSkipInstruction;
	exceptionCount         = 0;
	_Alignas(void*) char str[] = "123456789";
	PtrIntPair p;
	p.ptr = str;
	pass((cheri_address_get(p.ptr) & TagMask) == 0, "Alignment check for pointers");
	p = set_int(p, 4);
	pass(get_int(p) == 4, "Tag stored in low bits correctly");
	char *ptr = get_pointer(p);
	pass(cheri_is_equal_exact(ptr, str),
		 "Manipulating the low bits did not clear the tag");
	pass(ptr[0] == '1', "Pointer works correctly");
	pass(exceptionCount == 0, "No exceptions raised");
END_TEST
