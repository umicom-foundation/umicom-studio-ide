/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/log.h
 * PURPOSE: Logging helpers with domains and levels
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_LOG_H
#define UMICOM_LOG_H

#include <glib.h>

/* Usage:
 *   UMI_LOG_I("app", "started with %d args", argc);
 *   UMI_LOG_W("fs",  "missing file: %s", path);
 *   UMI_LOG_E("net", "connection failed: %s", err->message);
 */
#define UMI_LOG_I(dom, fmt, ...) g_log(dom, G_LOG_LEVEL_INFO,     fmt, ##__VA_ARGS__)
#define UMI_LOG_W(dom, fmt, ...) g_log(dom, G_LOG_LEVEL_WARNING,  fmt, ##__VA_ARGS__)
#define UMI_LOG_E(dom, fmt, ...) g_log(dom, G_LOG_LEVEL_ERROR,    fmt, ##__VA_ARGS__)
#define UMI_LOG_D(dom, fmt, ...) g_log(dom, G_LOG_LEVEL_DEBUG,    fmt, ##__VA_ARGS__)

void umi_log_set_verbosity(gboolean debug_enabled);

#endif /* UMICOM_LOG_H */
