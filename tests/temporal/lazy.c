// Copyright David Chisnall
// SPDX-License-Identifier: BSD-2-Clause

#include <test_suite.h>
#include <stdlib.h>
#include <stddef.h>
#include <cheri-builtins.h>

#ifndef PLATFORM_REVOCATION_BARRIER
#define PLATFORM_REVOCATION_BARRIER do {} while(0)
#endif

BEGIN_TEST(lazy_free)
	char *object1 = malloc(42);
	ptraddr_t address1 = (ptraddr_t)object1;
	free(object1);
	char *object2 = malloc(42);
	ptraddr_t address2 = (ptraddr_t)object2;
	pass((address1 != address2) || !cheri_is_valid(object1), "Newly allocated objects do not alias old ones");
	free(object2);
	PLATFORM_REVOCATION_BARRIER;
	nextErrorBehaviour = ErrorSkipInstruction;
	exceptionCount = 0;
	object2[0] = '\0';
	pass(exceptionCount == 1, "Use after free trapped after revocation");
END_TEST
