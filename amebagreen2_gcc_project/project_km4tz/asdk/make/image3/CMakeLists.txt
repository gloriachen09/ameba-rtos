ameba_add_image(target_img3 p_IMAGE_ALL ${c_MCU_PROJECT_NAME}_image3_all.bin)
# Use variable c_CURRENT_IMAGE to represent full image target name

###############################  ADD COMPONENT  ###################################

ameba_add_subdirectory(${c_CMPT_CHIPINFO_DIR})
ameba_add_subdirectory(${c_CMPT_FWLIB_DIR})
ameba_add_subdirectory(${c_CMPT_IMG3_DIR})
ameba_add_subdirectory(${c_CMPT_OS_DIR})
ameba_add_subdirectory_if_exist(${c_CMPT_SOC_DIR}/verification)

###############################  GENERATE IMAGE  ###################################
#---------------- PRE LINK -------------------#
add_custom_command(
  TARGET ${c_CURRENT_IMAGE} PRE_LINK
  COMMAND ${CMAKE_C_COMPILER} -E -P -xc -c ${c_IMG3_LD} -o ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld -I ${c_MENUCONFIG_DIR}
  COMMAND ${CMAKE_COMMAND} -E cat ${c_LINK_ROM_SYMBOL_S} >> ${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
  COMMENT "build img3 linkerscript rlx8721d.ld"
)

#----------------- LINKING -------------------#
target_link_libraries(${c_CURRENT_IMAGE}
    PRIVATE
    -T${CMAKE_CURRENT_BINARY_DIR}/rlx8721d.ld
    ${c_MCU_PROJ_CONFIG}
    -Wl,--gc-sections
    -Wl,-Map=${CMAKE_CURRENT_BINARY_DIR}/text.map

    -Wl,--out-implib=${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib
    -Wl,--cmse-implib

    -Wl,--whole-archive
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_libraries>
    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_whole_archive_libs>
    -Wl,--no-whole-archive

    $<TARGET_PROPERTY:${c_MCU_PROJ_CONFIG},${c_CURRENT_IMAGE}_no_whole_archive_libs>
    ${c_LINK_ROM_LIB}
)

#--------------- POST BUILD ------------------#
if(TARGET target_img2_km4tz_postbuild)
    #avoid making bin files in parallel
    set(DEP_KM4TZ target_img2_km4tz_postbuild)
endif()

add_custom_command(
    OUTPUT ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.lib ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf
    COMMAND ${CMAKE_COMMAND} -E make_directory ${c_SDK_IMAGE_TARGET_DIR}
    COMMAND ${CMAKE_COMMAND} -E touch ${c_SDK_IMAGE_TARGET_DIR}/.gitignore
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/cmse_implib.lib ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.lib
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/text.map ${c_SDK_IMAGE_TARGET_DIR}/text_image3.map
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_BINARY_DIR}/${c_CURRENT_IMAGE}.axf ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf
    COMMAND ${CMAKE_NM} ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf | sort > ${c_SDK_IMAGE_TARGET_DIR}/target_img3.map
    COMMAND ${CMAKE_OBJDUMP} -d ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf > ${c_SDK_IMAGE_TARGET_DIR}/target_img3.asm
    COMMAND ${CMAKE_COMMAND} -E copy ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img3.axf
    COMMAND ${CMAKE_STRIP}  ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img3.axf

    COMMAND ${CMAKE_READELF} -s ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.lib > ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.txt
    COMMAND ${CMAKE_OBJDUMP} -j .gnu.sgstubs -d ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf > ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.asm

    COMMAND ${CMAKE_COMMAND} -E echo "  BIN      ${c_MCU_PROJECT_NAME}_image3_all.bin"
    COMMAND ${CMAKE_OBJCOPY}  -j .ram_image3.text -j .ram_image3.data -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img3.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_3_s.bin
    COMMAND ${CMAKE_OBJCOPY}  -j .gnu.sgstubs -j .ram_image3.entry -Obinary ${c_SDK_IMAGE_TARGET_DIR}/target_pure_img3.axf ${c_SDK_IMAGE_TARGET_DIR}/ram_3_nsc.bin

    COMMAND ${CMAKE_COMMAND} -E echo "========== Image manipulating start =========="
    COMMAND ${op_PAD} -i ${c_SDK_IMAGE_TARGET_DIR}/ram_3_s.bin -l 32
    COMMAND ${op_PAD} -i ${c_SDK_IMAGE_TARGET_DIR}/ram_3_nsc.bin -l 32

    COMMAND ${op_PREPEND_HEADER}
        -o ${c_SDK_IMAGE_TARGET_DIR}/ram_3_s_prepend.bin
        -i ${c_SDK_IMAGE_TARGET_DIR}/ram_3_s.bin
        -s __ram_image3_text_start__
        -m ${c_SDK_IMAGE_TARGET_DIR}/target_img3.map
    COMMAND ${op_PREPEND_HEADER}
        -o ${c_SDK_IMAGE_TARGET_DIR}/ram_3_nsc_prepend.bin
        -i ${c_SDK_IMAGE_TARGET_DIR}/ram_3_nsc.bin
        -s __ram_image3_nsc_start__
        -m ${c_SDK_IMAGE_TARGET_DIR}/target_img3.map

    COMMAND ${CMAKE_COMMAND} -E cat ${c_SDK_IMAGE_TARGET_DIR}/ram_3_s_prepend.bin ${c_SDK_IMAGE_TARGET_DIR}/ram_3_nsc_prepend.bin > ${c_SDK_IMAGE_TARGET_DIR}/${c_MCU_PROJECT_NAME}_image3_all.bin

    DEPENDS ${c_CURRENT_IMAGE}
    WORKING_DIRECTORY ${c_SOC_PROJECT_DIR}
)

add_custom_target(
    ${c_CURRENT_IMAGE}_postbuild ALL
    DEPENDS ${c_SDK_IMAGE_TARGET_DIR}/cmse_implib.lib ${c_SDK_IMAGE_TARGET_DIR}/target_img3.axf
)

#------------- EXTRA PROCESS -----------------#
set(clean_files
    ${c_SDK_IMAGE_TARGET_DIR}
)

set(link_deps
    ${c_LAYOUT_LD}
    ${c_IMG3_LD}
    ${c_LINK_ROM_SYMBOL_S}
)

set_target_properties(${c_CURRENT_IMAGE} PROPERTIES
    #WARNING: values for these parameter MUST be placed inside a pair of quotes
    ADDITIONAL_CLEAN_FILES
        "${clean_files}"
    LINK_DEPENDS
        "${link_deps}"
)

################################  CUSTOM TARGET  ###################################

add_custom_target(
    image3
    COMMENT "build img3 only"
    DEPENDS ${c_CURRENT_IMAGE}
)
