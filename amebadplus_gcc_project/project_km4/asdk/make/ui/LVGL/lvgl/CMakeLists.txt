set(DIR ${BASEDIR}/component/ui/LVGL/lvgl)

set(
    MODULE_IFLAGS
    ${BASEDIR}/component/ui/LVGL/port/amebadplus
	${DIR}/
	${DIR}/src
	${DIR}/src/core
	${DIR}/src/draw
	${DIR}/src/extra
	${DIR}/src/font
	${DIR}/src/hal
	${DIR}/src/misc
	${DIR}/src/widgets
)

set(MODULE_C_DEFINES LV_CONF_INCLUDE_SIMPLE)

set(
    CSRC
    ${DIR}/src/core/lv_disp.c
	${DIR}/src/core/lv_event.c
	${DIR}/src/core/lv_group.c
	${DIR}/src/core/lv_indev.c
	${DIR}/src/core/lv_indev_scroll.c
	${DIR}/src/core/lv_obj.c
	${DIR}/src/core/lv_obj_class.c
	${DIR}/src/core/lv_obj_draw.c
	${DIR}/src/core/lv_obj_pos.c
	${DIR}/src/core/lv_obj_scroll.c
	${DIR}/src/core/lv_obj_style.c
	${DIR}/src/core/lv_obj_style_gen.c
	${DIR}/src/core/lv_obj_tree.c
	${DIR}/src/core/lv_refr.c
	${DIR}/src/core/lv_theme.c

    ${DIR}/src/draw/lv_draw_arc.c
	${DIR}/src/draw/lv_draw.c
	${DIR}/src/draw/lv_draw_img.c
	${DIR}/src/draw/lv_draw_label.c
	${DIR}/src/draw/lv_draw_line.c
	${DIR}/src/draw/lv_draw_mask.c
	${DIR}/src/draw/lv_draw_rect.c
	${DIR}/src/draw/lv_draw_transform.c
	${DIR}/src/draw/lv_draw_layer.c
	${DIR}/src/draw/lv_draw_triangle.c
	${DIR}/src/draw/lv_img_buf.c
	${DIR}/src/draw/lv_img_cache.c
	${DIR}/src/draw/lv_img_decoder.c
	${DIR}/src/draw/sw/lv_draw_sw.c
	${DIR}/src/draw/sw/lv_draw_sw_arc.c
	${DIR}/src/draw/sw/lv_draw_sw_blend.c
	${DIR}/src/draw/sw/lv_draw_sw_dither.c
	${DIR}/src/draw/sw/lv_draw_sw_gradient.c
	${DIR}/src/draw/sw/lv_draw_sw_img.c
	${DIR}/src/draw/sw/lv_draw_sw_letter.c
	${DIR}/src/draw/sw/lv_draw_sw_line.c
	${DIR}/src/draw/sw/lv_draw_sw_polygon.c
	${DIR}/src/draw/sw/lv_draw_sw_rect.c
	${DIR}/src/draw/sw/lv_draw_sw_transform.c
	${DIR}/src/draw/sw/lv_draw_sw_layer.c

    ${DIR}/src/extra/lv_extra.c
	${DIR}/src/extra/layouts/flex/lv_flex.c
	${DIR}/src/extra/layouts/grid/lv_grid.c
	${DIR}/src/extra/themes/basic/lv_theme_basic.c
	${DIR}/src/extra/themes/default/lv_theme_default.c
	${DIR}/src/extra/themes/mono/lv_theme_mono.c
	${DIR}/src/extra/widgets/animimg/lv_animimg.c
	${DIR}/src/extra/widgets/chart/lv_chart.c
	${DIR}/src/extra/widgets/imgbtn/lv_imgbtn.c
	${DIR}/src/extra/widgets/led/lv_led.c
	${DIR}/src/extra/widgets/msgbox/lv_msgbox.c
	${DIR}/src/extra/widgets/spinbox/lv_spinbox.c
	${DIR}/src/extra/widgets/tabview/lv_tabview.c
	${DIR}/src/extra/widgets/win/lv_win.c
	${DIR}/src/extra/widgets/calendar/lv_calendar.c
	${DIR}/src/extra/widgets/calendar/lv_calendar_header_arrow.c
	${DIR}/src/extra/widgets/calendar/lv_calendar_header_dropdown.c
	${DIR}/src/extra/widgets/colorwheel/lv_colorwheel.c
	${DIR}/src/extra/widgets/keyboard/lv_keyboard.c
	${DIR}/src/extra/widgets/list/lv_list.c
	${DIR}/src/extra/widgets/menu/lv_menu.c
	${DIR}/src/extra/widgets/meter/lv_meter.c
	${DIR}/src/extra/widgets/span/lv_span.c
	${DIR}/src/extra/widgets/spinner/lv_spinner.c
	${DIR}/src/extra/widgets/tileview/lv_tileview.c

    # ${DIR}/src/font/lv_extra.c
	${DIR}/src/font/lv_font.c
	${DIR}/src/font/lv_font_dejavu_16_persian_hebrew.c
	${DIR}/src/font/lv_font_fmt_txt.c
	${DIR}/src/font/lv_font_loader.c
	${DIR}/src/font/lv_font_montserrat_10.c
	${DIR}/src/font/lv_font_montserrat_12.c
	${DIR}/src/font/lv_font_montserrat_12_subpx.c
	${DIR}/src/font/lv_font_montserrat_14.c
	${DIR}/src/font/lv_font_montserrat_16.c
	${DIR}/src/font/lv_font_montserrat_18.c
	${DIR}/src/font/lv_font_montserrat_20.c
	${DIR}/src/font/lv_font_montserrat_22.c
	${DIR}/src/font/lv_font_montserrat_24.c
	${DIR}/src/font/lv_font_montserrat_26.c
	${DIR}/src/font/lv_font_montserrat_28.c
	${DIR}/src/font/lv_font_montserrat_28_compressed.c
	${DIR}/src/font/lv_font_montserrat_30.c
	${DIR}/src/font/lv_font_montserrat_32.c
	${DIR}/src/font/lv_font_montserrat_34.c
	${DIR}/src/font/lv_font_montserrat_36.c
	${DIR}/src/font/lv_font_montserrat_38.c
	${DIR}/src/font/lv_font_montserrat_40.c
	${DIR}/src/font/lv_font_montserrat_42.c
	${DIR}/src/font/lv_font_montserrat_44.c
	${DIR}/src/font/lv_font_montserrat_46.c
	${DIR}/src/font/lv_font_montserrat_48.c
	${DIR}/src/font/lv_font_montserrat_8.c
	${DIR}/src/font/lv_font_simsun_16_cjk.c
	${DIR}/src/font/lv_font_unscii_16.c
	${DIR}/src/font/lv_font_unscii_8.c

    ${DIR}/src/hal/lv_hal_disp.c
	${DIR}/src/hal/lv_hal_indev.c
	${DIR}/src/hal/lv_hal_tick.c

    ${DIR}/src/misc/lv_anim.c
	${DIR}/src/misc/lv_anim_timeline.c
	${DIR}/src/misc/lv_area.c
	${DIR}/src/misc/lv_async.c
	${DIR}/src/misc/lv_bidi.c
	${DIR}/src/misc/lv_color.c
	${DIR}/src/misc/lv_fs.c
	${DIR}/src/misc/lv_gc.c
	${DIR}/src/misc/lv_ll.c
	${DIR}/src/misc/lv_log.c
	${DIR}/src/misc/lv_math.c
	${DIR}/src/misc/lv_mem.c
	${DIR}/src/misc/lv_printf.c
	${DIR}/src/misc/lv_style.c
	${DIR}/src/misc/lv_style_gen.c
	${DIR}/src/misc/lv_timer.c
	${DIR}/src/misc/lv_tlsf.c
	${DIR}/src/misc/lv_txt_ap.c
	${DIR}/src/misc/lv_txt.c
	${DIR}/src/misc/lv_utils.c

    ${DIR}/src/widgets/lv_arc.c
	${DIR}/src/widgets/lv_bar.c
	${DIR}/src/widgets/lv_btn.c
	${DIR}/src/widgets/lv_btnmatrix.c
	${DIR}/src/widgets/lv_canvas.c
	${DIR}/src/widgets/lv_checkbox.c
	${DIR}/src/widgets/lv_dropdown.c
	${DIR}/src/widgets/lv_img.c
	${DIR}/src/widgets/lv_label.c
	${DIR}/src/widgets/lv_line.c
	${DIR}/src/widgets/lv_roller.c
	${DIR}/src/widgets/lv_slider.c
	${DIR}/src/widgets/lv_switch.c
	${DIR}/src/widgets/lv_table.c
	${DIR}/src/widgets/lv_textarea.c

    ${DIR}/demos/stress/lv_demo_stress.c
	${DIR}/demos/benchmark/lv_demo_benchmark.c
	${DIR}/demos/benchmark/assets/img_benchmark_cogwheel_argb.c
	${DIR}/demos/benchmark/assets/img_benchmark_cogwheel_rgb.c
	${DIR}/demos/benchmark/assets/img_benchmark_cogwheel_chroma_keyed.c
	${DIR}/demos/benchmark/assets/img_benchmark_cogwheel_indexed16.c
	${DIR}/demos/benchmark/assets/img_benchmark_cogwheel_alpha16.c
	${DIR}/demos/benchmark/assets/lv_font_bechmark_montserrat_12_compr_az.c.c
	${DIR}/demos/benchmark/assets/lv_font_bechmark_montserrat_16_compr_az.c.c
	${DIR}/demos/benchmark/assets/lv_font_bechmark_montserrat_28_compr_az.c.c

    ${DIR}/demos/widgets/lv_demo_widgets.c
	${DIR}/demos/widgets/assets/img_clothes.c
	${DIR}/demos/widgets/assets/img_demo_widgets_avatar.c
	${DIR}/demos/widgets/assets/img_lvgl_logo.c

    ${DIR}/examples/anim/lv_example_anim_1.c
	${DIR}/examples/anim/lv_example_anim_2.c
	${DIR}/examples/anim/lv_example_anim_3.c
	${DIR}/examples/anim/lv_example_anim_timeline_1.c
)

ameba_internal_library(lvgl)
target_compile_definitions(${CURRENT_LIB_NAME} PRIVATE ${MODULE_C_DEFINES})
target_compile_options(${CURRENT_LIB_NAME} PRIVATE -Wno-error)
target_include_directories(${CURRENT_LIB_NAME} PRIVATE ${MODULE_IFLAGS})
target_sources(${CURRENT_LIB_NAME} PRIVATE ${CSRC})

add_custom_command(TARGET ${CURRENT_LIB_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy lib_${CURRENT_LIB_NAME}.a ${TARGET_LIBAPP_FOLDER}/lib_lvgl.a
    COMMAND ${CMAKE_OBJCOPY} -g -D ${TARGET_LIBAPP_FOLDER}/lib_lvgl.a
)
