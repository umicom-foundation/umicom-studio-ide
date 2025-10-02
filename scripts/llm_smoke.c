#include "umicom/llm.h"
#include <glib.h>
int main() {
  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg);
  gchar *out=NULL; gchar err[256]={0};
  if (umi_llm_chat_simple(&cfg, "You are a helpful coding assistant.", "Say hello in one sentence.", &out, err, sizeof err)) {
    g_print("%s\n", out); g_free(out); return 0;
  } else {
    g_printerr("Error: %s\n", err); return 1;
  }
}
