ameba_internal_library(example_raw_i2c_int_master)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_i2c_int_master.c
    app_example.c
)
