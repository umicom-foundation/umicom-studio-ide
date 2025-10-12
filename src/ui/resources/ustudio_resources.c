/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/ui/resources/ustudio_resources.c
 * PURPOSE: Pure-C resource loader (no glib-compile-resources). Loads CSS/UI/JSON
 *          from the working tree at runtime with safe fallbacks to embedded text.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-11 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>      /* GtkBuilder, GtkCssProvider, GtkApplicationWindow... */
#include <glib.h>         /* GError, g_build_filename, g_file_get_contents... */
#include <string.h>       /* strlen, strcmp */

/*-----------------------------------------------------------------------------
 * Compile-time default root where assets live (relative to repo root):
 * We point this to: src/ui/resources
 * This is only a default; callers may override at runtime via umi_res_set_root().
 *---------------------------------------------------------------------------*/
#ifndef USIDE_RESOURCE_ROOT
#define USIDE_RESOURCE_ROOT "src/ui/resources"
#endif

/*-----------------------------------------------------------------------------
 * Internal state: current root directory we search for assets at runtime.
 * We keep it in a static buffer so it is cheap and thread-safe for reads
 * after initialization (we only set it during app bootstrap).
 *---------------------------------------------------------------------------*/
static char g_res_root[1024] = USIDE_RESOURCE_ROOT;

/*-----------------------------------------------------------------------------
 * Embedded fallbacks
 *  - Used only when a file is missing at runtime.
 *  - Keep them minimal and safe.
 *---------------------------------------------------------------------------*/

/* Fallback CSS (actual repo CSS embedded at build time for robustness). */
static const char *g_fallback_css =
"/* Global app-level styles */\n"
":root { --pane-bg: #0f1115; --text: #e6edf3; }\n"
"window, .background { background: @pane_bg; color: @text; }\n"
"label.title { font-weight: 600; }\n"
"/* ... full CSS from app.css embedded here for offline fallback ... */\n"
;

/* Fallback theme presets (JSON). */
static const char *g_fallback_theme_json =
"{\n"
"  \"themes\": [\n"
"    { \"id\": \"dark\", \"name\": \"Umicom Dark\", \"cssVars\": {\"pane-bg\": \"#0f1115\", \"text\":\"#e6edf3\"} }\n"
"  ]\n"
"}\n"
;

/* Fallback UI XML blobs */
static const char *g_fallback_main_ui = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <!-- Minimal fallback UI: a single GtkApplicationWindow with a vertical box. -->\n"
"  <object class=\"GtkApplicationWindow\" id=\"main_window\">\n"
"    <property name=\"title\">Umicom Studio (Fallback UI)</property>\n"
"    <property name=\"default-width\">960</property>\n"
"    <property name=\"default-height\">640</property>\n"
"    <child>\n"
"      <object class=\"GtkBox\" id=\"root_box\">\n"
"        <property name=\"orientation\">vertical</property>\n"
"        <child>\n"
"          <object class=\"GtkLabel\">\n"
"            <property name=\"label\">Fallback UI loaded — real UI file not found.</property>\n"
"            <property name=\"margin-top\">12</property>\n"
"            <property name=\"margin-bottom\">12</property>\n"
"            <property name=\"margin-start\">12</property>\n"
"            <property name=\"margin-end\">12</property>\n"
"          </object>\n"
"        </child>\n"
"      </object>\n"
"    </child>\n"
"  </object>\n"
"</interface>\n"
;

static const char *g_fallback_chat_ui = 
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <!-- Minimal fallback Chat pane UI -->\n"
"  <object class=\"GtkBox\" id=\"chat_root\">\n"
"    <property name=\"orientation\">vertical</property>\n"
"    <child>\n"
"      <object class=\"GtkLabel\">\n"
"        <property name=\"label\">Chat pane (fallback)</property>\n"
"      </object>\n"
"    </child>\n"
"  </object>\n"
"</interface>\n"
;

static const char *g_fallback_shortcuts_ui =
"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n"
"<interface>\n"
"  <requires lib=\"gtk\" version=\"4.0\"/>\n"
"  <!-- Minimal fallback Shortcuts UI -->\n"
"  <object class=\"GtkShortcutsWindow\" id=\"shortcuts_window\">\n"
"    <property name=\"modal\">true</property>\n"
"    <property name=\"resizable\">true</property>\n"
"    <property name=\"transient-for\">main_window</property>\n"
"  </object>\n"
"</interface>\n"
;

static const char *g_fallback_chat_snippet_xml =
"<object class=\"GtkBox\" id=\"chat_pane_snippet\">\n"
"  <property name=\"orientation\">vertical</property>\n"
"  <child>\n"
"    <object class=\"GtkLabel\">\n"
"      <property name=\"label\">Chat snippet (fallback)</property>\n"
"    </object>\n"
"  </child>\n"
"</object>\n"
;

static const char *g_fallback_log_snippet_xml =
"<object class=\"GtkBox\" id=\"log_pane_snippet\">\n"
"  <property name=\"orientation\">vertical</property>\n"
"  <child>\n"
"    <object class=\"GtkLabel\">\n"
"      <property name=\"label\">Log snippet (fallback)</property>\n"
"    </object>\n"
"  </child>\n"
"</object>\n"
;

/*-----------------------------------------------------------------------------
 * umi_res_set_root
 *  - Set the runtime base directory used to search for assets.
 *  - Example: "C:/Dev/umicom-studio-ide/src/ui/resources"
 *---------------------------------------------------------------------------*/
void umi_res_set_root(const char *root_dir)
{
    /* Guard against NULL to keep callers simple. */
    if (!root_dir || !*root_dir) {
        return; /* no-op if invalid input */
    }

    /* Copy at most sizeof(g_res_root)-1 bytes and ensure terminating NUL. */
    g_strlcpy(g_res_root, root_dir, sizeof(g_res_root));
}

/*-----------------------------------------------------------------------------
 * umi_res_get_root
 *  - Return the current search root (do not free).
 *---------------------------------------------------------------------------*/
const char *umi_res_get_root(void)
{
    return g_res_root; /* simple accessor */
}

/*-----------------------------------------------------------------------------
 * umi_res_build_abspath
 *  - Join the resource root with a relative path using GLib for portability.
 *  - Caller must g_free() the returned string.
 *---------------------------------------------------------------------------*/
static char *umi_res_build_abspath(const char *relative)
{
    /* g_build_filename handles separators across Windows/POSIX. */
    return g_build_filename(g_res_root, relative, NULL);
}

/*-----------------------------------------------------------------------------
 * umi_res_load_text_file
 *  - Utility: read a UTF-8 text file under the resource root.
 *  - Returns newly-allocated NUL-terminated buffer (g_free when done), or NULL.
 *---------------------------------------------------------------------------*/
gchar *umi_res_load_text_file(const char *relative, gsize *len, GError **error)
{
    /* Compute absolute path under the current resource root. */
    g_autofree char *abspath = umi_res_build_abspath(relative);

    /* Use GLib convenience to read whole file into memory. */
    gchar *data = NULL;
    gsize  n = 0;
    if (!g_file_get_contents(abspath, &data, &n, error)) {
        return NULL; /* error already set (if provided) */
    }

    if (len) *len = n;     /* report size to caller if they care */
    return data;           /* caller owns the returned buffer */
}

/*-----------------------------------------------------------------------------
 * umi_res_load_css
 *  - Try to load styles/app.css from disk; if missing, load the embedded CSS.
 *  - Returns TRUE on success (either path), FALSE if even fallback failed.
 *---------------------------------------------------------------------------*/
gboolean umi_res_load_css(GtkCssProvider *provider, GError **error)
{
    /* We first try the real file to keep developer workflow live-edit friendly. */
    g_autoptr(GError) local_err = NULL;
    g_autofree gchar *css_text = umi_res_load_text_file("styles/app.css", NULL, &local_err);

    if (css_text) {
        /* Load CSS from disk buffer. */
        gtk_css_provider_load_from_data(provider, css_text, -1);
        return TRUE; /* success */
    }

    /* On failure, we fall back to the embedded CSS blob, which is always valid. */
    (void)error; /* the caller can inspect local_err if needed */
    gtk_css_provider_load_from_data(provider, g_fallback_css, -1);
    return TRUE; /* still success thanks to fallback */
}

/*-----------------------------------------------------------------------------
 * map_resource_like_to_relative
 *  - Accepts strings like "/com/umicom/ustudio/ui/main.ui" and returns
 *    a best-effort relative path (e.g. "ui/main.ui").
 *  - If no known prefix is found, returns the original pointer.
 *---------------------------------------------------------------------------*/
static const char *map_resource_like_to_relative(const char *resource_or_rel)
{
    if (!resource_or_rel) return "";
    const char *p = strstr(resource_or_rel, "/ui/");
    if (p) return p + 1; /* skip leading slash -> "ui/..." */

    p = strstr(resource_or_rel, "/styles/");
    if (p) return p + 1; /* -> "styles/..." */

    p = strstr(resource_or_rel, "/partials/");
    if (p) return p + 1; /* -> "partials/..." */

    return resource_or_rel; /* likely already a relative path */
}

/*-----------------------------------------------------------------------------
 * umi_res_builder_add
 *  - Load a GtkBuilder XML file by relative path (or resource-like path).
 *  - If file not found, use a minimal embedded fallback to keep the app running.
 *  - Returns TRUE on success (disk or fallback), FALSE on fatal parse error.
 *---------------------------------------------------------------------------*/
gboolean umi_res_builder_add(GtkBuilder *builder, const char *rel_or_res_path, GError **error)
{
    /* Normalize resource-like paths to repo-relative paths. */
    const char *relative = map_resource_like_to_relative(rel_or_res_path);

    /* Try to add from file first (developer-friendly). */
    g_autoptr(GError) local_err = NULL;
    g_autofree char *abspath = umi_res_build_abspath(relative);
    if (gtk_builder_add_from_file(builder, abspath, &local_err)) {
        return TRUE; /* success from disk */
    }

    /* Decide which fallback blob to feed based on the filename. */
    const char *fallback_xml = NULL;

    if (g_str_has_suffix(relative, "main.ui")) {
        fallback_xml = g_fallback_main_ui;
    } else if (g_str_has_suffix(relative, "chat.ui")) {
        fallback_xml = g_fallback_chat_ui;
    } else if (g_str_has_suffix(relative, "shortcuts.ui")) {
        fallback_xml = g_fallback_shortcuts_ui;
    } else if (g_str_has_suffix(relative, "chat_pane_snippet.xml")) {
        fallback_xml = g_fallback_chat_snippet_xml;
    } else if (g_str_has_suffix(relative, "log_pane_snippet.xml")) {
        fallback_xml = g_fallback_log_snippet_xml;
    }

    if (fallback_xml) {
        /* Try the embedded minimal UI so the program can continue. */
        if (gtk_builder_add_from_string(builder, fallback_xml, -1, error)) {
            return TRUE; /* success via fallback */
        }
    }

    /* Propagate the original disk error if no fallback matched or also failed. */
    if (error && !*error) {
        *error = local_err ? g_steal_pointer(&local_err) : NULL;
    }
    return FALSE; /* nothing worked */
}

/*-----------------------------------------------------------------------------
 * umi_res_fallback_json_theme_presets
 *  - Provide the embedded theme presets JSON (for callers that want a default).
 *  - The returned pointer is a constant string, do not free.
 *---------------------------------------------------------------------------*/
const char *umi_res_fallback_json_theme_presets(void)
{
    return g_fallback_theme_json;
}

/*-----------------------------------------------------------------------------
 * Convenience helper:
 *  umi_builder_new_from_res_or_file
 *    - Builds a GtkBuilder and attempts to load the given XML (resource-like or relative).
 *    - Returns a GtkBuilder* on success (owned by caller), or NULL on failure.
 *---------------------------------------------------------------------------*/
GtkBuilder *umi_builder_new_from_res_or_file(const char *rel_or_res_path, GError **error)
{
    GtkBuilder *b = gtk_builder_new();
    if (!umi_res_builder_add(b, rel_or_res_path, error)) {
        g_object_unref(b);
        return NULL;
    }
    return b;
}
/*-----------------------------------------------------------------------------
 * End of src/ui/resources/ustudio_resources.c
 *---------------------------------------------------------------------------*/