/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/include/ansi_color.h
 * PURPOSE: Tiny ANSI color helpers for console output
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_ANSI_COLOR_H
#define UMICOM_ANSI_COLOR_H

/* These are simple, literal ANSI escape sequences. We intentionally DO NOT
 * add runtime detection here; higher-level code may decide when to emit them. */
#define ANSI_ESC        "\x1b["     /* CSI introducer, e.g. "\x1b[31m"            */
#define ANSI_RESET      "\x1b[0m"   /* Reset all attributes to defaults           */
#define ANSI_BOLD       "\x1b[1m"   /* Bold (or increased intensity)              */
#define ANSI_DIM        "\x1b[2m"   /* Faint/dim intensity                        */
#define ANSI_ITALIC     "\x1b[3m"   /* Italic (not widely supported on Windows)   */
#define ANSI_UNDERLINE  "\x1b[4m"   /* Underline                                  */

/* 8 basic foreground colors. Compose with ANSI_ESC + code + "m"
 * Example: printf(ANSI_ESC "31m" "error" ANSI_RESET "\n"); */
#define ANSI_FG_BLACK   "30"
#define ANSI_FG_RED     "31"
#define ANSI_FG_GREEN   "32"
#define ANSI_FG_YELLOW  "33"
#define ANSI_FG_BLUE    "34"
#define ANSI_FG_MAGENTA "35"
#define ANSI_FG_CYAN    "36"
#define ANSI_FG_WHITE   "37"

/* 8 basic background colors (same numbering offset to background plane). */
#define ANSI_BG_BLACK   "40"
#define ANSI_BG_RED     "41"
#define ANSI_BG_GREEN   "42"
#define ANSI_BG_YELLOW  "43"
#define ANSI_BG_BLUE    "44"
#define ANSI_BG_MAGENTA "45"
#define ANSI_BG_CYAN    "46"
#define ANSI_BG_WHITE   "47"

/*-----------------------------------------------------------------------------
 * ansi_color_enabled
 *
 * PURPOSE:
 *   Return non-zero when using ANSI color sequences is safe for the current
 *   output stream/environment (e.g. a terminal). Typically implemented in
 *   a .c file; this header declares it so call sites don’t guess.
 *
 * RETURNS:
 *   int (0 = disabled, non-zero = enabled).
 *---------------------------------------------------------------------------*/
int ansi_color_enabled(void);

/*-----------------------------------------------------------------------------
 * ansi_wrap (convenience)
 *
 * PURPOSE:
 *   Given a raw string and a color code (e.g., ANSI_FG_RED), return a newly
 *   allocated string wrapped with the corresponding ANSI escapes when enabled.
 *
 * RETURNS:
 *   char* that must be free()’d by the caller, or NULL on allocation failure.
 *   If color is disabled, returns a duplicate of 'text' (no escapes).
 *---------------------------------------------------------------------------*/
char *ansi_wrap(const char *text, const char *fg_code);

#endif /* UMICOM_ANSI_COLOR_H */
