/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/llm/include/privacy.h
 * PURPOSE: Public interface for privacy guardrails governing outbound URLs.
 *          - Query whether the IDE is running in “local-only” privacy mode.
 *          - Validate whether a given URL is permitted under the current mode.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT
 *---------------------------------------------------------------------------*/
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */
#ifndef UMICOM_PRIVACY_H
#define UMICOM_PRIVACY_H

#include <stdbool.h>  /* bool, true, false */

/*-----------------------------------------------------------------------------
 * Overview
 * -------
 * This header exposes two small helpers used throughout the networking layer:
 *
 *   1) umi_privacy_is_local_only()
 *      Returns whether “local-only” privacy mode is active. In that mode,
 *      the IDE should avoid talking to remote hosts on the public internet.
 *      The implementation typically decides this based on environment/config
 *      (e.g., variables like UMI_PRIVACY_MODE=local-only or
 *      UMI_PRIVACY_LOCAL_ONLY=1/true/yes). See llm_privacy.c for details.
 *
 *   2) umi_privacy_allow_url(url, errbuf, errcap)
 *      Given a URL, tells you if it is allowed to be used right now:
 *      - file:// URLs are always allowed (purely local file access).
 *      - If local-only mode is ON, only loopback http(s) endpoints are allowed
 *        (e.g., hosts like “localhost”, “127.0.0.1”, or “::1”).
 *      - If local-only mode is OFF, all syntactically valid URLs pass here
 *        (further policy checks may still happen elsewhere if needed).
 *
 * Thread-safety & Cost
 * --------------------
 * Both functions are designed to be cheap and safe to call from any thread.
 * Implementations cache/parse lightweight state and do not perform network I/O.
 *
 * Return Semantics
 * ----------------
 * All functions return 'bool' (true on success/allowed, false otherwise).
 * When a function returns false and you provided an error buffer, a short,
 * human-readable reason is written to that buffer (always NUL-terminated).
 *
 * Notes for Callers
 * -----------------
 * - Always pass absolute URLs to umi_privacy_allow_url().
 * - errbuf may be NULL if you do not need an error string.
 * - errcap is the total capacity of errbuf (including space for the '\0').
 * - The implementation uses GLib internally but does not expose it here.
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * umi_privacy_is_local_only
 * -------------------------
 * Query whether the IDE is currently in “local-only” privacy mode.
 *
 * Typical sources of truth (implementation detail; listed here for clarity):
 * - Environment variables:
 *     - UMI_PRIVACY_MODE=local-only   → enables local-only mode
 *     - UMI_PRIVACY_LOCAL_ONLY=1/true/yes → enables local-only mode
 *
 * Returns:
 *   true  → local-only mode is enabled (restrict outbound networking)
 *   false → normal mode (no extra host restrictions from this helper)
 */
bool umi_privacy_is_local_only(void);

/**
 * umi_privacy_allow_url
 * ---------------------
 * Validate whether 'url' is permitted under the current privacy policy.
 *
 * Behavior (high level):
 *   - file:// URLs are always allowed.
 *   - In local-only mode:
 *       * http:// and https:// are allowed ONLY if the host is loopback
 *         (e.g., “localhost”, “127.0.0.1”, “::1”). All other hosts are denied.
 *   - In normal mode:
 *       * Any syntactically valid URL passes this gate.
 *
 * Parameters:
 *   url    : (in)  A NUL-terminated string with the absolute URL to validate.
 *                  Must not be NULL. Undefined behavior if NULL.
 *   errbuf : (out) Optional buffer to receive a short reason on failure.
 *                  Pass NULL if you do not need error text.
 *   errcap : (in)  Capacity of 'errbuf' in bytes (including space for '\0').
 *                  Ignored if errbuf is NULL. If > 0, the function always
 *                  writes a NUL-terminated string on failure.
 *
 * Returns:
 *   true  → The URL is allowed right now.
 *   false → The URL is NOT allowed; if 'errbuf' was provided, it contains
 *           a human-readable explanation (truncated to fit 'errcap').
 *
 * Examples:
 *   char why[128];
 *   if (!umi_privacy_allow_url("https://api.example.com", why, sizeof why)) {
 *     // Denied under current settings; 'why' explains why.
 *   }
 */
bool umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap);

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* UMICOM_PRIVACY_H */
/*---------------------------------------------------------------------------*/