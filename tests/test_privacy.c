/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
#include <umicom/privacy.h>
#include <stdio.h>
int main(void){
  char err[128];
  int a = umi_privacy_allow_url("http://localhost:11434/v1/chat/completions", err, sizeof err);
  int b = umi_privacy_allow_url("http://127.0.0.1:8080/v1/chat/completions", err, sizeof err);
  printf("local checks: %d %d\n", a, b);
  puts("ok");
  return 0;
}
