/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: src/settings_bus.c
 * PURPOSE: Implementation of settings signal bus
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-01 | MIT
 *---------------------------------------------------------------------------*/

#include "include/settings_bus.h"

struct _UmiSettingsBus {
  GObject parent_instance;
};

G_DEFINE_TYPE(UmiSettingsBus, umi_settings_bus, G_TYPE_OBJECT)

enum { SIG_CHANGED, N_SIGS };
static guint signals[N_SIGS];

static void umi_settings_bus_class_init(UmiSettingsBusClass *klass){
  signals[SIG_CHANGED] = g_signal_new(
    "changed",
    G_TYPE_FROM_CLASS(klass),
    G_SIGNAL_RUN_LAST,
    0, NULL, NULL, NULL,
    G_TYPE_NONE, 2, G_TYPE_STRING, G_TYPE_STRING /* key, value */);
}

static void umi_settings_bus_init(UmiSettingsBus *self){ (void)self; }

UmiSettingsBus *umi_settings_bus_get(void){
  static UmiSettingsBus *singleton=NULL;
  if(!singleton) singleton = g_object_new(UMI_TYPE_SETTINGS_BUS, NULL);
  return singleton;
}

void umi_settings_emit(UmiSettingsBus *bus, const char *key, const char *value){
  if(!bus) bus = umi_settings_bus_get();
  g_signal_emit(bus, signals[SIG_CHANGED], 0, key?key:"", value?value:"");
}
