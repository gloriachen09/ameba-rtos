#-------------------------------------------------------------------------------
# Copyright (c) 2021-2022, Arm Limited. All rights reserved.
# Copyright (c) 2022 Cypress Semiconductor Corporation (an Infineon company)
# or an affiliate of Cypress Semiconductor Corporation. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#
#-------------------------------------------------------------------------------

# Since the main test directory is a NS dir, this subdir is specifically made a
# secure directory as it build the secure side tests and services
if (EXISTS ${TARGET_PLATFORM_PATH}/preload_ns.cmake)
    include(${TARGET_PLATFORM_PATH}/preload.cmake)
    tfm_toolchain_reload_compiler()
endif()

# Install directory for non-secure interface API of test services
# NS can pick up those interface files to build non-secure test cases
set(TEST_SERVICE_SRC_INSTALL_DIR ${TFM_INSTALL_PATH}/interface/src/test_service)
set(TEST_SERVICE_INC_INSTALL_DIR ${TFM_INSTALL_PATH}/interface/include/test_service)

# Test services are also required by some NS regression tests.
# Include test services at first no matter whether secure tests are enabled.
add_subdirectory(suites/spm/ipc/service ${CMAKE_BINARY_DIR}/tf-m-tests/ipc_srv)
add_subdirectory(suites/spm/sfn/service ${CMAKE_BINARY_DIR}/tf-m-tests/sfn_srv)
add_subdirectory(suites/spm/irq/service ${CMAKE_BINARY_DIR}/tf-m-tests/irq_srv)
add_subdirectory(suites/ps/service ${CMAKE_BINARY_DIR}/tf-m-tests/ps_srv)
add_subdirectory(suites/fpu/service ${CMAKE_BINARY_DIR}/tf-m-tests/fpu_srv)

if (NOT TEST_FRAMEWORK_S)
    return()
endif()

# secure test services are required if any secure test is opened
add_subdirectory(common_test_services/tfm_secure_client_service ${CMAKE_BINARY_DIR}/tf-m-tests/secure_client_srv)
add_subdirectory(common_test_services/tfm_secure_client_2 ${CMAKE_BINARY_DIR}/tf-m-tests/secure_client_2_srv)

add_library(tfm_test_framework_s INTERFACE)
add_library(tfm_s_tests INTERFACE)

target_link_libraries(tfm_test_framework_s
    INTERFACE
        psa_interface
        tfm_test_framework_common
        tfm_sprt
)

target_compile_definitions(tfm_test_framework_s
    INTERFACE
        USE_SP_LOG
)

target_sources(tfm_s_tests
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/secure_suites.c
)

target_link_libraries(tfm_s_tests
    INTERFACE
        tfm_test_framework_s
        tfm_partition_defs
        tfm_spm
)

target_compile_definitions(tfm_s_tests
    INTERFACE
        $<$<BOOL:${PS_TEST_NV_COUNTERS}>:PS_TEST_NV_COUNTERS>
)

############################# Secure tests #####################################

# Add secure test library targets here to inherit secure side configurations
if (TEST_S_CRYPTO)
    add_library(tfm_test_suite_crypto_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_ATTESTATION)
    add_library(tfm_test_suite_attestation_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_ITS)
    add_library(tfm_test_suite_its_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_PS)
    add_library(tfm_test_suite_ps_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_PLATFORM)
    add_library(tfm_test_suite_platform_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_IPC)
    add_library(tfm_test_suite_ipc_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_FWU)
    add_library(tfm_test_suite_fwu_s STATIC EXCLUDE_FROM_ALL)
endif()
if (TEST_S_FPU)
    add_library(tfm_test_suite_fpu_s STATIC EXCLUDE_FROM_ALL)
endif()
if (EXTRA_S_TEST_SUITE_PATH)
    add_library(tfm_test_suite_extra_s STATIC EXCLUDE_FROM_ALL)
endif()
