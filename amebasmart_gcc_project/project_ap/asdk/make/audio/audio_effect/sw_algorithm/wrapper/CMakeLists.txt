ameba_target_add(audio_effect_sw_algorithm_wrapper
    p_TYPE interface
    p_WRAP_TARGET_NAME
    p_PREFIX lib_
    p_LINK_LIBRARY
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_INCLUDE_DIRECTORIES
        d_PROJ_${d_MCU_PROJECT_NAME_UPPER}_LINK_OPTIONS
        src_${d_MCU_PROJECT_NAME}_audio_effect_sw_algorithm_wrapper
    p_LINKED_BY_LIBRARIES
        src_${d_MCU_PROJECT_NAME}_audio_xip_local
    p_LINKED_BY_SCOPE interface
    p_DEPENDENTED_BY_TARGETS
        dep_${d_MCU_PROJECT_NAME}_audio
)
