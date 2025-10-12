/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: src/ui/resources/ustudio_resources.h
 * PURPOSE: Public API for the Pure-C resource loader used to avoid
 *          glib-compile-resources and XML manifest steps.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-11 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_USTUDIO_RESOURCES_H
#define UMICOM_USTUDIO_RESOURCES_H

#include <gtk/gtk.h> /* GtkBuilder, GtkCssProvider */
#include <glib.h>    /* GError, gchar, gsize */

/*-----------------------------------------------------------------------------
 * Runtime configuration
 *---------------------------------------------------------------------------*/

/**
 * umi_res_set_root:
 * @root_dir: Absolute or relative directory containing 'styles', 'ui', 'partials'.
 *
 * Sets the base directory used to load UI/CSS/JSON assets at runtime.
 * This allows building without glib-compile-resources and editing assets live.
 * Passing NULL or an empty string is a no-op.
 */
void        umi_res_set_root(const char *root_dir);

/**
 * umi_res_get_root:
 *
 * Returns the currently configured resource root directory.
 * The returned pointer is owned by the library and must not be freed.
 */
const char *umi_res_get_root(void);

/*-----------------------------------------------------------------------------
 * Loading helpers
 *---------------------------------------------------------------------------*/

/**
 * umi_res_load_text_file:
 * @relative: Path relative to the resource root (e.g. "styles/app.css").
 * @len: (out) Optional byte count of returned data (may be NULL).
 * @error: (out) Optional GError** for failure details.
 *
 * Loads a UTF-8 text file entirely into memory. Returns a NUL-terminated
 * buffer that must be freed with g_free(), or NULL on error.
 */
gchar *     umi_res_load_text_file(const char *relative, gsize *len, GError **error);

/**
 * umi_res_load_css:
 * @provider: A GtkCssProvider to feed styles into.
 * @error:    (out) Optional GError** for failure details (file case only).
 *
 * Attempts to load "styles/app.css" from the resource root. On failure,
 * loads a safe embedded CSS fallback so the UI still renders. Returns TRUE
 * if either path succeeds, FALSE only if both paths fail.
 */
gboolean    umi_res_load_css(GtkCssProvider *provider, GError **error);

/**
 * umi_res_builder_add:
 * @builder:       A GtkBuilder instance that will receive objects.
 * @rel_or_res_path: Either a relative path like "ui/main.ui" or a resource-like
 *                   path such as "/com/umicom/ustudio/ui/main.ui".
 * @error:         (out) Optional GError** for failure details.
 *
 * Tries to load a GtkBuilder UI file from disk. If it's not found, a minimal,
 * embedded XML fallback is used for known files. Returns TRUE on success.
 */
gboolean    umi_res_builder_add(GtkBuilder *builder, const char *rel_or_res_path, GError **error);

/**
 * umi_res_fallback_json_theme_presets:
 *
 * Returns the embedded theme presets JSON for callers that want a guaranteed
 * default. The returned pointer is a constant string; do not free it.
 */
const char *umi_res_fallback_json_theme_presets(void);

/**
 * umi_builder_new_from_res_or_file:
 * @rel_or_res_path: See umi_res_builder_add().
 * @error:           (out) Optional GError** for failure details.
 *
 * Convenience to build and populate a GtkBuilder directly.
 * Caller owns the returned GtkBuilder and must g_object_unref() it.
 */
GtkBuilder *umi_builder_new_from_res_or_file(const char *rel_or_res_path, GError **error);

#endif /* UMICOM_USTUDIO_RESOURCES_H */
