#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <test_suite.h>
#include <signal.h>
#include <string.h>

#define DEFINE_TEST(name, description) void name(void);
#include <tests.h>

void write_log(const char *str)
{
	printf("%s", str);
}

void write_number(int number)
{
	printf("%d", number);
}

void unrecoverable_failure()
{
	abort();
}

jmp_buf *platform_jmpbuf()
{
	static jmp_buf jump_buf;
	return &jump_buf;
}

#ifdef __aarch64__
#define uc_pcc uc_mcontext.mc_capregs.cap_elr
#else
#error Location of program counter capability in ucontext must be defined
#endif

static void handle_sigprot(int signo, siginfo_t *info, ucontext_t *ucontext)
{
	CHERIExceptionCause cause;
	if (signo == SIGPROT)
	{
		switch (info->si_code)
		{
			case PROT_CHERI_BOUNDS:
				cause = CHERICauseBounds;
				break;
			case PROT_CHERI_TAG:
				cause = CHERICauseTag;
				break;
			case PROT_CHERI_PERM:
				cause = CHERICausePermission;
				break;
			default:
				cause = CHERICauseOther;
				break;
		}
	}
	else
	{
		cause = CHERICauseOther;
	}
	report_error(cause);
	switch (nextErrorBehaviour)
	{
		case ErrorFailTest:
			printf("Test Failed: %s\n", currentTestName);
			abort();
		case ErrorSkipInstruction:
			ucontext->uc_pcc = (uintptr_t)skip_instruction((void*)ucontext->uc_pcc);
			return;
		case ErrorLongJmp:
			longjmp(*platform_jmpbuf(), 1);
	}
};

int main(void)
{
	struct  sigaction sa;
	memset(&sa, 0, sizeof(sa));
	sa.sa_sigaction = (void (*)(int, struct __siginfo *, void *))handle_sigprot;
	sa.sa_flags = SA_SIGINFO;
	sigaction(SIGPROT, &sa, NULL);
	// On Morello, traps happen after jumps, which means you get a bus error on
	// the jump.
	sigaction(SIGBUS, &sa, NULL);
#define DEFINE_TEST(name, description)                                         \
	currentTestName = description;                                             \
	name();
#include <tests.h>
	report_final_result();
}
