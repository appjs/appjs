#ifndef IMPL_INIT_TYPES_H
#define IMPL_INIT_TYPES_H

#include <gtk/gtk.h>

namespace appjs {
// Avoid unused warnings
inline void use_it (volatile GType* it) {
}

// Force GTK+ to load all types, so GtkBuilder won't be confused because
// of not being able to find types by lazy initilization
inline void force_init_types () {
    volatile GType g;
    g = gtk_about_dialog_get_type();
    g = gtk_accel_group_get_type();
    g = gtk_accel_label_get_type();
    g = gtk_action_get_type();
    g = gtk_action_group_get_type();
    g = gtk_adjustment_get_type();
    g = gtk_alignment_get_type();
    g = gtk_arrow_get_type();
    g = gtk_aspect_frame_get_type();
    g = gtk_assistant_get_type();
    g = gtk_bin_get_type();
    g = gtk_box_get_type();
    g = gtk_builder_get_type();
    g = gtk_button_get_type();
    g = gtk_button_box_get_type();
    g = gtk_calendar_get_type();
    g = gtk_cell_renderer_get_type();
    g = gtk_cell_renderer_accel_get_type();
    g = gtk_cell_renderer_combo_get_type();
    g = gtk_cell_renderer_pixbuf_get_type();
    g = gtk_cell_renderer_progress_get_type();
    g = gtk_cell_renderer_spin_get_type();
    g = gtk_cell_renderer_spinner_get_type();
    g = gtk_cell_renderer_text_get_type();
    g = gtk_cell_renderer_toggle_get_type();
    g = gtk_cell_view_get_type();
    g = gtk_check_button_get_type();
    g = gtk_check_menu_item_get_type();
    g = gtk_clipboard_get_type();
    g = gtk_color_button_get_type();
    g = gtk_color_selection_get_type();
    g = gtk_color_selection_dialog_get_type();
    g = gtk_combo_box_get_type();
    g = gtk_combo_box_text_get_type();
    g = gtk_container_get_type();
    g = gtk_dialog_get_type();
    g = gtk_drawing_area_get_type();
    g = gtk_entry_get_type();
    g = gtk_entry_buffer_get_type();
    g = gtk_entry_completion_get_type();
    g = gtk_event_box_get_type();
    g = gtk_expander_get_type();
    g = gtk_file_chooser_button_get_type();
    g = gtk_file_chooser_dialog_get_type();
    g = gtk_file_chooser_widget_get_type();
    g = gtk_file_filter_get_type();
    g = gtk_fixed_get_type();
    g = gtk_font_button_get_type();
    g = gtk_font_selection_get_type();
    g = gtk_font_selection_dialog_get_type();
    g = gtk_frame_get_type();
    g = gtk_hbox_get_type();
    g = gtk_hbutton_box_get_type();
    g = gtk_hpaned_get_type();
    g = gtk_hscale_get_type();
    g = gtk_hscrollbar_get_type();
    g = gtk_hseparator_get_type();
    g = gtk_handle_box_get_type();
    g = gtk_icon_factory_get_type();
    g = gtk_icon_theme_get_type();
    g = gtk_icon_view_get_type();
    g = gtk_image_get_type();
    g = gtk_image_menu_item_get_type();
    g = gtk_info_bar_get_type();
    g = gtk_invisible_get_type();
    g = gtk_label_get_type();
    g = gtk_layout_get_type();
    g = gtk_link_button_get_type();
    g = gtk_list_store_get_type();
    g = gtk_menu_get_type();
    g = gtk_menu_bar_get_type();
    g = gtk_menu_item_get_type();
    g = gtk_menu_shell_get_type();
    g = gtk_menu_tool_button_get_type();
    g = gtk_message_dialog_get_type();
    g = gtk_misc_get_type();
    g = gtk_notebook_get_type();
    g = gtk_offscreen_window_get_type();
    g = gtk_page_setup_get_type();
    g = gtk_paned_get_type();
    g = gtk_plug_get_type();
    g = gtk_print_context_get_type();
    g = gtk_print_operation_get_type();
    g = gtk_print_settings_get_type();
    g = gtk_progress_bar_get_type();
    g = gtk_radio_action_get_type();
    g = gtk_radio_button_get_type();
    g = gtk_radio_menu_item_get_type();
    g = gtk_radio_tool_button_get_type();
    g = gtk_range_get_type();
    g = gtk_recent_action_get_type();
    g = gtk_recent_chooser_dialog_get_type();
    g = gtk_recent_chooser_menu_get_type();
    g = gtk_recent_chooser_widget_get_type();
    g = gtk_recent_filter_get_type();
    g = gtk_recent_manager_get_type();
    g = gtk_scale_get_type();
    g = gtk_scale_button_get_type();
    g = gtk_scrollbar_get_type();
    g = gtk_scrolled_window_get_type();
    g = gtk_separator_get_type();
    g = gtk_separator_menu_item_get_type();
    g = gtk_separator_tool_item_get_type();
    g = gtk_settings_get_type();
    g = gtk_size_group_get_type();
    g = gtk_socket_get_type();
    g = gtk_spin_button_get_type();
    g = gtk_spinner_get_type();
    g = gtk_status_icon_get_type();
    g = gtk_statusbar_get_type();
    g = gtk_table_get_type();
    g = gtk_tearoff_menu_item_get_type();
    g = gtk_text_buffer_get_type();
    g = gtk_text_child_anchor_get_type();
    g = gtk_text_mark_get_type();
    g = gtk_text_tag_get_type();
    g = gtk_text_tag_table_get_type();
    g = gtk_text_view_get_type();
    g = gtk_toggle_action_get_type();
    g = gtk_toggle_button_get_type();
    g = gtk_toggle_tool_button_get_type();
    g = gtk_tool_button_get_type();
    g = gtk_tool_item_get_type();
    g = gtk_tool_item_group_get_type();
    g = gtk_tool_palette_get_type();
    g = gtk_toolbar_get_type();
    g = gtk_tooltip_get_type();
    g = gtk_tree_model_filter_get_type();
    g = gtk_tree_model_sort_get_type();
    g = gtk_tree_selection_get_type();
    g = gtk_tree_store_get_type();
    g = gtk_tree_view_get_type();
    g = gtk_tree_view_column_get_type();
    g = gtk_ui_manager_get_type();
    g = gtk_vbox_get_type();
    g = gtk_vbutton_box_get_type();
    g = gtk_vpaned_get_type();
    g = gtk_vscale_get_type();
    g = gtk_vscrollbar_get_type();
    g = gtk_vseparator_get_type();
    g = gtk_viewport_get_type();
    g = gtk_volume_button_get_type();
    g = gtk_widget_get_type();
    g = gtk_window_get_type();
    g = gtk_window_group_get_type();

    use_it (&g);
}
} /* appjs */

#endif /* end of IMPL_INIT_TYPES_H */
