/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_settings.c
 *
 * PURPOSE:
 *   Verify the Studio schema defaults, typed validation, persistence and reset
 *   behaviour through the Framework UmiSettings implementation.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/settings.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int main(void)
{
    UmiSettings *settings = NULL;
    UmiSettings *loaded = NULL;
    char text[UMI_SETTING_VALUE_CAPACITY];
    int boolean_value = 0;
    int64_t integer_value = 0;
    int loaded_from_file = 0;
    const char *path = "umicom-studio-settings-test.conf";

    assert(umi_studio_settings_create(&settings) == UMI_STATUS_OK);
    assert(settings != NULL);
    assert(umi_settings_count(settings) == UMI_STUDIO_SETTING_COUNT);

    assert(umi_settings_get_text(settings,
                                 UMI_STUDIO_SETTING_UI_THEME,
                                 text,
                                 sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "system") == 0);

    assert(umi_settings_get_integer(settings,
                                    UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 4);
    assert(umi_settings_set_integer(settings,
                                    UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
                                    8) == UMI_STATUS_OK);
    assert(umi_settings_set_integer(settings,
                                    UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
                                    100) == UMI_STATUS_INVALID_ARGUMENT);

    assert(umi_settings_get_boolean(settings,
                                    UMI_STUDIO_SETTING_AI_ENABLED,
                                    &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 1);
    assert(umi_settings_set_boolean(settings,
                                    UMI_STUDIO_SETTING_AI_ENABLED,
                                    0) == UMI_STATUS_OK);

    assert(umi_settings_get_text(
        settings, UMI_STUDIO_SETTING_AUTHORENGINE_EXECUTABLE,
        text, sizeof(text)) == UMI_STATUS_OK);
    assert(strcmp(text, "uaengine") == 0);
    assert(umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_AI_CONTEXT_TOKENS,
        &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 32768);
    assert(umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_AI_ALLOW_REMOTE,
        &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 0);

    assert(umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_AI_CODING_CONTEXT_TOKENS,
        &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 16384);
    assert(umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_AI_CODING_REQUIRE_APPROVAL,
        &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 1);

    assert(umi_studio_settings_save(settings, path) == UMI_STATUS_OK);
    assert(umi_studio_settings_create(&loaded) == UMI_STATUS_OK);
    assert(umi_studio_settings_load_if_present(loaded,
                                               path,
                                               &loaded_from_file) == UMI_STATUS_OK);
    assert(loaded_from_file == 1);
    assert(umi_settings_get_integer(loaded,
                                    UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 8);
    assert(umi_settings_get_boolean(loaded,
                                    UMI_STUDIO_SETTING_AI_ENABLED,
                                    &boolean_value) == UMI_STATUS_OK);
    assert(boolean_value == 0);

    assert(umi_settings_reset(loaded,
                              UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH) == UMI_STATUS_OK);
    assert(umi_settings_get_integer(loaded,
                                    UMI_STUDIO_SETTING_EDITOR_TAB_WIDTH,
                                    &integer_value) == UMI_STATUS_OK);
    assert(integer_value == 4);

    umi_settings_destroy(loaded);
    umi_settings_destroy(settings);
    (void)remove(path);
    return 0;
}
