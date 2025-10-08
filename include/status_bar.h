/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: include/status_bar.h
 * PURPOSE: Back-compat shim mapping umi_status_bar_* to umi_status_*.
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-08 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMI_STATUS_BAR_H
#define UMI_STATUS_BAR_H
#include "status_util.h"
#define umi_status_bar_flash  umi_status_flash
typedef UmiStatus UmiStatusBar;
#endif /* UMI_STATUS_BAR_H */
