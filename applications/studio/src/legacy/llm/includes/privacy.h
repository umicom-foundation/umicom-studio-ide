/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/includes/privacy.h
 *
 * PURPOSE:
 *   Public interface for URL privacy checks used by LLM HTTP code. We validate
 *   that outgoing requests are safe to perform (e.g., only http/https schemes,
 *   not targeting loopback or private networks unless explicitly allowed).
 *
 * DESIGN CHOICES:
 *   - Single stable function signature (no conflicting prototypes).
 *   - GLib-friendly: returns gboolean, uses plain C buffers for error strings.
 *   - Loosely coupled: only depends on <glib.h>; no cross-module includes.
 *
 * HOW IT WORKS (high level):
 *   - Parse the URL via GLib’s GUri.
 *   - Allow schemes: "http" and "https" (reject others).
 *   - Reject obvious local/loopback/private targets by default:
 *       localhost, 127.0.0.0/8, ::1, 10.0.0.0/8, 192.168.0.0/16,
 *       172.16.0.0/12 (172.16.x.x .. 172.31.x.x)
 *   - On reject, write a human-readable reason into errbuf (if provided).
 *
 * THREADING:
 *   - Purely CPU-bound checks; safe to call from any thread.
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-12 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_PRIVACY_H
#define UMICOM_PRIVACY_H

/* Include GLib for gboolean and basic types. */
#include <glib.h>  /* provides gboolean, TRUE/FALSE, gsize, etc. */

/*-----------------------------------------------------------------------------
 * umi_privacy_allow_url
 *
 * PURPOSE:
 *   Validate whether 'url' is allowed to be accessed by the application.
 *
 * PARAMETERS:
 *   url     - (in)  NUL-terminated URL string to check. Must be non-NULL.
 *   errbuf  - (out) Optional buffer to receive a short explanation on failure.
 *                  May be NULL if caller does not need the error text.
 *   errcap  - (in)  Size in bytes of 'errbuf'. Ignored if errbuf==NULL.
 *
 * RETURNS:
 *   TRUE  => URL is allowed (passes privacy policy).
 *   FALSE => URL is rejected; errbuf (if provided) explains why.
 *
 * NOTES:
 *   - This function does **not** perform I/O; it only validates the string.
 *   - Callers should fail fast when this returns FALSE.
 *---------------------------------------------------------------------------*/
gboolean umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap);

#endif /* UMICOM_PRIVACY_H */
/*--- end of file ---*/
