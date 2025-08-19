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
#include <stdint.h>
#include <stddef.h>
#include <cheri-builtins.h>

extern volatile intptr_t tmp;
extern volatile intptr_t one;
extern volatile intptr_t two;
volatile intptr_t tmp;
volatile intptr_t one = 1;
volatile intptr_t two = 2;

// Modern CHERI systems don't expose offsets natively, but they're quite
// convenient for this test so pretend that we have them.
#ifndef cheri_offset_get
#define cheri_offset_get(x) ({ __typeof__((x)) tmp = (x); cheri_address_get(tmp) - cheri_base_get(tmp); })
#endif

BEGIN_TEST(intptr)
	nextErrorBehaviour = ErrorSkipInstruction;
	static char str[] = "0123456789";
	intptr_t foo = 42;
	pass(cheri_is_valid((void*)foo) == 0, "intptr_t from integer is untagged");
	pass(cheri_address_get((void*)foo) == 42, "intptr_t from integer has correct address");
	pass(cheri_base_get((void*)foo) == 0, "intptr_t from integer has zero base");
	foo = (intptr_t)str;
	pass(cheri_is_valid((void*)foo), "intptr_t from string is valid");
	foo += 5;
	pass((*(char*)foo) == '5', "Round trips and arithmetic on intptr_t work");
	pass(cheri_offset_get((void*)foo) == 5, "Offset of intptr_t arithmetic is correct");
	pass(cheri_base_get((void*)foo) == cheri_base_get(str), "Base of intptr_t arithmetic is correct");
	pass(cheri_length_get((void*)foo) == cheri_length_get(str), "Length of intptr_t arithmetic is correct");
	// Try taking the pointer out of bounds and back.
	// If this test fails, the `displacement` constant below may need to be
	// tweaked for some encodings.
	const ptrdiff_t displacement = 50;
	//_Static_assert(displacement >= 6, "Displacement must take the pointer at least to the one-past-the-end address");
	foo += displacement;
	// Ensure that the +50 is not removed
	tmp = foo;
	foo = tmp;
	foo -= displacement;
	pass((*(char*)foo) == '5', "Out of bounds displacement and back");
	pass(cheri_offset_get((void*)foo) == 5, "Offset after out-of-bounds displacement");
	pass((size_t)one == (size_t)1, "intptr_t 1 is correctly cast to an integer");
	// Check that storing a capability always yields the same value back
	intptr_t tmp2 = two;
	pass(cheri_is_equal_exact((void*)tmp2, (void*)(intptr_t)2), "Storing an intptr_t doesn't change the bit pattern");
	pass((size_t)tmp2 == (size_t)2, "intptr_t 2 is correctly cast to an integer");
	two = 3;
	pass((size_t)two == (size_t)3, "intptr_t 3 stored through a global is correctly cast to an integer");
	two = tmp2; // restore old value
	pass((size_t)two == (size_t)2, "intptr_t 2 stored through a global is correctly cast to an integer");
	pass(cheri_is_equal_exact((void*)two, (void*)(intptr_t)2), "Exact equality for provenance-free intptr_t is path independent");
END_TEST
