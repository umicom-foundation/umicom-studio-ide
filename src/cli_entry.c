/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/cli_entry.c
 * PURPOSE: Optional CLI entry: parse options, set log level, print hello
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "options.h"
#include "log.h"

int main(int argc, char **argv){
  GError *err = NULL;
  UmiOptions *opt = umi_options_parse(&argc, &argv, &err);
  if(!opt){
    g_printerr("Options error: %s\n", err?err->message:"(unknown)");
    if(err) g_error_free(err);
    return 2;
  }
  /* Map --log 0..3 to our logger */
  if(opt->log_level <= 0)      umi_log_set_level(UMI_LOG_DEBUG);
  else if(opt->log_level == 1) umi_log_set_level(UMI_LOG_INFO);
  else if(opt->log_level == 2) umi_log_set_level(UMI_LOG_WARN);
  else                         umi_log_set_level(UMI_LOG_ERROR);

  UMI_LOGI("Umicom Studio CLI starting (headless=%d, workspace=%s)",
           opt->headless, opt->workspace?opt->workspace:"(none)");
  UMI_LOGD("Verbose mode: %d", opt->verbose);

  /* This file is not wired into the GUI app; it exists to provide a tiny
     compile-ready entry point for smoke testing the toolchain. */

  umi_options_free(opt);
  return 0;
}
