ameba_internal_library(example_raw_spi_display_ili9341)

target_sources(
    ${CURRENT_LIB_NAME} PRIVATE
    example_raw_spi_display_ili9341.c
    app_example.c
)
