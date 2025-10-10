/*-----------------------------------------------------------------------------
 * Umicom Studio IDE (USIDE)
 * File: crash_guard.c
 * PURPOSE: Crash guard: stub writer (silence unused params).
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 09-10-2025
 * License: MIT
 *---------------------------------------------------------------------------*/

#include <glib.h>

static GLogWriterOutput writer(GLogLevelFlags level, const GLogField *fields, gsize n_fields, gpointer user_data){
    (void)level; (void)fields; (void)n_fields; (void)user_data;
    return G_LOG_WRITER_HANDLED;
}
