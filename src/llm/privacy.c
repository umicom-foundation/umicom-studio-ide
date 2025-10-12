/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/privacy.c
 *
 * PURPOSE:
 *   Implementation of the privacy/URL-allowlist checks declared in
 *   src/llm/includes/privacy.h. This protects users from accidental or
 *   malicious requests to local machine / private networks / unsafe schemes.
 *
 * REQUIREMENTS:
 *   - GLib 2.68+ (GUri API). We rely on g_uri_parse() to safely inspect URLs.
 *
 * HOW IT WORKS (step-by-step):
 *   1) Validate input pointers and buffer capacities.
 *   2) Parse the URL using g_uri_parse() into a GUri* (structured form).
 *   3) Ensure the scheme is http or https. Reject others (e.g., file, data).
 *   4) Inspect the host. If it’s loopback/localhost or in private ranges,
 *      reject with a clear message.
 *   5) Clean up the GUri and return TRUE/FALSE accordingly.
 *
 * SECURITY NOTES:
 *   - This is a conservative default policy. If later we need exceptions
 *     (e.g., allow localhost for dev builds), we can add a toggle or hook.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#include <string.h>        /* for strlen, memcmp, strncmp                              */
#include <glib.h>          /* for gboolean, g_autoptr, g_uri_parse, GUri               */
#include <glib/guri.h>     /* explicit include for GUri API                            */
#include "includes/privacy.h"  /* our own public header (stable signature)             */

/* Small helper to write an error string safely into errbuf (if provided). */
static void
write_err(char *errbuf, unsigned errcap, const char *msg)
{
  /* If no buffer was supplied, we silently do nothing. */
  if (!errbuf || errcap == 0)
    return;

  /* g_strlcpy guarantees NUL-termination as long as errcap > 0. */
  g_strlcpy(errbuf, msg ? msg : "unknown error", (gsize)errcap);
}

/* Check textual host for loopback / localhost. */
static gboolean
is_loopback_host(const char *host)
{
  if (!host || !*host)
    return FALSE;                           /* empty host => handled elsewhere     */

  /* "localhost" matches exactly (case-insensitive). */
  if (g_ascii_strcasecmp(host, "localhost") == 0)
    return TRUE;

  /* IPv6 loopback "[::1]" often appears without brackets in parsed host as "::1". */
  if (strcmp(host, "::1") == 0)
    return TRUE;

  /* IPv4 loopback range 127.0.0.0/8 => textual prefix "127." */
  if (g_str_has_prefix(host, "127."))
    return TRUE;

  return FALSE;
}

/* Check textual host for RFC1918 private IPv4 ranges. */
static gboolean
is_private_v4_host(const char *host)
{
  if (!host)
    return FALSE;

  /* 10.0.0.0/8 => "10." */
  if (g_str_has_prefix(host, "10."))
    return TRUE;

  /* 192.168.0.0/16 => "192.168." */
  if (g_str_has_prefix(host, "192.168."))
    return TRUE;

  /* 172.16.0.0/12 => "172.16." .. "172.31." (check up to first two octets) */
  if (g_str_has_prefix(host, "172."))
  {
    /* Parse second octet quickly without full inet_pton; robust enough for strings. */
    const char *p = host + 4;               /* points just after "172."              */
    /* Read number until next '.' */
    int oct2 = 0;
    while (*p >= '0' && *p <= '9') { oct2 = oct2 * 10 + (*p - '0'); p++; }
    if (*p == '.' && oct2 >= 16 && oct2 <= 31)
      return TRUE;
  }

  return FALSE;
}

/* Public function: see header for full contract. */
gboolean
umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap)
{
  /* Validate input URL pointer. */
  if (!url || !*url)
  {
    write_err(errbuf, errcap, "empty URL");
    return FALSE;
  }

  /* Parse URL into a structured GUri. We use strict-enough flags for safety. */
  GError *perr = NULL;                       /* local parse error pointer           */
  GUri   *uri  = g_uri_parse(url, G_URI_FLAGS_NONE, &perr);  /* parse text URL          */

  if (!uri)
  {
    /* Parsing failed: provide GLib’s diagnostic if available. */
    write_err(errbuf, errcap, perr && perr->message ? perr->message : "invalid URL");
    if (perr) g_error_free(perr);            /* free parse error                    */
    return FALSE;                            /* reject                              */
  }

  /* At this point, we have a valid GUri*. Pull out scheme and host for checks. */
  const char *scheme = g_uri_get_scheme(uri);/* may be NULL for strange inputs      */
  const char *host   = g_uri_get_host(uri);  /* may be NULL if URL lacks host       */

  /* Enforce scheme allow-list: only http / https are permitted by default. */
  if (!scheme ||
      (g_ascii_strcasecmp(scheme, "http") != 0 &&
       g_ascii_strcasecmp(scheme, "https") != 0))
  {
    write_err(errbuf, errcap, "blocked: only http/https schemes are allowed");
    g_uri_unref(uri);                        /* drop parsed object                  */
    return FALSE;                            /* reject                              */
  }

  /* Protect against local/loopback/private endpoints by default. */
  if (is_loopback_host(host) || is_private_v4_host(host))
  {
    write_err(errbuf, errcap, "blocked: local/loopback/private host is not allowed");
    g_uri_unref(uri);
    return FALSE;
  }

  /* All checks passed. Clean up and allow. */
  g_uri_unref(uri);                          /* free parsed GUri                    */
  write_err(errbuf, errcap, "");             /* optional: write empty message       */
  return TRUE;                               /* permitted                           */
}
/*--- end of file ---*/
