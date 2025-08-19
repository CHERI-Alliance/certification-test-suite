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

#pragma clang diagnostic ignored "-Wmissing-variable-declarations"

char foo[] = "01234";
// Check for initialisation of global pointers
volatile char *bar = (volatile char*)&foo;
typedef void(*fn)(void);

int called;
void test_fn_ptr(void)
{
	called++;
}
// Check that function pointers are correctly initialised both inside
// structures and bare:
struct f
{
	int x;
	fn f;
} x = {0, test_fn_ptr};

fn f = test_fn_ptr;

BEGIN_TEST(init)
	// Check that the initialisation of a pointer to a global worked:
	pass(bar[0] == '0', "Initialising global pointer is correct");
	pass(bar[1] == '1', "Initialising global pointer is correct");
	pass(bar[2] == '2', "Initialising global pointer is correct");
	pass(bar[3] == '3', "Initialising global pointer is correct");
	pass(bar[4] == '4', "Initialising global pointer is correct");
	pass(bar[5] == 0, "Initialising global pointer is correct");
	// Check that the two function pointers point to the correct place.
	pass(cheri_is_equal_exact((void*)f, (void*)test_fn_ptr), "Function pointer in global");
	pass(cheri_is_equal_exact((void*)x.f, (void*)test_fn_ptr), "Function pointer in global");
	void *pcc = __builtin_cheri_program_counter_get();
	// Pointers to functions should be pcc with the offset set to the address
	// of the function.
	pass(__builtin_cheri_length_get(pcc) == __builtin_cheri_length_get((void*)f), "Function pointer bounds are correct");
	pass(__builtin_cheri_base_get(pcc) == __builtin_cheri_base_get((void*)f), "Function pointer bounds are correct");
	// That's all good in theory - now check that we can actually call the
	// functions!
	x.f();
	f();
	pass(called == 2, "Calling function indirectly and directly");
END_TEST
