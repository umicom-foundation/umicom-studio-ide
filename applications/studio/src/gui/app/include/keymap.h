/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/gui/app/include/keymap.h
 *
 * PURPOSE:
 *   Central header defining the keymap callback structure. This avoids
 *   duplicate struct definitions across modules and provides a single
 *   source of truth for how actions are wired up.
 *
 * DESIGN:
 *   - Kept minimal and stable
 *   - Pure C, no dependencies beyond glib
 *   - Used by app_actions.c to fill callbacks for the app shell
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMI_KEYMAP_H
#define UMI_KEYMAP_H

#include <glib.h>

G_BEGIN_DECLS

/*-----------------------------------------------------------------------------
 * UmiActionCallback
 *
 * Standard callback signature for all application actions.
 * Each action receives a gpointer to arbitrary user data (context).
 *---------------------------------------------------------------------------*/
typedef void (*UmiActionCallback)(gpointer user_data);

/*-----------------------------------------------------------------------------
 * UmiKeymapCallbacks
 *
 * Central structure collecting all application-level action callbacks.
 * This is filled by umi_app_fill_keymap() in app_actions.c and used by
 * the app shell to wire up menus, toolbar buttons, and keyboard shortcuts.
 *
 * FIELDS:
 *   palette      - Show command palette / quick search
 *   save         - Save current document
 *   save_as      - Save current document with new name
 *   run          - Start the run/build pipeline
 *   stop         - Stop the run/build pipeline
 *   focus_search - Move focus to search bar
 *---------------------------------------------------------------------------*/
typedef struct {
    UmiActionCallback palette;
    UmiActionCallback save;
    UmiActionCallback save_as;
    UmiActionCallback run;
    UmiActionCallback stop;
    UmiActionCallback focus_search;
} UmiKeymapCallbacks;

G_END_DECLS

#endif /* UMI_KEYMAP_H */