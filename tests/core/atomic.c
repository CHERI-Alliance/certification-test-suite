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
#include <stdatomic.h>
#include <cheri-builtins.h>
#include <stdio.h>

#ifndef NULL
#define NULL ((void*)0)
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wmissing-variable-declarations"
#endif

_Atomic(char) c;
_Atomic(short) h;
_Atomic(int) w;
_Atomic(long long) d;
_Atomic(int*) p = NULL;

_Atomic(char) *cp = &c;
_Atomic(short) *hp = &h;
_Atomic(int) *wp = &w;
_Atomic(long long) *dp = &d;
_Atomic(int*) *pp = &p;

BEGIN_TEST(atomic)
	pass(cheri_length_get(cp) == sizeof(c), "Correct bounds for global atomic char");
	pass(cheri_length_get(hp) == sizeof(h), "Correct bounds for global atomic short");
	pass(cheri_length_get(wp) == sizeof(w), "Correct bounds for global atomic int");
	pass(cheri_length_get(dp) == sizeof(d), "Correct bounds for global atomic long long");
	c++;
	h++;
	w++;
	d++;
	(*cp)++;
	(*hp)++;
	(*wp)++;
	(*dp)++;
	pass(c == 2, "Successfully incremented atomic char");
	pass(h == 2, "Successfully incremented atomic short");
	pass(w == 2, "Successfully incremented atomic int");
	pass(d == 2, "Successfully incremented atomic long long");

	// Test operations on pointers:
	pass(cheri_length_get(pp) == sizeof(p), "Correct bounds for global atomic pointer");
	static int newval;
	pass(cheri_is_equal_exact(pp, &p), "Correct initialisation for global atomic pointer");
	*pp = (int*)(__uintcap_t)1;
	int* expected = (int*)(__uintcap_t)1;
	pass(atomic_compare_exchange_strong_explicit(&p, &expected, &newval, memory_order_seq_cst, memory_order_seq_cst), "Compare and exchange on pointers");
	pass(cheri_is_equal_exact(p, &newval), "Atomic compare and exchange wrote the correct value");
	int* xchg_value = atomic_exchange_explicit(&p, (void*)(__uintcap_t)2, memory_order_seq_cst);
	pass(cheri_is_equal_exact(xchg_value, &newval), "Atomic exchange read the correct value");
	pass(cheri_is_equal_exact(p, (void*)(__uintcap_t)2), "Atomic exchange wrote the correct value");
	atomic_store_explicit(&p, (void*)(__uintcap_t)3, memory_order_seq_cst);
	pass(cheri_is_equal_exact(p, (void*)(__uintcap_t)3), "Atomic store wrote the correct value");
END_TEST

