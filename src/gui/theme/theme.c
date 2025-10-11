/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: src/gui/theme/theme.c
 * PURPOSE: Theme application (base CSS + named presets via GResource)
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 15-09-2025
 * License: MIT
 *
 * DO NOT REMOVE HEADERS.
 *
 * Summary:
 *   - Loads and installs base app CSS from a compiled GResource.
 *   - Optionally resolves and installs a theme-specific CSS from presets
 *     defined in a JSON manifest (also compiled into the GResource).
 *   - Keeps comments very explicit for maintenance and clarity.
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>
#include <gio/gio.h>
#include <json-glib/json-glib.h>
#include "include/theme.h"

/*----------------------------------------------------------------------------
 * Resource layout (must match src/ui/resources/ustudio.gresource.xml)
 *   Prefix: /com/umicom/ustudio
 *   Files:
 *     - styles/app.css
 *     - styles/theme_presets.json
 *     - styles/<preset>.css    (examples: light.css, dark.css, etc.)
 *----------------------------------------------------------------------------*/
#define USIDE_RES_PREFIX              "/com/umicom/ustudio"
#define USIDE_RES_STYLES_PREFIX       "/com/umicom/ustudio/styles"
#define USIDE_RES_APP_CSS             "/com/umicom/ustudio/styles/app.css"
#define USIDE_RES_PRESETS_JSON        "/com/umicom/ustudio/styles/theme_presets.json"

/*----------------------------------------------------------------------------
 * Helper: Get target display. If a window is provided, we derive the display
 * from it; otherwise we fall back to the default display.
 *----------------------------------------------------------------------------*/
static GdkDisplay *get_target_display(GtkWindow *win) {
  if (win != NULL) {
    GtkWidget *w = GTK_WIDGET(win);
    if (gtk_widget_get_realized(w)) {
      return gtk_widget_get_display(w);
    }
    /* If not realized yet, still prefer the widget's display when available */
    GdkDisplay *disp = gtk_widget_get_display(w);
    if (disp) return disp;
  }
  return gdk_display_get_default();
}

/*----------------------------------------------------------------------------
 * Helper: Load a CSS resource and add it to the given display with
 * GTK_STYLE_PROVIDER_PRIORITY_APPLICATION.
 * Returns: TRUE on success, FALSE on error (logged).
 *----------------------------------------------------------------------------*/
static gboolean apply_css_from_resource(GdkDisplay *display, const char *css_res_path) {
  g_autoptr(GtkCssProvider) provider = gtk_css_provider_new();

  /* gtk_css_provider_load_from_resource() expects a valid resource path
     that is compiled into the binary via GResource. */
  gtk_css_provider_load_from_resource(provider, css_res_path);

  /* Attach to the display with application priority so app styles override theme defaults. */
  gtk_style_context_add_provider_for_display(
      display,
      GTK_STYLE_PROVIDER(provider),
      GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

  /* There is no conventional error return for load_from_resource; if the
     resource path is wrong, nothing will apply. We still log a warning. */
  if (!g_resources_get_info(css_res_path, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL, NULL, NULL)) {
    g_warning("Theme: CSS resource not found: %s", css_res_path);
    return FALSE;
  }

  return TRUE;
}

/*----------------------------------------------------------------------------
 * Helper: Load raw bytes from a GResource path.
 *----------------------------------------------------------------------------*/
static gboolean load_bytes_from_resource(const char *res_path, GBytes **out_bytes) {
  g_return_val_if_fail(out_bytes != NULL, FALSE);
  *out_bytes = g_resources_lookup_data(res_path, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL);
  if (!*out_bytes) {
    g_warning("Theme: failed to load resource bytes: %s", res_path);
    return FALSE;
  }
  return TRUE;
}

/*----------------------------------------------------------------------------
 * Presets JSON layout (theme_presets.json):
 * {
 *   "themes": [
 *     { "name": "Light", "css": "light.css" },
 *     { "name": "Dark",  "css": "dark.css"  }
 *   ],
 *   "default": "Light"
 * }
 *
 * We resolve a theme name to a CSS resource path under /styles.
 * If 'theme_name' is NULL or empty, we pick the "default" entry when present.
 *----------------------------------------------------------------------------*/
static char *resolve_css_resource_for_theme(const char *theme_name) {
  GBytes *json_bytes = NULL;
  if (!load_bytes_from_resource(USIDE_RES_PRESETS_JSON, &json_bytes)) {
    /* No presets JSON available */
    return NULL;
  }

  gsize len = 0;
  const char *data = g_bytes_get_data(json_bytes, &len);

  JsonParser *parser = json_parser_new();
  GError *err = NULL;
  gboolean parsed = json_parser_load_from_data(parser, data, (gssize)len, &err);
  if (!parsed) {
    g_warning("Theme: invalid JSON in presets: %s", err ? err->message : "unknown error");
    if (err) g_error_free(err);
    g_object_unref(parser);
    g_bytes_unref(json_bytes);
    return NULL;
  }

  JsonNode *root = json_parser_get_root(parser);
  if (!JSON_NODE_HOLDS_OBJECT(root)) {
    g_warning("Theme: presets root is not an object.");
    g_object_unref(parser);
    g_bytes_unref(json_bytes);
    return NULL;
  }

  JsonObject *obj = json_node_get_object(root);
  JsonArray *themes = NULL;
  if (json_object_has_member(obj, "themes")) {
    JsonNode *arr_node = json_object_get_member(obj, "themes");
    if (JSON_NODE_HOLDS_ARRAY(arr_node)) {
      themes = json_node_get_array(arr_node);
    }
  }

  char *resolved = NULL;

  /* Select default if no name passed */
  const char *requested = theme_name;
  if (!requested || !*requested) {
    if (json_object_has_member(obj, "default")) {
      requested = json_object_get_string_member(obj, "default");
    }
  }

  if (themes != NULL) {
    if (!requested || !*requested) {
      /* No name at all — attempt to use the first theme if present */
      if (json_array_get_length(themes) > 0) {
        JsonObject *t0 = json_array_get_object_element(themes, 0);
        const char *css = t0 ? json_object_get_string_member(t0, "css") : NULL;
        if (css && *css) {
          resolved = g_build_filename(USIDE_RES_STYLES_PREFIX, css, NULL);
        }
      }
    } else {
      /* Find by name (case-insensitive) */
      gsize n = json_array_get_length(themes);
      for (gsize i = 0; i < n; i++) {
        JsonObject *t = json_array_get_object_element(themes, i);
        const char *name = t ? json_object_get_string_member(t, "name") : NULL;
        if (name && g_ascii_strcasecmp(name, requested) == 0) {
          const char *css = json_object_get_string_member(t, "css");
          if (css && *css) {
            resolved = g_build_filename(USIDE_RES_STYLES_PREFIX, css, NULL);
          }
          break;
        }
      }
    }
  }

  g_object_unref(parser);
  g_bytes_unref(json_bytes);

  if (resolved) {
    /* Verify that the CSS file actually exists in the compiled resources */
    if (!g_resources_get_info(resolved, G_RESOURCE_LOOKUP_FLAGS_NONE, NULL, NULL, NULL)) {
      g_warning("Theme: resolved CSS resource does not exist: %s", resolved);
      g_free(resolved);
      resolved = NULL;
    }
  }

  return resolved; /* caller takes ownership (g_free) */
}

/*----------------------------------------------------------------------------
 * Always apply the base app CSS (app.css).
 *----------------------------------------------------------------------------*/
static void apply_builtin_app_css(GdkDisplay *display) {
  if (!apply_css_from_resource(display, USIDE_RES_APP_CSS)) {
    g_warning("Theme: failed to apply base app.css (resource path: %s)", USIDE_RES_APP_CSS);
  }
}

/*----------------------------------------------------------------------------
 * Public API
 *----------------------------------------------------------------------------*/
void umi_theme_apply(GtkWindow *win, const char *theme_name) {
  /* Choose the display from the window if possible; fallback to default */
  GdkDisplay *display = get_target_display(win);
  if (!display) {
    g_warning("Theme: no available GdkDisplay — skipping CSS application.");
    return;
  }

  /* 1) Base app styles (always) */
  apply_builtin_app_css(display);

  /* 2) Optional theme preset chosen by name */
  g_autofree char *preset_css = resolve_css_resource_for_theme(theme_name);
  if (preset_css != NULL) {
    if (!apply_css_from_resource(display, preset_css)) {
      g_warning("Theme: failed to apply preset CSS: %s", preset_css);
    }
  } else {
    if (theme_name && *theme_name) {
      g_message("Theme: preset '%s' not found — using base CSS only.", theme_name);
    }
  }
}
