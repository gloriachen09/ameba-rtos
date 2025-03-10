cmake_minimum_required(VERSION 3.20.0)

### GLOBAL DIRECTIONS AND VALUES DEFINE ###
set(PROJECTDIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRCDIR ${PROJECTDIR}/src)
set(INCDIR ${PROJECTDIR}/inc)
set(BASEDIR ${CMAKE_CURRENT_SOURCE_DIR}/../..)
#remove ../.. in BASEDIR, because BASEDIR is used by so many CMakeLists files
cmake_path(NORMAL_PATH BASEDIR)


set(KR4_BUILDDIR ${PROJECTDIR}/../project_kr4)
cmake_path(NORMAL_PATH KR4_BUILDDIR)
set(KM4_BUILDDIR ${PROJECTDIR})


set(TARGETDIR ${BASEDIR}/component/soc/amebalite)
set(CMAKE_FILES_DIR ${BASEDIR}/cmake)

set(FREERTOS_VER v10.4.3)
set(MBEDTLS_VER 3.6.2)
set(LITTLEFS_VER r2.50)
set(FATFS_VER r0.14b)
set(LWIP_VER v2.1.2)

set(FREERTOSDIR ${BASEDIR}/component/os/freertos/freertos_${FREERTOS_VER}/Source)
set(OSDIR ${BASEDIR}/component/os)
set(MBEDTLSDIR ${BASEDIR}/component/ssl/mbedtls-${MBEDTLS_VER})
set(WIFIFWDIR ${BASEDIR}/component/wifi/wifi_fw)
set(WIFIMAKEDIR ${BASEDIR}/component/wifi/wifi_make)

set(LDDIR ${CMAKE_CURRENT_SOURCE_DIR}/asdk/ld)

set(UTILITYDIR ${PROJECTDIR}/asdk/gnu_utility)
set(FLASHDLDIR ${PROJECTDIR}/asdk/flashloader)
set(GNU_SCRIPT ${UTILITYDIR}/gnu_script)
set(FLASHDLIMGDIR ${GNU_SCRIPT}/acut_loader)
set(FLOADER_BIN_DIR ${UTILITYDIR}/image_tool_flashloader/amebalite_acut)

set(TARGET_LIBSOC_FOLDER ${PROJECTDIR}/asdk/lib/soc)
set(TARGET_LIBAPP_FOLDER ${PROJECTDIR}/asdk/lib/application)

if(NOT FINAL_IMAGE_DIR)
    set(FINAL_IMAGE_DIR ${PROJECTDIR}/..)
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

project(km4 LANGUAGES C CXX ASM)
message ("project : " ${PROJECT_NAME})

set(CMAKE_BUILD_TYPE "")
# set CMAKE_VERBOSE_MAKEFILE ON to print compile info
set(CMAKE_VERBOSE_MAKEFILE OFF)

###########################################
file(READ ${PROJECTDIR}/../utils/jlink_script/gdb.py content)
string(REGEX REPLACE "CMAKE_GDB = [^\r\n\t]*" "CMAKE_GDB = \'${CMAKE_GDB}\'" content ${content})
file(WRITE ${PROJECTDIR}/../utils/jlink_script/gdb.py ${content})
#############   include  ##################

include (${CMAKE_FILES_DIR}/ccache.cmake)

include (${CMAKE_FILES_DIR}/extensions.cmake)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/../menuconfig/.config_km4 ${CMAKE_CURRENT_BINARY_DIR}/.config_km4 COPYONLY)
import_kconfig("CONFIG" ${CMAKE_CURRENT_BINARY_DIR}/.config_km4)
set(CONFIG_CPU_HAS_FPU y) ####TEMP....TODO DELETE
include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/compileflags.cmake)

include (${CMAKE_FILES_DIR}/flags/${PROJECT_NAME}/linkflags.cmake)

include(${CMAKE_FILES_DIR}/axf2bin.cmake)

include(${CMAKE_FILES_DIR}/git/GetGitRevisionDescription.cmake)
git_describe(GIT_VER)
###########################################

if(CONFIG_AS_INIC_AP)
    set(AP_IMAGEDIR ${KM4_BUILDDIR}/asdk/image)
else()
    set(AP_IMAGEDIR ${KR4_BUILDDIR}/vsdk/image)
endif()

if (CONFIG_MP_INCLUDED)
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image_mp)
    set(BUILD_TYPE MFG)
    set(ANALYZE_MP_IMG 1)

else()
    set(IMAGE_TARGET_FOLDER ${PROJECTDIR}/asdk/image)
    set(BUILD_TYPE NONE)
    set(ANALYZE_MP_IMG 0)
endif()

set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})

###ADD SUBDIRECTORIES ###
#they are all in asdk
add_subdirectory(asdk)
#########################
