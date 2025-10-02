
#include "umicom/llm.h"
#include <assert.h>
#include <stdio.h>

int main(void){
  const char *line = "{\"choices\":[{\"delta\":{\"content\":\"Hel\"},\"logprobs\":{\"content\":[{\"token\":\"Hel\",\"logprob\":-0.1,\"top_logprobs\":[{\"token\":\"Hel\",\"logprob\":-0.1},{\"token\":\"Hal\",\"logprob\":-1.2}]}]}}]}";
  gchar *frag=NULL; UmiLlmTokenAlt *alts=NULL; guint n=0;
  bool ok = umi_llm_debug_parse_openai_sse_line(line, &frag, &alts, &n);
  assert(ok);
  assert(frag && n>=1);
  printf("frag=%s n=%u\n", frag, n);
  umi_llm_free_alts(alts, n);
  g_free(frag);
  puts("ok");
  return 0;
}
