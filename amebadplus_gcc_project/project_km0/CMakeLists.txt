if(NOT CMAKE_REFACTOR) # For temporary compatibility
cmake_minimum_required(VERSION 3.20.0)

### GLOBAL DIRECTIONS AND VALUES DEFINE ###

set(PROJECTDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCDIR ${PROJECTDIR}/src)
set(INCDIR ${PROJECTDIR}/inc)
set(BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
#remove ../.. in BASEDIR, because BASEDIR is used by so many CMakeLists files
cmake_path(NORMAL_PATH BASEDIR)

set(KM0_BUILDDIR ${PROJECTDIR})
set(KM4_BUILDDIR ${PROJECTDIR}/../project_km4)
cmake_path(NORMAL_PATH KM4_BUILDDIR)

set(TARGETDIR ${BASEDIR}/component/soc/amebadplus)
set(BOOTLOADER ${BASEDIR}/component/soc/amebadplus/bootloader)
set(CMAKE_FILES_DIR ${BASEDIR}/cmake)
set(WORKING_PROJECT_DIR ${CMAKE_BINARY_DIR}/..)
set(MENUCONFIGDIR ${WORKING_PROJECT_DIR}/menuconfig)


set(FREERTOS_VER v10.4.3)
set(MBEDTLS_VER 3.6.2)
set(LWIP_VER v2.1.2)

set(FREERTOSDIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER}/Source)
set(OSDIR ${BASEDIR}/component/os)
set(MBEDTLSDIR ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER})
set(WIFIFWDIR ${BASEDIR}/component/wifi/wifi_fw)
set(WIFIMAKEDIR ${BASEDIR}/component/wifi/wifi_make)

set(LDDIR ${CMAKE_CURRENT_SOURCE_DIR}/asdk/ld)

set(TARGET_LIBSOC_FOLDER ${PROJECTDIR}/asdk/lib/soc)
set(TARGET_LIBAPP_FOLDER ${PROJECTDIR}/asdk/lib/application)

if(NOT FINAL_IMAGE_DIR)
    set(FINAL_IMAGE_DIR ${WORKING_PROJECT_DIR})
else()
    if(NOT EXISTS FINAL_IMAGE_DIR)
        execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${FINAL_IMAGE_DIR})
    endif()
endif()

###########################################
if(NOT ASDK_VER)
    set(ASDK_VER 10.3.1)
    include (${CMAKE_FILES_DIR}/toolchain/ameba-toolchain-asdk-${ASDK_VER}.cmake)
    set(CMAKE_USER_MAKE_RULES_OVERRIDE ${CMAKE_FILES_DIR}/set_extensions.cmake)
    set(CMAKE_PROJECT_INCLUDE_BEFORE ${CMAKE_FILES_DIR}/disable_compiler_detection.cmake)
endif()

project(km0 LANGUAGES C CXX ASM)
message ("project : " ${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "")
# set CMAKE_VERBOSE_MAKEFILE ON to print compile info
set(CMAKE_VERBOSE_MAKEFILE OFF)

###########################################

#############   include  ##################
include (${CMAKE_FILES_DIR}/ccache.cmake)

include (${CMAKE_FILES_DIR}/extensions.cmake)

if (EXAMPLEDIR AND EXISTS ${EXAMPLEDIR}/prj.conf)
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${EXAMPLEDIR}/prj.conf ${MENUCONFIGDIR}/prj.conf
    )
endif()

set(PRJ_CONF)

if (EXISTS ${MENUCONFIGDIR}/prj.conf)
    list(APPEND PRJ_CONF ${MENUCONFIGDIR}/prj.conf)
endif()

if (NOT EXISTS ${MENUCONFIGDIR}/.config_km0)
    execute_process(
        COMMAND python menuconfig.py -f default.conf ${PRJ_CONF} -d ${WORKING_PROJECT_DIR}
        WORKING_DIRECTORY ${PROJECTDIR}/..
    )
endif()


configure_file(${MENUCONFIGDIR}/.config_km0 ${CMAKE_CURRENT_BINARY_DIR}/.config_km0 COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_km0)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/compileflags.cmake)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/linkflags.cmake)

include(${CMAKE_FILES_DIR}/axf2bin.cmake)

include(${CMAKE_FILES_DIR}/git/GetGitRevisionDescription.cmake)
git_describe(GIT_VER)
###########################################

if (CONFIG_MP_INCLUDED)
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image_mp)
    set(BUILD_TYPE MFG)
    set(ANALYZE_MP_IMG  1)

else()
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image)
    set(BUILD_TYPE NONE)
    set(ANALYZE_MP_IMG  0)
endif()

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
###ADD SUBDIRECTORIES ###
#they are all in asdk
add_subdirectory(asdk)
#########################
else() #For temporary compatibility.NOTE:  script below is new version cmake
###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
set(v_FREERTOS_VER v10.4.3)
set(v_LWIP_VER v2.1.2)
set(v_MBEDTLS_VER 3.6.2)

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(km0 km0)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectories(${c_SDK_NAME})
endif() # For temporary compatibility