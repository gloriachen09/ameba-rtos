##########################################################################################
## * This part defines public part of the component
## * Public part will be used as global build configures for all component

set(public_includes)                #public include directories, NOTE: relative path is OK
set(public_definitions)             #public definitions
set(public_libraries)               #public libraries(files), NOTE: linked with whole-archive options

#----------------------------------------#
# Component public part, user config begin

set(TARGET_LIB wifi_api)
# You may use if-else condition to set or update predefined variable above
ameba_list_append(public_includes
    ${CMAKE_CURRENT_SOURCE_DIR}
)
if(NOT (CONFIG_WHC_BRIDGE AND CONFIG_WPA_STD))
ameba_list_append(public_includes
    ${c_CMPT_WIFI_DIR}/wpa_supplicant/src
)
endif()
if(CONFIG_ZEPHYR_SDK AND CONFIG_WHC_NONE)
    ameba_list_append(public_libraries
        ${c_SDK_LIB_APPLICATION_DIR}/lib_${TARGET_LIB}.a
    )
endif()
# Component public part, user config end
#----------------------------------------#

#WARNING: Fixed section, DO NOT change!
ameba_global_include(${public_includes})
ameba_global_define(${public_defines})
ameba_global_library(${public_libraries}) #default: whole-archived

##########################################################################################
## * This part defines private part of the component
## * Private part is used to build target of current component
## * NOTE: The build API guarantees the global build configures(mentioned above)
## *       applied to the target automatically. So if any configure was already added
## *       to public above, it's unnecessary to add again below.

#NOTE: User defined section, add your private build configures here
# You may use if-else condition to set these predefined variable
# They are only for ameba_add_internal_library/ameba_add_external_app_library/ameba_add_external_soc_library
set(private_sources)                 #private source files, NOTE: relative path is OK
set(private_includes)                #private include directories, NOTE: relative path is OK
set(private_definitions)             #private definitions
set(private_compile_options)         #private compile_options

#------------------------------#
# Component private part, user config begin

ameba_list_append(private_sources
    ${c_CMPT_USRCFG_DIR}/ameba_wificfg.c
)

if(CONFIG_WHC_HOST)
    ameba_list_append(private_sources
        ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant/wifi_eap_config.c
        ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant/wifi_wps_config.c
        ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/tls_polarssl.c
    )
elseif(CONFIG_WHC_NONE)
    if(NOT (CONFIG_WHC_BRIDGE AND CONFIG_WPA_STD))
        ameba_list_append(private_sources
            ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant/wifi_eap_config.c
            ${c_CMPT_WIFI_DIR}/wpa_supplicant/wpa_supplicant/wifi_wps_config.c
            ${c_CMPT_WIFI_DIR}/wpa_supplicant/src/crypto/tls_polarssl.c
        )
    endif()
endif()

if(NOT CONFIG_WHC_HOST)
    ameba_list_append(private_sources
        ${c_CMPT_USRCFG_DIR}/ameba_wifi_power_table_usrcfg.c
        ${c_CMPT_USRCFG_DIR}/ameba_wifi_country_code_table_usrcfg.c
    )
    ameba_list_append_if(CONFIG_WIFI_POWER_TABLE_USRCFG_1v8 private_sources
        ${c_CMPT_USRCFG_DIR}/ameba_wifi_power_table_usrcfg_1v8.c
    )

    ameba_list_append_if(CONFIG_WIFI_POWER_TABLE_USRCFG_3v3_1v25 private_sources
        ${c_CMPT_USRCFG_DIR}/ameba_wifi_power_table_usrcfg_3v3_1v25.c
    )
endif()

# Component private part, user config end
#------------------------------#

#WARNING: Select right API based on your component's release/not-release/standalone

if(CONFIG_ZEPHYR_SDK AND CONFIG_WHC_NONE)
###NOTE: For closed-source component, only build before release and libs are packaged into lib/application
    ameba_add_external_app_library(${TARGET_LIB}
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
else()
###NOTE: For open-source component, always build from source
    ameba_add_internal_library(${TARGET_LIB}
        p_DROP_IF_NO_SOURCES
        p_SOURCES
            ${private_sources}
        p_INCLUDES
            ${private_includes}
        p_DEFINITIONS
            ${private_definitions}
        p_COMPILE_OPTIONS
            ${private_compile_options}
    )
endif()



##########################################################################################