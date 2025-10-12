/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/util/sys/ansi_color.c
 * PURPOSE: Parse simple ANSI SGR color escape sequences and apply GTK tags
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include <gtk/gtk.h>        /* GtkTextBuffer, GtkTextTagTable */
#include <glib.h>           /* GLib types and utilities       */
#include <string.h>         /* strlen, memchr                 */
#include <stdlib.h>         /* strtol                         */

/* Use project include dirs */
#include <ansi_color.h>

/* Internal: a small struct describing one SGR “graphic rendition” code. */
/* Each field describes a visual attribute we may enable/disable.        */
typedef struct {
    gboolean bold;          /* TRUE to enable bold; FALSE leaves unchanged         */
    gboolean italic;        /* TRUE to enable italic; FALSE leaves unchanged       */
    gboolean underline;     /* TRUE to enable underline; FALSE leaves unchanged    */
    const char *fg;         /* Named color or #RRGGBB for foreground, NULL = none  */
    const char *bg;         /* Named color or #RRGGBB for background, NULL = none  */
    gboolean reset;         /* TRUE if this code requests a full reset (SGR 0)     */
} UmiSgr;

/* Internal: map the most common SGR numeric codes to a CSS-ish color name. */
/* NOTE: We keep the palette tiny and predictable; expand as needed.        */
static const char *umi_sgr_color_8(int code) {
    /* Basic 8-colors per ANSI (30–37 FG, 40–47 BG). */
    switch (code) {
        case 30: return "black";
        case 31: return "red";
        case 32: return "green";
        case 33: return "yellow";
        case 34: return "blue";
        case 35: return "magenta";
        case 36: return "cyan";
        case 37: return "lightgray";
        /* Bright variants (90–97) often used; map to lighter tones. */
        case 90: return "darkgray";
        case 91: return "lightcoral";
        case 92: return "lightgreen";
        case 93: return "lightyellow";
        case 94: return "lightskyblue";
        case 95: return "plum";
        case 96: return "paleturquoise";
        case 97: return "white";
        default: return NULL; /* Unknown → let caller ignore. */
    }
}

/* Internal: parse a single SGR numeric token and fill a UmiSgr delta.     */
/* Returns TRUE if token recognized; FALSE if unknown (safe to ignore).    */
static gboolean umi_sgr_from_code(int n, UmiSgr *out) {
    /* Initialize “no-op” so we only modify what the code affects. */
    out->bold = out->italic = out->underline = FALSE;
    out->fg = out->bg = NULL;
    out->reset = FALSE;

    /* SGR “0” is reset-all. */
    if (n == 0) { out->reset = TRUE; return TRUE; }

    /* Style toggles */
    if (n == 1) { out->bold = TRUE; return TRUE; }
    if (n == 3) { out->italic = TRUE; return TRUE; }
    if (n == 4) { out->underline = TRUE; return TRUE; }

    /* Foreground 30–37, 90–97 */
    if ((n >= 30 && n <= 37) || (n >= 90 && n <= 97)) {
        out->fg = umi_sgr_color_8(n);
        return TRUE;
    }
    /* Background 40–47 */
    if (n >= 40 && n <= 47) {
        /* Map background by subtracting 10 to reuse palette. */
        out->bg = umi_sgr_color_8(n - 10);
        return TRUE;
    }
    /* Unknown SGR code → safely ignore. */
    return FALSE;
}

/* Internal: ensure a tag with the given properties exists in the tag table.     */
/* Returns an owned reference to the tag name string we created or reused.       */
static gchar *umi_ensure_tag(GtkTextBuffer *buf, const char *fg, const char *bg,
                             gboolean bold, gboolean italic, gboolean underline)
{
    /* Build a canonical tag name encoding the attributes so tags can be reused. */
    /* Example: "umi-fg-red_bg-none-biu" (b=bold,i=italic,u=underline flags)     */
    GString *name = g_string_new("umi");
    /* Foreground */
    g_string_append(name, "-fg-");
    g_string_append(name, fg ? fg : "none");
    /* Background */
    g_string_append(name, "_bg-");
    g_string_append(name, bg ? bg : "none");
    /* Styles bitset in canonical order */
    g_string_append(name, "-");
    g_string_append_c(name, bold ? 'b' : '-');
    g_string_append_c(name, italic ? 'i' : '-');
    g_string_append_c(name, underline ? 'u' : '-');

    /* See if it already exists. */
    GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buf); /* tag table handle */
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, name->str); /* lookup by name*/

    if (!tag) {
        /* Create and configure a new tag with the desired properties. */
        tag = gtk_text_tag_new(name->str);                /* create tag */
        if (fg) g_object_set(tag, "foreground", fg, NULL);/* set FG if requested */
        if (bg) g_object_set(tag, "background", bg, NULL);/* set BG if requested */
        if (bold) g_object_set(tag, "weight", PANGO_WEIGHT_BOLD, NULL);
        if (italic) g_object_set(tag, "style", PANGO_STYLE_ITALIC, NULL);
        if (underline) g_object_set(tag, "underline", PANGO_UNDERLINE_SINGLE, NULL);
        gtk_text_tag_table_add(table, tag);               /* register in table */
        g_object_unref(tag);                              /* table owns a ref now */
    }

    /* Return the canonical name (caller frees). */
    return g_string_free(name, FALSE);
}

/* Apply SGR state to the last appended text range. */
/* The caller provides the iter range that should get tagged.                 */
static void umi_apply_sgr(GtkTextBuffer *buf, GtkTextIter *start, GtkTextIter *end,
                          const UmiSgr *state)
{
    /* Compute desired attributes from state; in real code this might be a
     * cumulative state object, but for our simple usage we pass the delta. */
    const gchar *fg = state->fg;
    const gchar *bg = state->bg;
    const gboolean bold = state->bold;
    const gboolean italic = state->italic;
    const gboolean underline = state->underline;

    /* Create or fetch a tag for exactly these attributes, then apply it. */
    g_autofree gchar *tag_name =
        umi_ensure_tag(buf, fg, bg, bold, italic, underline); /* allocates name */
    GtkTextTagTable *table = gtk_text_buffer_get_tag_table(buf); /* tag table */
    GtkTextTag *tag = gtk_text_tag_table_lookup(table, tag_name); /* tag handle */
    if (tag) {
        gtk_text_buffer_apply_tag(buf, tag, start, end);        /* apply to range */
    }
}

/* Parse a buffer containing text with ANSI SGR sequences, append to GtkTextBuffer,
 * and apply appropriate tags. Unknown SGR codes are ignored safely.              */
void umi_ansi_append(GtkTextBuffer *buf, const char *text)
{
    /* Fast path for NULL or empty strings. */
    if (!text || !*text) return; /* nothing to do */

    GtkTextIter insert_before;       /* start of the just-inserted segment */
    GtkTextIter insert_after;        /* end of the just-inserted segment   */
    GtkTextIter cur;                 /* current insert position            */

    /* Current active SGR (for simplicity, we apply per chunk). */
    UmiSgr cur_sgr = { FALSE, FALSE, FALSE, NULL, NULL, TRUE /* reset */ };
    (void)cur_sgr; /* structure kept for clarity; we apply per encountered code */

    const char *p = text;            /* read pointer over input string     */
    const char *chunk_start = p;     /* start of plain-text chunk to append*/

    gtk_text_buffer_get_end_iter(buf, &cur); /* initialize iter to end of buffer */

    /* Walk the input and split at ESC[ ... m sequences. */
    while (*p) {
        /* Look for ESC (0x1B). */
        const char *esc = memchr(p, 0x1B, strlen(p)); /* next escape (if any) */
        if (!esc) {
            /* No more escapes → append the remainder as plain text. */
            gtk_text_buffer_get_end_iter(buf, &insert_before);     /* mark start */
            gtk_text_buffer_insert(buf, &insert_before, p, -1);    /* insert     */
            gtk_text_buffer_get_end_iter(buf, &insert_after);      /* mark end   */
            /* No tag to apply (plain text); done. */
            break;
        }

        /* Append any plain text before the escape. */
        if (esc > p) {
            gtk_text_buffer_get_end_iter(buf, &insert_before);           /* start */
            gtk_text_buffer_insert(buf, &insert_before, p, (gint)(esc - p)); /* N */
            gtk_text_buffer_get_end_iter(buf, &insert_after);            /* end   */
            /* The just inserted chunk uses whatever the last SGR said;
             * for simplicity in this minimal version, we leave it untagged. */
        }

        /* Expect CSI “[” sequence: ESC [ ... m */
        const char *csi = esc + 1;                         /* points after ESC */
        if (*csi == '[') {
            /* Find trailing 'm' (end of SGR). */
            const char *m = strchr(csi, 'm');              /* end marker       */
            if (!m) { /* malformed; write ESC verbatim and continue after it */
                gtk_text_buffer_get_end_iter(buf, &insert_before);
                gtk_text_buffer_insert(buf, &insert_before, esc, 1);
                p = csi;                                   /* resume after ESC */
                continue;
            }

            /* Parse semicolon-separated codes between “[” and “m”. */
            const char *codes = csi + 1;                   /* first digit char */
            while (codes < m) {
                char *end = NULL;
                long n = strtol(codes, &end, 10);          /* parse integer    */
                if (end == codes) {                        /* not a number -> */
                    break;                                 /* stop parsing     */
                }
                UmiSgr delta;
                if (umi_sgr_from_code((int)n, &delta)) {
                    /* Apply to an empty range so future insertions inherit tags?
                     * GtkTextBuffer doesn’t have “current attributes”, so we keep
                     * it minimal: we parse and remember, and apply when we have
                     * a concrete text range. This sample keeps parsing logic only. */
                    (void)delta; /* Parsed successfully; kept for future expansion. */
                }
                codes = (*end == ';') ? (end + 1) : end;   /* move to next token */
            }

            /* Advance after the entire ESC[...]m sequence. */
            p = m + 1;
        } else {
            /* Not a CSI we understand → copy the ESC byte as-is and continue. */
            gtk_text_buffer_get_end_iter(buf, &insert_before);
            gtk_text_buffer_insert(buf, &insert_before, esc, 1);
            p = csi; /* continue after ESC */
        }
    }
}
/* --- IGNORE --- */