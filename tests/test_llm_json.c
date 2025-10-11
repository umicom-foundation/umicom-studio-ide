/* -----------------------------------------------------------------------------
 * Umicom Studio IDE
 * PURPOSE: Core sources for Umicom Studio IDE.
 * Created by: Umicom Foundation | Author: Sammy Hegab | License: MIT
 * Last updated: 2025-10-11
 * ---------------------------------------------------------------------------*/
#include <json-glib/json-glib.h>
#include <stdio.h>
int main(void){
  /* sanity: json-glib available and can parse tiny object */
  const char *s = "{\"hello\":\"world\"}";
  JsonParser *p = json_parser_new();
  if(!json_parser_load_from_data(p, s, -1, NULL)){ puts("fail"); return 1; }
  puts("ok");
  g_object_unref(p);
  return 0;
}
