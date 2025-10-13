/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/ui/resources/ustudio_resources.c
 *
 * PURPOSE:
 *   Pure-C resource loader (no glib-compile-resources). In "no-CSS" mode,
 *   the app uses only platform/native styling (no external stylesheets).
 *
 * DESIGN:
 *   - Build-time switch: USIDE_DISABLE_CSS
 *       1 = never load CSS; stick to native theme
 *       0 = allow loading GTK CSS from disk, with safe fallbacks
 *   - All helpers are defensive: NULL-safe, clean error handling.
 *   - No cross-module includes; public API remains small and stable.
 *
 * API:
 *   void        umi_res_set_root(const char *root_dir);
 *   const char *umi_res_get_root(void);
 *   gchar      *umi_res_load_text_file(const char *rel, gsize *len, GError **err);
 *   gboolean    umi_res_load_css(GtkCssProvider *prov, GError **err); // no-op if disabled
 *   gboolean    umi_res_builder_add(GtkBuilder *b, const char *rel_or_res, GError **err);
 *   GtkBuilder *umi_builder_new_from_res_or_file(const char *rel_or_res, GError **err);
 *   const char *umi_res_fallback_json_theme_presets(void);
 *
 * SECURITY/ROBUSTNESS:
 *   - All disk I/O goes through GLib (safe UTF-8 paths, error paths).
 *   - Fallbacks are embedded strings; never trust external data blindly.
 *
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>

#ifndef USIDE_RESOURCE_ROOT
#define USIDE_RESOURCE_ROOT "src/ui/resources"
#endif

static char g_res_root[1024] = USIDE_RESOURCE_ROOT;

/*----------------------------- Embedded fallbacks ---------------------------*/
static const char *g_fallback_theme_json =
"{\n"
"  \"themes\": [ { \"id\": \"native\", \"name\": \"Native\" } ]\n"
"}\n";

/* Minimal fallback UIs keep the application usable if UI files are missing. */
static const char *g_fallback_main_ui =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <object class=\"GtkApplicationWindow\" id=\"main_window\">\n"
"    <property name=\"title\">Umicom Studio (No-CSS mode)</property>\n"
"    <property name=\"default-width\">1024</property>\n"
"    <property name=\"default-height\">720</property>\n"
"    <child>\n"
"      <object class=\"GtkBox\" id=\"root_box\">\n"
"        <property name=\"orientation\">vertical</property>\n"
"        <child>\n"
"          <object class=\"GtkLabel\">\n"
"            <property name=\"label\">UI loaded without CSS; using native theme.</property>\n"
"            <property name=\"margin-top\">12</property>\n"
"            <property name=\"margin-bottom\">12</property>\n"
"            <property name=\"margin-start\">12</property>\n"
"            <property name=\"margin-end\">12</property>\n"
"          </object>\n"
"        </child>\n"
"      </object>\n"
"    </child>\n"
"  </object>\n"
"</interface>\n";

static const char *g_fallback_chat_ui =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <object class=\"GtkBox\" id=\"chat_root\">\n"
"    <property name=\"orientation\">vertical</property>\n"
"    <child>\n"
"      <object class=\"GtkLabel\">\n"
"        <property name=\"label\">Chat pane (no-CSS mode)</property>\n"
"      </object>\n"
"    </child>\n"
"  </object>\n"
"</interface>\n";

static const char *g_fallback_shortcuts_ui =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <object class=\"GtkShortcutsWindow\" id=\"shortcuts_window\">\n"
"    <property name=\"modal\">true</property>\n"
"    <property name=\"resizable\">true</property>\n"
"    <property name=\"transient-for\">main_window</property>\n"
"  </object>\n"
"</interface>\n";

/* Optional snippet fallbacks */
static const char *g_fallback_chat_snippet_xml =
"<object class=\"GtkBox\" id=\"chat_pane_snippet\">\n"
"  <property name=\"orientation\">vertical</property>\n"
"  <child><object class=\"GtkLabel\"><property name=\"label\">Chat snippet</property></object></child>\n"
"</object>\n";

static const char *g_fallback_log_snippet_xml =
"<object class=\"GtkBox\" id=\"log_pane_snippet\">\n"
"  <property name=\"orientation\">vertical</property>\n"
"  <child><object class=\"GtkLabel\"><property name=\"label\">Log snippet</property></object></child>\n"
"</object>\n";

/*------------------------------ Root directory ------------------------------*/
void umi_res_set_root(const char *root_dir)
{
    if (!root_dir || !*root_dir) return;
    g_strlcpy(g_res_root, root_dir, sizeof g_res_root);
}

const char *umi_res_get_root(void) { return g_res_root; }

static char *umi_res_build_abspath(const char *relative)
{
    return g_build_filename(g_res_root, relative, NULL);
}

/*----------------------------- Disk text loading ----------------------------*/
gchar *umi_res_load_text_file(const char *relative, gsize *len, GError **error)
{
    g_autofree char *abspath = umi_res_build_abspath(relative);
    gchar  *data = NULL;
    gsize   n = 0;
    if (!g_file_get_contents(abspath, &data, &n, error)) return NULL;
    if (len) *len = n;
    return data; /* caller g_free()s */
}

/*------------------------------- CSS loading --------------------------------*/
/* In no-CSS mode we simply do nothing and succeed. */
gboolean umi_res_load_css(GtkCssProvider *provider, GError **error)
{
#ifdef USIDE_DISABLE_CSS
    (void)provider; (void)error;
    return TRUE; /* Explicitly succeed: callers can skip any theming paths. */
#else
    (void)error; /* we provide a robust fallback instead of bubbling errors */

    /* Try to load a stylesheet from disk; if missing, just succeed silently. */
    g_autoptr(GError) local_err = NULL;
    g_autofree gchar *css_text = umi_res_load_text_file("styles/app.css", NULL, &local_err);

    if (css_text && *css_text) {
        gtk_css_provider_load_from_string(provider, css_text);
        return TRUE;
    }

    /* No fallback stylesheet in CSS-enabled mode: native theme is fine. */
    return TRUE;
#endif
}

/*------------------------------- Builder utils ------------------------------*/
static const char *map_resource_like_to_relative(const char *resource_or_rel)
{
    if (!resource_or_rel) return "";
    const char *p = strstr(resource_or_rel, "/ui/");
    if (p) return p + 1;
    p = strstr(resource_or_rel, "/partials/");
    if (p) return p + 1;
    return resource_or_rel;
}

gboolean umi_res_builder_add(GtkBuilder *builder, const char *rel_or_res_path, GError **error)
{
    const char *relative = map_resource_like_to_relative(rel_or_res_path);

    g_autoptr(GError) local_err = NULL;
    g_autofree char *abspath = umi_res_build_abspath(relative);
    if (gtk_builder_add_from_file(builder, abspath, &local_err)) return TRUE;

    /* Fallbacks keep the app running if files are missing. */
    const char *fallback_xml = NULL;
    if (g_str_has_suffix(relative, "main.ui"))                     fallback_xml = g_fallback_main_ui;
    else if (g_str_has_suffix(relative, "chat.ui"))                fallback_xml = g_fallback_chat_ui;
    else if (g_str_has_suffix(relative, "shortcuts.ui"))           fallback_xml = g_fallback_shortcuts_ui;
    else if (g_str_has_suffix(relative, "chat_pane_snippet.xml"))  fallback_xml = g_fallback_chat_snippet_xml;
    else if (g_str_has_suffix(relative, "log_pane_snippet.xml"))   fallback_xml = g_fallback_log_snippet_xml;

    if (fallback_xml && gtk_builder_add_from_string(builder, fallback_xml, -1, error))
        return TRUE;

    if (error && !*error) *error = local_err ? g_steal_pointer(&local_err) : NULL;
    return FALSE;
}

GtkBuilder *umi_builder_new_from_res_or_file(const char *rel_or_res_path, GError **error)
{
    GtkBuilder *b = gtk_builder_new();
    if (!umi_res_builder_add(b, rel_or_res_path, error)) { g_object_unref(b); return NULL; }
    return b;
}

/*-------------------------- JSON theme presets blob -------------------------*/
const char *umi_res_fallback_json_theme_presets(void)
{
    return g_fallback_theme_json;
}
/*---------------------------------------------------------------------------*/
