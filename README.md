CHERI Conformance Test Suite
============================

This is a (work-in-progress) test suite for CHERI Alliance CHERI Certification.
It contains tests mostly inherited from the CHERI C test suite, with ones intended to test corner cases of CHERI MIPS behaviour removed, and the remainder cleaned up.

The test suite contains a number of tests for different aspects of a CHERI system.
Each of these includes one or more checks.
All checks must pass for the test suite to be considered to have passed.

Platform integration
--------------------

The test suite is intended to be possible to run bare metal in a freestanding C environment or in a hosted environment on an operating system.


The tests for temporal safety require implementations of `malloc` and `free`, but these can be very simple.
They will be called with a constant argument, a limited number of times, from a single thread.
A toy implementation that demonstrates temporal safety support is sufficient.
If this is not possible, the relevant tests can be excluded (see below)

Integrating these tests with a platform (freestanding or hosted environment) requires a test runner that will call each test.
The simplest way to do this is to define each test as a function and then to call it:

```c
// At the top level.
#define DEFINE_TEST(name, description) void name(void);
#include <tests.h>

...
// In the test entry-point function:
#define DEFINE_TEST(name, description)                                         \
    currentTestName = description;                                             \
    name();
```

The test runner must call `report_final_result();` at the end of the tests.
It must also implement the four functions defined in [integration.h](tests/include/integration.h).

Finally, the test runner must handle CHERI traps.
The trap handler must:

1. Map the platform-specific set of CHERI trap reasons to the `CHERIExceptionCause` enumeration.
2. Call `report_error` with the result of this mapping.
3. Inspect the value of `nextErrorBehaviour` and act accordingly:
 - If the value is `ErrorFailTest`, the test suite has encountered a catastrophic failure.
   The test runner may either abort entirely, or try running the next test (the latter for information only: the test suite has failed if this cause is encountered).
 - If the value is `ErrorSkipInstruction`, it must resume execution after the faulting instruction.
   The `skip_instruction` helper is provided for some architectures (currently RISC-V and AArch64) to skip the instruction given a valid program counter capability.
 - If the value is `ErrorLongJmp` then the handler must call `longjmp` on the jump buffer returned from `platform_jmpbuf()` and must set the return value for `longjmp` to a non-zero value.


### Supporting non-immediate revocation

The CHERI SoC working group defines two levels of temporal safety:

 - Use-after-free protection, which requires capabilities to become unusable as soon as an object is freed.
 - Use-after-reuse protection, which requires dangling pointers to be eventually invalidated, and not reused until this has happened.

If your platform supports deterministic use-after-free protection, you do not need to do anything, you can run the test suite in the default configuration.

If it supports only use-after-reuse protection, then you must define two macros:

 - `EXCLUDE_USE_AFTER_FREE` excludes the use-after-free tests if defined.
 - `PLATFORM_REVOCATION_BARRIER` is defined to the function that should be called to force any operations that invalidate dangling pointers.

See the CheriBSD integration for an example of these.

If your platform does not support temporal safety at all, you can define `EXCLUDE_TEMPORAL_SAFETY` to exclude these.

### Reducing verbosity

By default, the test suite will report every passed and failed check.
If an integration defines `CHERI_TESTS_QUIET`, only completed tests and failed checks will be reported.
