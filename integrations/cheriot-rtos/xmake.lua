-- Copyright David Chisnall
-- SPDX-License-Identifier: MIT

set_project("CHERIoT Integration of CHERI Alliance test suite")
sdkdir = os.getenv("CHERIOT_SDK") or "../../../cheriot-rtos/sdk/"
includes(sdkdir)
set_toolchains("cheriot-clang")

option("board")
	set_default("sail")

compartment("compliance-tests")
	add_deps("freestanding", "stdio", "atomic")
	add_deps("debug")
	add_includedirs("../../tests/include")
	add_files("tests.cc", "../../tests/support.c", "malloc.c")
	add_files("../../tests/core/*.c")
	add_files("../../tests/temporal/*.c")
	add_defines("CHERIOT_CUSTOM_DEFAULT_MALLOC_CAPABILITY")

-- Firmware image for the example.
firmware("compliance")
	add_deps("compliance-tests")
	on_load(function(target)
		target:values_set("board", "$(board)")
		target:values_set("threads", {
			{
				compartment = "compliance-tests",
				priority = 1,
				entry_point = "tests",
				stack_size = 0x1f00,
				trusted_stack_frames = 5
			}
		}, {expand = false})
	end)
