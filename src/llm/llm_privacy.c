/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/llm_privacy.c
 * PURPOSE: Helpers for loading/saving privacy settings
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>                   /* Core GLib types/macros, memory helpers  */
#include <json-glib/json-glib.h>    /* JSON-GLib: parsing + generation         */
#include <privacy.h>                /* Public struct + API for privacy settings*/

/* ---------------------------------------------------------------------------
 * Internal helpers
 * -------------------------------------------------------------------------*/

/* Return a heap-allocated struct pre-populated with safe defaults.
 * Caller must free via umi_privacy_free(). */
static UmiPrivacySettings *s_default_settings(void) {
  /* Allocate the settings struct; GLib’s g_new0() zero-initializes memory.   */
  UmiPrivacySettings *s = g_new0(UmiPrivacySettings, 1);

  /* Sensible + conservative defaults. Tweak here if product policy changes. */
  s->allow_network     = TRUE;   /* Enable network by default (IDE needs it). */
  s->redact_file_paths = TRUE;   /* Hide absolute local file paths.           */
  s->redact_usernames  = TRUE;   /* Hide system usernames.                    */
  s->ban_telemetry     = TRUE;   /* No telemetry unless explicitly disabled.  */
  /* s->extra_redactions remains NULL unless loaded from file.               */

  return s; /* Ownership to caller. */
}

/* Parse a JSON object -> fill an already-allocated UmiPrivacySettings. 
 * Returns TRUE on success (all recognized fields parsed), FALSE on any error. */
static gboolean s_parse_json(JsonObject *obj, UmiPrivacySettings *s) {
  /* Defensive checks: both pointers must be non-NULL and obj should be a JSON
   * object. JSON-GLib keeps types dynamic, so we validate before reading.     */
  if (!obj || !s) return FALSE;

  /* Helper pointer reused for each lookup to avoid re-declarations.          */
  const JsonNode *tmp = NULL;

  /* Each section below:
   *  1) Look up a member by key.
   *  2) Ensure the node holds the expected type.
   *  3) Extract the value into our struct.                                   */

  /* allow_network: boolean */
  tmp = json_object_get_member(obj, "allow_network");
  if (tmp && JSON_NODE_HOLDS_VALUE(tmp))
    s->allow_network = json_node_get_boolean(tmp);

  /* redact_file_paths: boolean */
  tmp = json_object_get_member(obj, "redact_file_paths");
  if (tmp && JSON_NODE_HOLDS_VALUE(tmp))
    s->redact_file_paths = json_node_get_boolean(tmp);

  /* redact_usernames: boolean */
  tmp = json_object_get_member(obj, "redact_usernames");
  if (tmp && JSON_NODE_HOLDS_VALUE(tmp))
    s->redact_usernames = json_node_get_boolean(tmp);

  /* ban_telemetry: boolean */
  tmp = json_object_get_member(obj, "ban_telemetry");
  if (tmp && JSON_NODE_HOLDS_VALUE(tmp))
    s->ban_telemetry = json_node_get_boolean(tmp);

  /* extra_redactions: string (optional). We dup into our own owned buffer.   */
  tmp = json_object_get_member(obj, "extra_redactions");
  if (tmp && JSON_NODE_HOLDS_VALUE(tmp)) {
    const gchar *val = json_node_get_string(tmp);   /* Returns const gchar*.  */
    g_free(s->extra_redactions);                    /* Free old value if any. */
    s->extra_redactions = val ? g_strdup(val) : NULL;
  }

  return TRUE;
}

/* ---------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------*/

/* Load settings from a JSON file at `path`.
 * On success: returns a valid, heap-allocated settings struct.
 * On failure: returns defaults (still valid) and logs the reason. */
UmiPrivacySettings *umi_privacy_load(const char *path) {
  /* Always start with safe defaults so we return a usable struct even if
   * reading/parsing fails.                                                   */
  UmiPrivacySettings *s = s_default_settings();

  /* Quick exit if caller passed no path. We keep defaults and return.        */
  if (!path || !*path)
    return s;

  /* JSON-GLib parser: g_autoptr auto-unrefs when we leave this function.     */
  g_autoptr(JsonParser) parser = json_parser_new();
  g_autoptr(GError)     err    = NULL;

  /* Attempt to parse the file (returns FALSE on IO or JSON syntax errors).   */
  if (!json_parser_load_from_file(parser, path, &err)) {
    g_warning("privacy: failed to parse '%s': %s — using defaults",
              path, err ? err->message : "unknown error");
    return s; /* Keep defaults. */
  }

  /* Validate the root is an object; otherwise ignore and keep defaults.      */
  JsonNode *root = json_parser_get_root(parser);
  if (!root || json_node_get_node_type(root) != JSON_NODE_OBJECT) {
    g_warning("privacy: root is not a JSON object in '%s' — using defaults", path);
    return s;
  }

  /* Fill the defaults with values from the file.                              */
  if (!s_parse_json(json_node_get_object(root), s)) {
    g_warning("privacy: failed to read members from '%s' — using defaults", path);
    /* We still return `s` which contains defaults.                            */
  }

  return s; /* Caller owns and must free via umi_privacy_free(). */
}

/* Save settings to a JSON file at `path`.
 * Returns TRUE on success; FALSE on any IO or serialization error. */
gboolean umi_privacy_save(const char *path, const UmiPrivacySettings *s) {
  if (!path || !*path || !s) {
    g_warning("privacy: save called with invalid argument(s)");
    return FALSE;
  }

  /* Build a JSON object { ... } using JsonBuilder for clarity and ordering.  */
  g_autoptr(JsonBuilder) builder = json_builder_new();
  json_builder_begin_object(builder);                       /* { */

  /* Serialize every field explicitly so the on-disk schema is stable.       */
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

  json_builder_end_object(builder);                         /* } */

  /* Turn the builder’s root node into text with pretty formatting.           */
  g_autoptr(JsonGenerator) gen = json_generator_new();
  g_autoptr(JsonNode)      node = json_builder_get_root(builder);
  json_generator_set_root(gen, node);
  json_generator_set_pretty(gen, TRUE);

  /* json_generator_to_data() returns a UTF-8 buffer and its length.          */
  gsize len = 0;
  char *text = json_generator_to_data(gen, &len);
  if (!text) {
    g_warning("privacy: failed to serialize settings (generator returned NULL)");
    return FALSE;
  }

  /* IMPORTANT FIX:
   * Use the exact length returned by JSON-GLib, not strlen(), to avoid an
   * 'unused variable' warning and to be robust if the buffer ever contains
   * embedded NULs (it shouldn’t, but this is correct API usage).             */
  g_autoptr(GError) err = NULL;
  gboolean written = g_file_set_contents(path, text, len, &err);
  g_free(text); /* We own the buffer returned by json_generator_to_data().    */

  if (!written) {
    g_warning("privacy: failed to write '%s': %s",
              path, err ? err->message : "unknown error");
    return FALSE;
  }

  return TRUE;
}

/* Free a UmiPrivacySettings previously returned by umi_privacy_load()
 * or created by the caller. Safe to call with NULL. */
void umi_privacy_free(UmiPrivacySettings *s) {
  if (!s) return;                /* Defensive: allow umi_privacy_free(NULL).  */
  g_free(s->extra_redactions);   /* Free any heap string we own.              */
  g_free(s);                     /* Finally free the struct itself.           */
}
/*---------------------------------------------------------------------------*/