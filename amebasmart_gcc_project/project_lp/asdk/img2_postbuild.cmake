#Note: Previously defined variables cannot be used directly in this file
#unless passed through -D
include(${CMAKE_FILES_DIR}/axf2bin.cmake)
include(${CMAKE_FILES_DIR}/utility.cmake)
set(t_USER_CUSTOM_LOG_PREFIX "LP_POSTBUILD")

execute_process(
    COMMAND ${CMAKE_OBJCOPY} -j .sram_image2.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/sram_2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .xip_image2.text -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/xip_image2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .psram_image2.text.data -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/psram_image2.bin
    COMMAND ${CMAKE_OBJCOPY} -j .ram_retention.text -j .ram_retention.entry -Obinary ${IMAGE_TARGET_FOLDER}/target_pure_img2.axf ${IMAGE_TARGET_FOLDER}/ram_retention.bin
)

message( "========== Image Info HEX ==========")

execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=16 ${IMAGE_TARGET_FOLDER}/target_img2.axf
)

message("========== Image Info HEX ==========")


message( "========== Image Info DEC ==========")

execute_process(
    COMMAND ${CMAKE_SIZE} -A --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf
)
execute_process(
    COMMAND ${CMAKE_SIZE} -t --radix=10 ${IMAGE_TARGET_FOLDER}/target_img2.axf
)

message( "========== Image Info DEC ==========")


message( "========== Image manipulating start ==========")
execute_process(
	COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/sram_2.bin 32
	COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/psram_image2.bin 32
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin 32
)


execute_process(
    # COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/ram_retention.bin  __retention_entry_func__  ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/sram_2.bin  __sram_image2_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/psram_image2.bin  __psram_image2_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
    COMMAND ${PREPENDTOOL} ${IMAGE_TARGET_FOLDER}/xip_image2.bin  __flash_text_start__  ${IMAGE_TARGET_FOLDER}/target_img2.map
)
execute_process(
    COMMAND ${CMAKE_COMMAND} -E cat ${IMAGE_TARGET_FOLDER}/xip_image2_prepend.bin ${IMAGE_TARGET_FOLDER}/sram_2_prepend.bin ${IMAGE_TARGET_FOLDER}/psram_image2_prepend.bin
    OUTPUT_FILE ${IMAGE_TARGET_FOLDER}/km0_image2_all.bin
)
execute_process(
    COMMAND ${PADTOOL} ${IMAGE_TARGET_FOLDER}/km0_image2_all.bin 4096
)
execute_process(
    COMMAND ${IMAGETOOL} ${IMAGE_TARGET_FOLDER}/km0_image2_all.bin ${BUILD_TYPE}
    WORKING_DIRECTORY ${PROJECTDIR}/..
)


if(CONFIG_FATFS_WITHIN_APP_IMG)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin AND EXISTS ${d_PLATFORM_PROJECT_DIR}/fatfs.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PREPENDTOOL} ${d_PLATFORM_PROJECT_DIR}/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${IMAGE_TARGET_FOLDER}/target_img2.map
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_tmp.bin ${d_PLATFORM_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns_tmp.bin ${d_PLATFORM_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_ns.bin
        )
    elseif(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin AND EXISTS ${d_PLATFORM_PROJECT_DIR}/fatfs.bin)
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E rename ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PADTOOL} ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin 4096
        )
        execute_process(
            COMMAND ${PREPENDTOOL} ${d_PLATFORM_PROJECT_DIR}/fatfs.bin  VFS1_FLASH_BASE_ADDR  ${IMAGE_TARGET_FOLDER}/target_img2.map
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_tmp.bin ${d_PLATFORM_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin
        )
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E cat ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns_tmp.bin ${d_PLATFORM_PROJECT_DIR}/fatfs_prepend.bin
            OUTPUT_FILE ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_ns.bin
        )
    endif()
endif()


if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
    set(_tmp_bin_name km0_km4_app)
elseif(EXISTS ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin)
    set(_tmp_bin_name km0_km4_ca32_app)
else()
    unset(_tmp_bin_name)
endif()
if(CONFIG_UPGRADE_BOOTLOADER)
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin
        )
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}_ns.bin ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_ns.bin
        )
    endif()
else()
    if(EXISTS ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}.bin)
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}.bin
        )
        execute_process(
            COMMAND ${OTAPREPENDTOOL} ${KM4_PROJECT_DIR}/asdk/image/${_tmp_bin_name}_ns.bin
        )
    endif()
endif()

message("========== Image manipulating end ==========")

if(NOT CONFIG_AMEBA_RLS)
    message("========== Image analyze start ==========")
    execute_process(
        COMMAND ${STATIC_ANALYZE_PYTHON} ${DAILY_BUILD}
        WORKING_DIRECTORY ${PROJECTDIR}/asdk
    )
    message("========== Image analyze end ==========")
endif()

if(CONFIG_MP_INCLUDED)
    set(km4_boot_all ${KM4_PROJECT_DIR}/asdk/image_mp/km4_boot_all.bin)
    if(CONFIG_LINUX_FW_EN)
        set(app_all ${KM4_PROJECT_DIR}/asdk/image_mp/km0_km4_app_mp.bin)
    else()
        set(app_all	 ${KM4_PROJECT_DIR}/asdk/image_mp/km0_km4_ca32_app_mp.bin)
    endif()
else()
    set(km4_boot_all ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all.bin)
    set(km4_boot_all_sec ${KM4_PROJECT_DIR}/asdk/image/km4_boot_all_sec.bin)

    if(CONFIG_LINUX_FW_EN)
        set(app_all ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app.bin)
        set(km0_image2_all_sec ${KM4_PROJECT_DIR}/asdk/image/km0_km4_app_sec.bin)
    else()
        set(app_all ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app.bin)
        set(km0_image2_all_sec  ${KM4_PROJECT_DIR}/asdk/image/km0_km4_ca32_app_sec.bin)
    endif()
endif()
set(ota_all  ${KM4_PROJECT_DIR}/asdk/image/ota_all.bin)

if(EXISTS ${km4_boot_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${km4_boot_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(EXISTS ${app_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${app_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(EXISTS ${ota_all})
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy ${ota_all} ${FINAL_IMAGE_DIR}
    )
endif()
if(NOT CONFIG_MP_INCLUDED)
    if(EXISTS ${km4_boot_all_sec})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${km4_boot_all_sec} ${FINAL_IMAGE_DIR}
        )
    endif()
    if(EXISTS ${km0_image2_all_sec})
        execute_process(
            COMMAND ${CMAKE_COMMAND} -E copy ${km0_image2_all_sec} ${FINAL_IMAGE_DIR}
        )
    endif()
endif()