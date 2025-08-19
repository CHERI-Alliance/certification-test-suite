cmake_minimum_required(VERSION 3.26)

project(conformance-test-suite)

message("Project root: ${PROJECT_SOURCE_DIR}")
file(GLOB CORE_SOURCES "${PROJECT_SOURCE_DIR}/../../tests/core/*.c")
file(GLOB TEMPORAL_SOURCES "${PROJECT_SOURCE_DIR}/../../tests/temporal/*.c")
add_executable(conformance-tests "main.c" ${CORE_SOURCES} ${TEMPORAL_SOURCES} "../../tests/support.c")
include_directories(AFTER ../../tests/include)
add_definitions(-DPLATFORM_REVOCATION_BARRIER=malloc_revoke_quarantine_force_flush\(\))
add_definitions(-DEXCLUDE_USE_AFTER_FREE)
