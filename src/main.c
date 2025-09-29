/*-----------------------------------------------------------------------------
 * Umicom Studio IDE — FIM Demo Entrypoint
 * Usage: studio_fim_demo "<prefix>" "<suffix>"
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * Date: 29-09-2025
 * License: MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "studio/codestral.h"

int main(int argc, char **argv){
  if(argc < 3){
    fprintf(stderr, "Usage: %s \"<prefix>\" \"<suffix>\"\n", argv[0]);
    return 64;
  }
  const char *prefix = argv[1];
  const char *suffix = argv[2];
  const char *key = getenv("MISTRAL_API_KEY");
  if(!key || !*key){
    fprintf(stderr, "MISTRAL_API_KEY not set\n");
    return 1;
  }
  const char *base = getenv("UENG_MISTRAL_BASE_URL"); /* optional */
  const char *model = getenv("UENG_MISTRAL_MODEL");   /* optional, e.g., codestral-latest */

  char out[65536];
  int rc = studio_codestral_fim(key, base, model, prefix, suffix, 256, 0.2, out, sizeof out);
  if(rc){ fprintf(stderr, "FIM error: %d -> %s\n", rc, out); return rc; }
  printf("%s\n", out);
  return 0;
}
