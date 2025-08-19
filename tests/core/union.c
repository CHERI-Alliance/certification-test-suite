/*-
 * Copyright (c) 2012-2015 David Chisnall
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
#include <stdint.h>

union ptr_or_data
{
	void* ptr;
	char bytes[sizeof(void*)];
	uint16_t halfwords[sizeof(void*)/2];
	uint32_t words[sizeof(void*)/4];
};

static char buffer[] = "1234567";
static char *ptr = buffer;

BEGIN_TEST(unions)
	// Check that overwriting a capability in memory gives you something that
	// is not a valid capability.
	// Note that this needs to be volatile, as otherwise the aliasing rules in
	// C permit the compiler to treat the loads and stores as separate memory
	// locations and for the writes to be ordered after the reads.
	volatile union ptr_or_data p;
	for (int i=0 ; i<sizeof(void*) ; i++)
	{
		p.ptr = ptr;
		pass(cheri_length_get(p.ptr), "Pointer in union has correct ");
		pass(cheri_is_valid(p.ptr), "Union of a pointer and data is valid");
		p.bytes[i] = 40;
		pass(!cheri_is_valid(p.ptr), "Overwriting part of a capability cleared the tag");
	}
	for (int i=0 ; i<sizeof(void*)/2 ; i++)
	{
		p.ptr = ptr;
		pass(cheri_length_get(p.ptr), "Pointer in union has correct ");
		pass(cheri_is_valid(p.ptr), "Union of a pointer and data is valid");
		p.halfwords[i] = 400;
		pass(!cheri_is_valid(p.ptr), "Overwriting part of a capability cleared the tag");
	}
	for (int i=0 ; i<sizeof(void*)/4 ; i++)
	{
		p.ptr = ptr;
		pass(cheri_length_get(p.ptr), "Pointer in union has correct ");
		pass(cheri_is_valid(p.ptr), "Union of a pointer and data is valid");
		p.words[i] = 0xf00dface;
		pass(!cheri_is_valid(p.ptr), "Overwriting part of a capability cleared the tag");
	}
END_TEST

