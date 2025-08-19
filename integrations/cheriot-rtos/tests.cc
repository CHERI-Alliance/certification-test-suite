#include <test_suite.h>

#include <compartment.h>
#include <debug.hh>
#include <priv/riscv.h>
#include <simulator.h>
#include <unwind.h>

using Debug = ConditionalDebug<true, "Compliance test suite">;

#define DEFINE_TEST(name, description) extern "C" void name();
#include <tests.h>

using namespace CHERI;

namespace
{
	/**
	 * Structure containing a jump buffer.  The definition of `jmp_buf` is a
	 * size-one array so that it can be passed 'by reference' in C, but that
	 * ends up being annoying in various other contexts.  Wrapping it in a
	 * structure addresses this.
	 */
	struct JmpBufWrapper
	{
		jmp_buf jumpbuf;
	};

	/**
	 * The address of the on-stack jump buffer.
	 */
	ptraddr_t jumpbufAddress;

	/**
	 * Get the on-stack jump buffer.
	 */
	jmp_buf &get_jumpbuf()
	{
		Capability<void> jumpbuf = __builtin_cheri_stack_get();
		jumpbuf.address()        = jumpbufAddress;
		jumpbuf.bounds()         = sizeof(jmp_buf);
		return jumpbuf.cast<JmpBufWrapper>()->jumpbuf;
	}

} // namespace

enum ErrorRecoveryBehaviour
compartment_error_handler(struct ErrorState *frame, size_t mcause, size_t mtval)
{
	// The test suite should be generating only CHERI exceptions, if anything
	// else happens then force unwind.
	if (mcause != priv::MCAUSE_CHERI)
	{
		Debug::log("Non-cheri fault!");
		return ForceUnwind;
	}

	// Map the hardware cause to the smaller set that the test suite requires.
	auto [exceptionCode, registerNumber] = CHERI::extract_cheri_mtval(mtval);
	CHERIExceptionCause testSuiteCause;
	switch (exceptionCode)
	{
		case CauseCode::None:
			testSuiteCause = CHERICauseOther;
			break;
		case CauseCode::Invalid:
			testSuiteCause = CHERICauseOther;
			break;
		case CauseCode::BoundsViolation:
			testSuiteCause = CHERICauseBounds;
			break;
		case CauseCode::TagViolation:
			testSuiteCause = CHERICauseTag;
			break;
		case CauseCode::SealViolation:
			testSuiteCause = CHERICauseSeal;
			break;
		case CauseCode::PermitExecuteViolation:
			testSuiteCause = CHERICausePermission;
			break;
		case CauseCode::PermitLoadViolation:
			testSuiteCause = CHERICausePermission;
			break;
		case CauseCode::PermitStoreViolation:
			testSuiteCause = CHERICausePermission;
			break;
		case CauseCode::PermitStoreCapabilityViolation:
			testSuiteCause = CHERICausePermission;
			break;
		case CauseCode::PermitStoreLocalCapabilityViolation:
			testSuiteCause = CHERICausePermission;
			break;
		case CauseCode::PermitAccessSystemRegistersViolation:
			testSuiteCause = CHERICausePermission;
			break;
	}
	report_error(testSuiteCause);
	switch (nextErrorBehaviour)
	{
		case ErrorFailTest:
			printf("Test Failed: %s\n", currentTestName);
			cleanup_unwind();
		case ErrorSkipInstruction:
		{
			// Reconstruct a tagged PCC and pass it to the handler.
			Capability<void> pcc = __builtin_cheri_program_counter_get();
			pcc.address()        = Capability<void>(frame->pcc).address();
			// If the PCC is invalid, we cannot skip the instruction, so bail
			// on this test.
			if (!pcc.is_valid())
			{
				printf("Test Failed: %s\n", currentTestName);
				cleanup_unwind();
			}
			frame->pcc = skip_instruction(pcc);
			// If error_handler returned normally, resume from the provided
			// context.
			return InstallContext;
		}
		case ErrorLongJmp:
			switcher_handler_invocation_count_reset();
			longjmp(get_jumpbuf(), 1);
	}
}

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
	simulation_exit();
}

jmp_buf *platform_jmpbuf()
{
	return &get_jumpbuf();
}

__cheriot_compartment("compliance-tests") void tests()
{
	volatile JmpBufWrapper jumpbuf;
	jumpbufAddress =
	  static_cast<ptraddr_t>(reinterpret_cast<uintptr_t>(&jumpbuf));

#define DEFINE_TEST(name, description)                                         \
	CHERIOT_DURING                                                             \
	currentTestName = description;                                             \
	name();                                                                    \
	CHERIOT_HANDLER                                                            \
	CHERIOT_END_HANDLER
#include <tests.h>
	report_final_result();
}
