// Copyright David Chisnall
// SPDX-License-Identifier: BSD-2-Clause

#include <test_suite.h>
#include <stdlib.h>
#include <cheri-builtins.h>

BEGIN_TEST(eager_free)
	char *volatile object = malloc(42);
	free(object);
	nextErrorBehaviour = ErrorSkipInstruction;
	exceptionCount = 0;
	object[0] = '\0';
	pass(exceptionCount == 1, "Use after free trapped");
END_TEST
