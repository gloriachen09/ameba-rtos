###############################################################################
#NOTE: Configures, these parameter name is fixed and cannot be changed
# set(v_ASDK_VER 10.3.1)
# set(v_VSDK_VER 10.3.1)
# set(v_FATFS_VER r0.14b)
# set(v_FREERTOS_VER v10.2.1)
# set(c_FREERTOS_DIR_SUFFIX _smp)
# set(v_LITTLEFS_VER r2.50)
# set(v_LWIP_VER v2.1.2)

set(c_VERBOSE_MAKEFILE FALSE)
###############################################################################

ameba_mcu_project_create(km4tz km4)
ameba_mcu_project_check()

###############################################################################
set(DAILY_BUILD 0 CACHE STRING "code analysis argument")
message ("DAILY_BUILD = " ${DAILY_BUILD})
ameba_add_subdirectory(${c_SDK_NAME})