#include "umicom/llm.h"
#include <assert.h>
#include <stdio.h>
int main(void){
  UmiLlmCfg cfg; umi_llm_cfg_init_from_env(&cfg);
  (void)cfg; /* Ensure symbol is found and header compiles */
  puts("ok");
  return 0;
}
