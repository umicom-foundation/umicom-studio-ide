/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/theme.c
 *
 * PURPOSE:
 *   Apply base app CSS and an optional named theme preset from GResource.
 *   Preset mapping is read from styles/theme_presets.json inside resources.
 *
 * DESIGN:
 *   - GTK4 only; uses GtkCssProvider and display-scoped providers.
 *   - No blocking I/O; all data is embedded via GResource.
 *   - Defensive checks around resource presence.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include "theme.h"

/* Resource paths (must match ustudio.gresource.xml) */
#define USIDE_RES_STYLES_PREFIX  "/com/umicom/ustudio/styles"
#define USIDE_RES_APP_CSS        "/com/umicom/ustudio/styles/app.css"
#define USIDE_RES_PRESETS_JSON   "/com/umicom/ustudio/styles/theme_presets.json"

/* --- Helpers -------------------------------------------------------------- */

static GdkDisplay *get_target_display(GtkWindow *win)
{
  if (win) {
    GdkDisplay *d = gtk_widget_get_display(GTK_WIDGET(win));
    if (d) return d;
  }
  return gdk_display_get_default();
}

static gboolean apply_css_from_resource(GdkDisplay *display, const char *css_res_path)
{
  if (!display || !css_res_path || !*css_res_path) return FALSE;

  if (!g_resources_get_info(css_res_path, 0, NULL, NULL, NULL)) {
    g_warning("Theme: CSS resource missing: %s", css_res_path);
    return FALSE;
  }

  g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();
  gtk_css_provider_load_from_resource(provider, css_res_path);
  gtk_style_context_add_provider_for_display(
      display, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
  return TRUE;
}

static gboolean load_bytes_from_resource(const char *res_path, GBytes **out_bytes)
{
  g_return_val_if_fail(out_bytes != NULL, FALSE);
  *out_bytes = g_resources_lookup_data(res_path, 0, NULL);
  if (!*out_bytes) {
    g_warning("Theme: failed to load resource: %s", res_path);
    return FALSE;
  }
  return TRUE;
}

/* Resolve a preset name to a CSS resource path, or NULL if not found. */
static char *resolve_css_resource_for_theme(const char *theme_name)
{
  GBytes *json_bytes = NULL;
  if (!load_bytes_from_resource(USIDE_RES_PRESETS_JSON, &json_bytes))
    return NULL;

  gsize len = 0;
  const char *data = g_bytes_get_data(json_bytes, &len);

  g_autoptr(JsonParser) parser = json_parser_new();
  GError *err = NULL;
  if (!json_parser_load_from_data(parser, data, (gssize)len, &err)) {
    g_warning("Theme: presets JSON invalid: %s", err ? err->message : "unknown");
    if (err) g_error_free(err);
    g_bytes_unref(json_bytes);
    return NULL;
  }

  JsonNode   *root  = json_parser_get_root(parser);
  if (!JSON_NODE_HOLDS_OBJECT(root)) {
    g_warning("Theme: presets root not an object");
    g_bytes_unref(json_bytes);
    return NULL;
  }

  JsonObject *obj   = json_node_get_object(root);
  JsonArray  *arr   = NULL;
  if (json_object_has_member(obj, "themes")) {
    JsonNode *n = json_object_get_member(obj, "themes");
    if (n && JSON_NODE_HOLDS_ARRAY(n)) arr = json_node_get_array(n);
  }

  /* Choose requested name or default */
  const char *requested = theme_name;
  if (!requested || !*requested) {
    if (json_object_has_member(obj, "default"))
      requested = json_object_get_string_member(obj, "default");
  }

  char *resolved = NULL;
  if (arr) {
    const gsize n = json_array_get_length(arr);
    if (!requested || !*requested) {
      if (n > 0) {
        JsonObject *t0 = json_array_get_object_element(arr, 0);
        const char *css = t0 ? json_object_get_string_member(t0, "css") : NULL;
        if (css && *css) resolved = g_build_filename(USIDE_RES_STYLES_PREFIX, css, NULL);
      }
    } else {
      for (gsize i = 0; i < n; i++) {
        JsonObject *t = json_array_get_object_element(arr, i);
        const char *name = t ? json_object_get_string_member(t, "name") : NULL;
        if (name && g_ascii_strcasecmp(name, requested) == 0) {
          const char *css = json_object_get_string_member(t, "css");
          if (css && *css) resolved = g_build_filename(USIDE_RES_STYLES_PREFIX, css, NULL);
          break;
        }
      }
    }
  }

  g_bytes_unref(json_bytes);

  if (resolved && !g_resources_get_info(resolved, 0, NULL, NULL, NULL)) {
    g_warning("Theme: preset CSS not in resources: %s", resolved);
    g_free(resolved);
    resolved = NULL;
  }
  return resolved; /* caller g_free */
}

/* --- Public API ----------------------------------------------------------- */

void umi_theme_apply(GtkWindow *win, const char *theme_name)
{
  GdkDisplay *display = get_target_display(win);
  if (!display) {
    g_warning("Theme: no display available");
    return;
  }

  /* Always apply base CSS */
  (void)apply_css_from_resource(display, USIDE_RES_APP_CSS);

  /* Optional preset */
  g_autofree char *css = resolve_css_resource_for_theme(theme_name);
  if (css) (void)apply_css_from_resource(display, css);
}

void umi_theme_apply_default(GtkWindow *win)
{
  umi_theme_apply(win, NULL);
}

/* Expose the parsed presets to callers who want to inspect them. */
JsonNode *umi_theme_load_presets(void)
{
  GBytes *bytes = NULL;
  if (!load_bytes_from_resource(USIDE_RES_PRESETS_JSON, &bytes)) return NULL;

  gsize len = 0;
  const char *data = g_bytes_get_data(bytes, &len);

  JsonParser *parser = json_parser_new();
  GError *err = NULL;
  if (!json_parser_load_from_data(parser, data, (gssize)len, &err)) {
    if (err) g_error_free(err);
    g_object_unref(parser);
    g_bytes_unref(bytes);
    return NULL;
  }

  /* Return a deep copy of the root so callers can own/free it. */
  JsonNode *root = json_parser_steal_root(parser);
  g_object_unref(parser);
  g_bytes_unref(bytes);
  return root; /* caller: json_node_free(root) */
}

/* Optional cleanup hook for leak-check runs (no persistent singletons here). */
void umi_theme_shutdown(void)
{
  /* nothing persistent to free today */
}
