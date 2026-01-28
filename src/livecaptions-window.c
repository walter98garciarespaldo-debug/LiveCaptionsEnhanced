/* livecaptions-window.c
 *
 * Copyright 2022 abb128
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "livecaptions-config.h"
#include "livecaptions-window.h"
#include "livecaptions-application.h"
#include "april_api.h"
#include "audiocap.h"
#include "window-helper.h"
#include "history.h"

// ======================================================================
// ========== FUNCIÓN: MANTENER SCROLL ABAJO ============================
// ======================================================================

static void on_adjustment_changed(GtkAdjustment *adjustment, gpointer user_data) {
    LiveCaptionsWindow *self = user_data;

    // Bloqueamos la señal para evitar bucles infinitos al cambiar el valor
    g_signal_handler_block(adjustment, self->scroll_handler_id);
    double upper = gtk_adjustment_get_upper(adjustment);
    double page_size = gtk_adjustment_get_page_size(adjustment);
    gtk_adjustment_set_value(adjustment, upper - page_size);
    g_signal_handler_unblock(adjustment, self->scroll_handler_id);
}

// ======================================================================
// ========== FUNCIÓN: FORZAR SCROLL ABAJO AL ABRIR =====================
// ======================================================================

static void force_scroll_bottom(LiveCaptionsWindow *self) {
    if (!self || !self->scroll_adjustment)
        return;
    double upper = gtk_adjustment_get_upper(self->scroll_adjustment);
    double page_size = gtk_adjustment_get_page_size(self->scroll_adjustment);
    gtk_adjustment_set_value(self->scroll_adjustment, upper - page_size);
}

static void on_scrolled_window_map(GtkWidget *widget, gpointer user_data) {
    LiveCaptionsWindow *self = user_data;
    // Forzar el scroll al fondo cuando el widget se muestre en pantalla
    g_idle_add_full(G_PRIORITY_DEFAULT_IDLE, (GSourceFunc)force_scroll_bottom, self, NULL);
}

// ======================================================================
// ========== DEFINICIÓN DE CLASE GTK ===================================
// ======================================================================

G_DEFINE_TYPE(LiveCaptionsWindow, livecaptions_window, GTK_TYPE_APPLICATION_WINDOW)

static void livecaptions_window_class_init(LiveCaptionsWindowClass *klass) {
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

    gtk_widget_class_set_template_from_resource(widget_class, "/net/waltergarcia/LiveCaptions/livecaptions-window.ui");
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, main);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, scrolled_window);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, side_box);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, side_box_tiny);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, mic_button);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, label);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, too_slow_warning);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, slow_warning);
    gtk_widget_class_bind_template_child(widget_class, LiveCaptionsWindow, slowest_warning);
}

// ======================================================================
// ========== FUNCIÓN: CAMBIO DE LAYOUT DE BOTONES ======================
// ======================================================================

static void change_button_layout(LiveCaptionsWindow *self, gint text_height) {
    int button_height = 45; // Actualizado según el nuevo diseño

    if (text_height > (2 * button_height)) {
        gtk_widget_set_visible(GTK_WIDGET(self->side_box), true);
        gtk_widget_set_visible(GTK_WIDGET(self->side_box_tiny), false);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(self->side_box_tiny), true);
        gtk_widget_set_visible(GTK_WIDGET(self->side_box), false);
    }
}

// ======================================================================
// ========== FUNCIÓN: ANCHO DE LÍNEA ===================================
// ======================================================================

const char LINE_WIDTH_TEXT_TEMPLATE[] =
    "This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version. This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.";

static void update_line_width(LiveCaptionsWindow *self) {
    if (self->font_layout != NULL)
        g_object_unref(self->font_layout);

    int preferred_width = g_settings_get_int(self->settings, "line-width");
    size_t text_len = sizeof(LINE_WIDTH_TEXT_TEMPLATE);
    if (preferred_width < text_len)
        text_len = preferred_width;

    int width, height;
    PangoLayout *layout = gtk_label_get_layout(self->label);
    layout = pango_layout_copy(layout);

    pango_layout_set_width(layout, -1);
    pango_layout_set_text(layout, LINE_WIDTH_TEXT_TEMPLATE, text_len);
    pango_layout_get_size(layout, &width, &height);

    int line_height = height / PANGO_SCALE;
    height = line_height * 4; // Reducido de 10 a 4 para ser más compacto
    width = (width / PANGO_SCALE);
    change_button_layout(self, height);

    // SVP Compensation: Add 68px to account for sidebar + margins + scrollbar
    gtk_widget_set_size_request(GTK_WIDGET(self->scrolled_window), width + 68, -1);
    gtk_scrolled_window_set_min_content_height(self->scrolled_window, height);

    self->max_text_width = width;

    self->font_layout = layout;
    self->font_layout_counter++;
}

// ======================================================================
// ========== FUNCIÓN: FUENTE ===========================================
// ======================================================================

static void update_font(LiveCaptionsWindow *self) {
    PangoFontDescription *desc = pango_font_description_from_string(g_settings_get_string(self->settings, "font-name"));
    PangoAttribute *attr_font = pango_attr_font_desc_new(desc);

    PangoAttrList *attr = gtk_label_get_attributes(self->label);
    if (attr == NULL) {
        attr = pango_attr_list_new();
    }
    pango_attr_list_change(attr, attr_font);

    gtk_label_set_attributes(self->label, attr);

    pango_font_description_free(desc);
    update_line_width(self);
}

// ======================================================================
// ========== FUNCIÓN: TRANSPARENCIA ====================================
// ======================================================================

static void update_window_transparency(LiveCaptionsWindow *self) {
    if (!self || !self->settings || !self->css_provider) return;

    bool use_transparency = g_settings_get_double(self->settings, "window-transparency") > 0.01;

    if (use_transparency) {
        gtk_widget_add_css_class(GTK_WIDGET(self), "transparent-mode");
        int transparency = (int)((1.0 - g_settings_get_double(self->settings, "window-transparency")) * 255.0);

        char css_data[256];
        snprintf(css_data, 256, ".transparent-mode { background-color: rgba(0,0,0,%f); }", g_settings_get_double(self->settings, "window-transparency"));
        
        // No sobreescribimos el proveedor principal, usamos uno local para la transparencia dinámica
        gtk_css_provider_load_from_string(self->css_provider, css_data);
    } else {
        gtk_widget_remove_css_class(GTK_WIDGET(self), "transparent-mode");
    }
}

// ======================================================================
// ========== FUNCIÓN: KEEP ABOVE =======================================
// ======================================================================

static void update_keep_above(LiveCaptionsWindow *self) {
    gboolean keep_above = g_settings_get_boolean(self->settings, "keep-on-top");
    if (!set_window_keep_above(GTK_WINDOW(self), keep_above)) {
        printf("Failed to set keep above\n");
    }
}

// ======================================================================
// ========== CALLBACK DE SETTINGS ======================================
// ======================================================================

static void on_settings_change(G_GNUC_UNUSED GSettings *settings, char *key, gpointer user_data) {
    LiveCaptionsWindow *self = user_data;
    if (g_str_equal(key, "font-name")) {
        update_font(self);
    } else if (g_str_equal(key, "line-width")) {
        update_line_width(self);
    } else if (g_str_equal(key, "transparent-window") || g_str_equal(key, "window-transparency")) {
        update_window_transparency(self);
    } else if (g_str_equal(key, "keep-on-top")) {
        update_keep_above(self);
    }
}

static gboolean deferred_update_keep_above(void *userdata) {
    LiveCaptionsWindow *self = userdata;
    update_keep_above(self);
    return G_SOURCE_REMOVE;
}

// ======================================================================
// ========== ADVERTENCIA DE LENTITUD ===================================
// ======================================================================

static gboolean show_relevant_slow_warning(void *userdata) {
    LiveCaptionsWindow *self = userdata;

    GtkApplication *curr_app = gtk_window_get_application(GTK_WINDOW(self));
    if (!curr_app) return G_SOURCE_CONTINUE;
    
    LiveCaptionsApplication *app = LIVECAPTIONS_APPLICATION(curr_app);
    asr_thread asr = app->asr;
    if (!asr) return G_SOURCE_CONTINUE;

    if (asr_thread_is_errored(asr)) {

        gtk_label_set_text(self->label, "[Model Error]");
        self->was_errored = true;
        return G_SOURCE_CONTINUE;
    } else if (self->was_errored) {
        self->was_errored = false;
        gtk_label_set_text(self->label, "");
    }

    AprilASRSession session = (AprilASRSession)asr_thread_get_session(asr);
    if (session == NULL)
        return G_SOURCE_CONTINUE;

    float speedup = aas_realtime_get_speedup(session);

    if (speedup <= 1.1) {
        gtk_widget_set_visible(GTK_WIDGET(self->slow_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->slowest_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), false);
    } else if (speedup <= 1.666) {
        gtk_widget_set_visible(GTK_WIDGET(self->slow_warning), true);
        gtk_widget_set_visible(GTK_WIDGET(self->slowest_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), false);
    } else if (speedup <= 2.33) {
        gtk_widget_set_visible(GTK_WIDGET(self->slow_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->slowest_warning), true);
        gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), false);
    } else {
        gtk_widget_set_visible(GTK_WIDGET(self->slow_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->slowest_warning), false);
        gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), true);
    }

    return G_SOURCE_CONTINUE;
}

// ======================================================================
// ========== INICIALIZACIÓN PRINCIPAL ==================================
// ======================================================================

static void livecaptions_window_init(LiveCaptionsWindow *self) {
    gtk_widget_init_template(GTK_WIDGET(self));

    self->settings = g_settings_new("net.waltergarcia.LiveCaptions");
    self->scroll_adjustment = gtk_scrolled_window_get_vadjustment(self->scrolled_window);

    // Mantener scroll al fondo durante actualizaciones (GTK4 Way)
    self->scroll_handler_id =
        g_signal_connect(self->scroll_adjustment, "changed", G_CALLBACK(on_adjustment_changed), self);

    // Forzar scroll al fondo al mostrar la ventana por primera vez
    g_signal_connect(self->scrolled_window, "map", G_CALLBACK(on_scrolled_window_map), self);

    self->css_provider = gtk_css_provider_new();
    GtkStyleContext *context = gtk_widget_get_style_context(GTK_WIDGET(self));
    gtk_style_context_add_provider(context, GTK_STYLE_PROVIDER(self->css_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    g_signal_connect(self->settings, "changed", G_CALLBACK(on_settings_change), self);
    g_settings_bind(self->settings, "microphone", self->mic_button, "active", G_SETTINGS_BIND_DEFAULT);


    self->font_layout = NULL;
    self->font_layout_counter = 0;

    update_font(self);
    update_window_transparency(self);

    self->slow_warning_shown = false;
    self->was_errored = false;

    g_idle_add(deferred_update_keep_above, self);
    g_timeout_add_seconds(5, show_relevant_slow_warning, self);

    gtk_widget_remove_css_class(GTK_WIDGET(self), "solid-csd");
}

// ======================================================================
// ========== ADVERTENCIA TEMPORAL ======================================
// ======================================================================

static gboolean hide_slow_warning_after_some_time(void *userdata) {
    LiveCaptionsWindow *self = userdata;
    time_t current_time = time(NULL);

    if (difftime(current_time, self->slow_time) > 4.0) {
        self->slow_warning_shown = false;
        gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), false);
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

void livecaptions_window_warn_slow(LiveCaptionsWindow *self) {
    self->slow_time = time(NULL);
    if (self->slow_warning_shown)
        return;

    gtk_widget_set_visible(GTK_WIDGET(self->too_slow_warning), true);
    self->slow_warning_shown = true;

    g_timeout_add_seconds(1, hide_slow_warning_after_some_time, self);
}
