/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/llm_privacy.c
  * PURPOSE:
 *   Load/save IDE privacy settings (JSON) with zero cross-folder deps.
 *
 * DESIGN:
 *   - Self-contained: only needs GLib + JSON-GLib + local public header <privacy.h>.
 *   - Uses json_object_*_member() helpers instead of raw JsonNode access to
 *     avoid const-discard warnings and to keep code simpler/safer.
 *   - Defensive: missing keys fallback to a sane default.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/llm_privacy.c
 * PURPOSE:
 *   Load/save IDE privacy settings (JSON) with zero cross-folder deps.
 *
 * DESIGN:
 *   - Self-contained: only needs GLib + JSON-GLib + local public header <privacy.h>.
 *   - Uses json_object_*_member() helpers instead of raw JsonNode access to
 *     avoid const-discard warnings and to keep code simpler/safer.
 *   - Defensive: missing keys fallback to a sane default.
 *
 * COPYRIGHT: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>
#include <json-glib/json-glib.h>
#include <string.h>

#include <privacy.h>   /* UmiPrivacySettings API/struct */

/*-----------------------------------------------------------------------------
 * Internal helpers
 *---------------------------------------------------------------------------*/

/* Create default settings. Caller owns the returned struct. */
static UmiPrivacySettings *s_default_settings(void) {
  UmiPrivacySettings *s = g_new0(UmiPrivacySettings, 1);

  /* Defaults are conservative (privacy-first) but still usable. */
  s->allow_network     = TRUE;   /* IDE often needs network for LLMs, updates, etc. */
  s->redact_file_paths = TRUE;   /* Don’t leak absolute local paths.                */
  s->redact_usernames  = TRUE;   /* Don’t leak $USERNAME.                           */
  s->ban_telemetry     = TRUE;   /* No telemetry unless user opts in.               */
  s->extra_redactions  = NULL;   /* Optional; comma/regex list; NULL = none.        */

  return s;
}

/* Populate settings from a JSON object. Returns TRUE on success. */
static gboolean s_parse_json(JsonObject *obj, UmiPrivacySettings *s) {
  g_return_val_if_fail(obj != NULL, FALSE);
  g_return_val_if_fail(s   != NULL, FALSE);

  /* NOTE:
   * We intentionally use the typed json_object_*_member() helpers which
   * accept (JsonObject*) and return primitive types. This avoids the
   * const-discard warnings you saw with JsonNode access, and is simpler.
   */

  if (json_object_has_member(obj, "allow_network"))
    s->allow_network = json_object_get_boolean_member(obj, "allow_network");

  if (json_object_has_member(obj, "redact_file_paths"))
    s->redact_file_paths = json_object_get_boolean_member(obj, "redact_file_paths");

  if (json_object_has_member(obj, "redact_usernames"))
    s->redact_usernames = json_object_get_boolean_member(obj, "redact_usernames");

  if (json_object_has_member(obj, "ban_telemetry"))
    s->ban_telemetry = json_object_get_boolean_member(obj, "ban_telemetry");

  if (json_object_has_member(obj, "extra_redactions")) {
    const gchar *val = json_object_get_string_member(obj, "extra_redactions");
    g_clear_pointer(&s->extra_redactions, g_free);
    s->extra_redactions = (val && *val) ? g_strdup(val) : NULL;
  }

  return TRUE;
}

/*-----------------------------------------------------------------------------
 * Public API
 *---------------------------------------------------------------------------*/

UmiPrivacySettings *umi_privacy_load(const char *path) {
  /* Always start with defaults; layer file values over them if present. */
  UmiPrivacySettings *s = s_default_settings();
  if (!path || !*path) return s;

  g_autoptr(GError) err = NULL;
  g_autoptr(JsonParser) parser = json_parser_new();

  if (!json_parser_load_from_file(parser, path, &err)) {
    g_debug("privacy: load failed for '%s': %s (using defaults)",
            path, err ? err->message : "unknown");
    return s; /* Defaults already set. */
  }

  JsonNode *root = json_parser_get_root(parser);
  if (!JSON_NODE_HOLDS_OBJECT(root)) {
    g_debug("privacy: invalid root JSON type in '%s' (using defaults)", path);
    return s;
  }

  JsonObject *obj = json_node_get_object(root);
  if (!s_parse_json(obj, s)) {
    g_debug("privacy: parse failed for '%s' (using defaults)", path);
    /* Keep defaults already in `s`. */
  }

  return s;
}

gboolean umi_privacy_save(const char *path, const UmiPrivacySettings *s) {
  g_return_val_if_fail(path && *path, FALSE);
  g_return_val_if_fail(s != NULL, FALSE);

  g_autoptr(GError) err = NULL;
  g_autoptr(JsonBuilder) builder = json_builder_new();

  json_builder_begin_object(builder);

  json_builder_set_member_name(builder, "allow_network");
  json_builder_add_boolean_value(builder, s->allow_network);

  json_builder_set_member_name(builder, "redact_file_paths");
  json_builder_add_boolean_value(builder, s->redact_file_paths);

  json_builder_set_member_name(builder, "redact_usernames");
  json_builder_add_boolean_value(builder, s->redact_usernames);

  json_builder_set_member_name(builder, "ban_telemetry");
  json_builder_add_boolean_value(builder, s->ban_telemetry);

  json_builder_set_member_name(builder, "extra_redactions");
  json_builder_add_string_value(builder, s->extra_redactions ? s->extra_redactions : "");

  json_builder_end_object(builder);

  g_autoptr(JsonGenerator) gen = json_generator_new();
  g_autoptr(JsonNode) root = json_builder_get_root(builder);
  json_generator_set_root(gen, root);
  json_generator_set_pretty(gen, TRUE);

  if (!json_generator_to_file(gen, path, &err)) {
    g_warning("privacy: save failed for '%s': %s", path, err ? err->message : "unknown");
    return FALSE;
  }
  return TRUE;
}

void umi_privacy_free(UmiPrivacySettings *s) {
  if (!s) return;
  g_clear_pointer(&s->extra_redactions, g_free);
  g_free(s);
}
/*--- end of file ---*/