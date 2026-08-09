/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/settings_main.c
 *
 * PURPOSE:
 *   Provide a native C23 command for listing, reading, validating, changing and
 *   resetting Studio's typed settings.  The command uses the same schema and
 *   Framework repository as the GTK4 and console frontends.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/settings.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void print_usage(const char *program)
{
    (void)printf(
        "Umicom Studio typed settings\n\n"
        "Usage:\n"
        "  %s [--file PATH] list\n"
        "  %s [--file PATH] get KEY\n"
        "  %s [--file PATH] set KEY VALUE\n"
        "  %s [--file PATH] reset KEY\n"
        "  %s [--file PATH] reset-all\n"
        "  %s [--file PATH] validate\n",
        program,
        program,
        program,
        program,
        program,
        program
    );
}

static int load_settings(UmiSettings *settings, const char *path)
{
    int loaded = 0;
    UmiStatus status = umi_studio_settings_load_if_present(settings,
                                                           path,
                                                           &loaded);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Could not load settings file '%s': %s\n",
                      path,
                      umi_status_text(status));
        return 0;
    }

    if (loaded == 0) {
        (void)printf("Settings file not found; using defaults: %s\n", path);
    }
    return 1;
}

static int list_settings(UmiSettings *settings)
{
    size_t index;

    for (index = 0U; index < umi_settings_count(settings); ++index) {
        UmiSettingDefinitionView definition;
        char value[UMI_SETTING_VALUE_CAPACITY];
        UmiStatus status = umi_settings_definition_at(settings,
                                                       index,
                                                       &definition);
        if (status != UMI_STATUS_OK) {
            return 0;
        }
        status = umi_settings_get_text(settings,
                                       definition.key,
                                       value,
                                       sizeof(value));
        if (status != UMI_STATUS_OK) {
            return 0;
        }

        (void)printf("%s [%s] = %s\n",
                     definition.key,
                     umi_setting_type_text(definition.type),
                     value);
        if (definition.description != NULL &&
            definition.description[0] != '\0') {
            (void)printf("  %s\n", definition.description);
        }
    }
    return 1;
}

int main(int argc, char **argv)
{
    UmiSettings *settings = NULL;
    const char *path = umi_studio_settings_default_path();
    const char *command;
    int argument_index = 1;
    UmiStatus status;
    int result = EXIT_FAILURE;

    if (argc > 2 && strcmp(argv[argument_index], "--file") == 0) {
        path = argv[argument_index + 1];
        argument_index += 2;
    }

    if (argument_index >= argc || strcmp(argv[argument_index], "--help") == 0) {
        print_usage(argv[0]);
        return argument_index >= argc ? EXIT_FAILURE : EXIT_SUCCESS;
    }
    command = argv[argument_index++];

    status = umi_studio_settings_create(&settings);
    if (status != UMI_STATUS_OK) {
        (void)fprintf(stderr,
                      "Could not create Studio settings: %s\n",
                      umi_status_text(status));
        return EXIT_FAILURE;
    }

    if (!load_settings(settings, path)) {
        umi_settings_destroy(settings);
        return EXIT_FAILURE;
    }

    if (strcmp(command, "list") == 0) {
        result = list_settings(settings) ? EXIT_SUCCESS : EXIT_FAILURE;
    } else if (strcmp(command, "get") == 0 && argument_index < argc) {
        char value[UMI_SETTING_VALUE_CAPACITY];
        status = umi_settings_get_text(settings,
                                       argv[argument_index],
                                       value,
                                       sizeof(value));
        if (status == UMI_STATUS_OK) {
            (void)printf("%s\n", value);
            result = EXIT_SUCCESS;
        } else {
            (void)fprintf(stderr, "Unknown setting: %s\n", argv[argument_index]);
        }
    } else if (strcmp(command, "set") == 0 && argument_index + 1 < argc) {
        const char *key = argv[argument_index];
        const char *value = argv[argument_index + 1];
        status = umi_settings_set_text(settings, key, value);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_settings_save(settings, path);
        }
        if (status == UMI_STATUS_OK) {
            (void)printf("Updated %s in %s\n", key, path);
            result = EXIT_SUCCESS;
        } else {
            (void)fprintf(stderr,
                          "Could not update %s: %s\n",
                          key,
                          umi_status_text(status));
        }
    } else if (strcmp(command, "reset") == 0 && argument_index < argc) {
        const char *key = argv[argument_index];
        status = umi_settings_reset(settings, key);
        if (status == UMI_STATUS_OK) {
            status = umi_studio_settings_save(settings, path);
        }
        if (status == UMI_STATUS_OK) {
            (void)printf("Reset %s in %s\n", key, path);
            result = EXIT_SUCCESS;
        } else {
            (void)fprintf(stderr,
                          "Could not reset %s: %s\n",
                          key,
                          umi_status_text(status));
        }
    } else if (strcmp(command, "reset-all") == 0) {
        umi_settings_reset_all(settings);
        status = umi_studio_settings_save(settings, path);
        if (status == UMI_STATUS_OK) {
            (void)printf("Reset all settings in %s\n", path);
            result = EXIT_SUCCESS;
        } else {
            (void)fprintf(stderr,
                          "Could not save %s: %s\n",
                          path,
                          umi_status_text(status));
        }
    } else if (strcmp(command, "validate") == 0) {
        (void)printf("Settings are valid: %zu recognised keys\n",
                     umi_settings_count(settings));
        result = EXIT_SUCCESS;
    } else {
        print_usage(argv[0]);
    }

    umi_settings_destroy(settings);
    return result;
}
