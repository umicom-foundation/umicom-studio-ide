/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/workbench_views.c
 *
 * PURPOSE:
 *   Provide live Studio view models for the unified Framework workbench so the
 *   Activity Bar opens useful product data rather than placeholder view-type
 *   labels.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * These factories expose ordinary UmiUiViewModel properties. They never create
 * GTK widgets. The Framework GTK4 adapter can render them today, while a future
 * Qt, Wt or headless adapter can render the same models without rewriting the
 * Studio services underneath them.
 */

#include "umicom/studio/workbench_views.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "umicom/studio/build.h"
#include "umicom/studio/debugger.h"
#include "umicom/studio/designer.h"
#include "umicom/studio/source_control.h"
#include "umicom/studio/tests.h"

#define VIEW_EXPLORER      "studio.project-explorer"
#define VIEW_SEARCH        "studio.search"
#define VIEW_SOURCE_CTRL   "studio.source-control"
#define VIEW_RUN_DEBUG     "studio.run-debug"
#define VIEW_TESTING       "studio.testing"
#define VIEW_DESIGNER      "studio.designer"
#define VIEW_APPLICATIONS  "studio.application-hub"
#define VIEW_FRAMEWORK     "studio.framework"
#define VIEW_AI            "studio.authorengine"

typedef UmiStatus (*StudioViewCreateFn)(
    const char *view_id,
    void *user_data,
    UmiUiViewModel **out_view
);

static UmiStatus property_string(UmiUiViewModel *view,
                                 const char *key,
                                 const char *text)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_string(&value, text != NULL ? text : "");
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus property_integer(UmiUiViewModel *view,
                                  const char *key,
                                  int64_t number)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_integer(&value, number);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus property_boolean(UmiUiViewModel *view,
                                  const char *key,
                                  int enabled)
{
    UmiUiValue value;
    UmiStatus status = umi_ui_value_set_boolean(&value, enabled);
    return status == UMI_STATUS_OK
        ? umi_ui_view_model_set_property(view, key, &value)
        : status;
}

static UmiStatus create_base_view(const char *view_id,
                                  const char *view_type,
                                  const char *title,
                                  const char *summary,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;

    status = umi_ui_view_model_create(
        view_id, view_type, UMI_UI_ROLE_PANE, out_view);
    if (status != UMI_STATUS_OK) return status;

    status = property_string(*out_view, "title", title);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "summary", summary);
    }
    if (status != UMI_STATUS_OK) {
        umi_ui_view_model_destroy(*out_view);
        *out_view = NULL;
    }
    return status;
}

static UmiStatus create_explorer(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiFileIndexStats stats;
    UmiFileIndex *index;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_EXPLORER,
        "Explorer",
        "Workspace files indexed by the reusable Framework file-index service.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    index = umi_studio_services_file_index(services);
    if (index == NULL) return property_boolean(*out_view, "available", 0);
    stats = umi_file_index_stats(index);

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "workspace", stats.root);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "indexed-files",
                                  (int64_t)stats.files);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "index-revision",
                                  (int64_t)stats.revision);
    }
    return status;
}

static UmiStatus create_search(const char *view_id,
                               void *user_data,
                               UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiFileIndexStats stats;
    UmiFileIndex *index;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_SEARCH,
        "Search",
        "Search the same indexed workspace used by Explorer and Quick Open.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    index = umi_studio_services_file_index(services);
    if (index == NULL) return property_boolean(*out_view, "available", 0);
    stats = umi_file_index_stats(index);

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "searchable-files",
                              (int64_t)stats.files);
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "root", stats.root);
    }
    return status;
}

static UmiStatus create_source_control(const char *view_id,
                                       void *user_data,
                                       UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioSourceControlSnapshot snapshot;
    UmiStudioSourceControlService *service;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_SOURCE_CTRL,
        "Source Control",
        "Provider-neutral repository state exposed by Studio over Framework VCS.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    service = umi_studio_services_source_control(services);
    (void)memset(&snapshot, 0, sizeof(snapshot));

    if (service != NULL &&
        umi_studio_source_control_service_snapshot(service, &snapshot) ==
            UMI_STATUS_OK) {
        status = property_boolean(*out_view, "available", snapshot.available);
        if (status == UMI_STATUS_OK) {
            status = property_string(*out_view, "branch", snapshot.branch);
        }
        if (status == UMI_STATUS_OK) {
            status = property_string(*out_view, "upstream", snapshot.upstream);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "changes",
                                      (int64_t)snapshot.changes);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "staged",
                                      (int64_t)snapshot.staged);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "ahead",
                                      (int64_t)snapshot.ahead);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "behind",
                                      (int64_t)snapshot.behind);
        }
        return status;
    }

    return property_boolean(*out_view, "available", 0);
}

static UmiStatus create_run_debug(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioBuildSnapshot build_snapshot;
    UmiStudioDebuggerSnapshot debug_snapshot;
    UmiStudioBuildService *build;
    UmiStudioDebuggerService *debugger;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_RUN_DEBUG,
        "Run and Debug",
        "Build-state and Debug Adapter Protocol state presented together.",
        out_view);

    if (status != UMI_STATUS_OK) return status;

    build = umi_studio_services_build(services);
    debugger = umi_studio_services_debugger(services);
    (void)memset(&build_snapshot, 0, sizeof(build_snapshot));
    (void)memset(&debug_snapshot, 0, sizeof(debug_snapshot));

    if (build != NULL &&
        umi_studio_build_service_snapshot(build, &build_snapshot) ==
            UMI_STATUS_OK) {
        status = property_string(*out_view, "build-directory",
                                 build_snapshot.build_directory);
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "build-history",
                                      (int64_t)build_snapshot.history_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "build-diagnostics",
                                      (int64_t)build_snapshot.diagnostic_count);
        }
        if (status != UMI_STATUS_OK) return status;
    }

    if (debugger != NULL &&
        umi_studio_debugger_service_snapshot(debugger, &debug_snapshot) ==
            UMI_STATUS_OK) {
        status = property_boolean(*out_view, "debugger-initialized",
                                  debug_snapshot.initialized);
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "breakpoints",
                                      (int64_t)debug_snapshot.breakpoint_count);
        }
        if (status == UMI_STATUS_OK) {
            status = property_integer(*out_view, "debug-messages-received",
                                      (int64_t)debug_snapshot.received_messages);
        }
    }
    return status;
}

static UmiStatus create_testing(const char *view_id,
                                void *user_data,
                                UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioTestSnapshot snapshot;
    UmiStudioTestService *service;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_TESTING,
        "Testing",
        "Discovered test suites and latest Framework testing summary.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    service = umi_studio_services_tests(services);
    (void)memset(&snapshot, 0, sizeof(snapshot));

    if (service == NULL ||
        umi_studio_test_service_snapshot(service, &snapshot) != UMI_STATUS_OK) {
        return property_boolean(*out_view, "available", 0);
    }

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "suites",
                                  (int64_t)snapshot.suite_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "tests",
                                  (int64_t)snapshot.test_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "passed",
                                  (int64_t)snapshot.passed);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "failed",
                                  (int64_t)snapshot.failed);
    }
    return status;
}

static UmiStatus create_designer(const char *view_id,
                                 void *user_data,
                                 UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioDesignerSnapshot snapshot;
    UmiStudioDesigner *designer;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_DESIGNER,
        "Visual Designer",
        "Framework declarative designer state, selection and history.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    designer = umi_studio_services_designer(services);
    (void)memset(&snapshot, 0, sizeof(snapshot));

    if (designer == NULL ||
        umi_studio_designer_snapshot(designer, &snapshot) != UMI_STATUS_OK) {
        return property_boolean(*out_view, "available", 0);
    }

    status = property_boolean(*out_view, "available", 1);
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "palette-items",
                                  (int64_t)snapshot.palette_items);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "selected-items",
                                  (int64_t)snapshot.selected_items);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "undo",
                                  (int64_t)snapshot.undo_count);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "redo",
                                  (int64_t)snapshot.redo_count);
    }
    return status;
}

static UmiStatus create_applications(const char *view_id,
                                     void *user_data,
                                     UmiUiViewModel **out_view)
{
    UmiStatus status;
    (void)user_data;

    status = create_base_view(
        view_id,
        VIEW_APPLICATIONS,
        "Applications",
        "Umicom application catalogue, runtime presence and safe launch planning.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "provider",
                                 "Studio Runtime Manager / Integration Fabric");
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "launch-mode",
                                 "planned and supervised");
    }
    return status;
}

static UmiStatus create_framework(const char *view_id,
                                  void *user_data,
                                  UmiUiViewModel **out_view)
{
    UmiStatus status;
    (void)user_data;

    status = create_base_view(
        view_id,
        VIEW_FRAMEWORK,
        "Umicom Framework",
        "Reusable C23 platform capabilities consumed by Studio and other products.",
        out_view);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "version",
                                 UMICOM_FRAMEWORK_VERSION_STRING);
    }
    if (status == UMI_STATUS_OK) {
        status = property_integer(*out_view, "abi",
                                  (int64_t)UMICOM_FRAMEWORK_ABI_VERSION);
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "language", "C23");
    }
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "ui-contract",
                                 "toolkit-neutral + GTK4 reference adapter");
    }
    return status;
}

static UmiStatus create_ai(const char *view_id,
                           void *user_data,
                           UmiUiViewModel **out_view)
{
    UmiStudioServices *services = (UmiStudioServices *)user_data;
    UmiStudioAiPlatform *platform;
    const char *provider;
    UmiStatus status = create_base_view(
        view_id,
        VIEW_AI,
        "AI / AuthorEngine",
        "Studio AI composition over the reusable Framework AI and Helix runtimes.",
        out_view);

    if (status != UMI_STATUS_OK) return status;
    platform = umi_studio_services_ai_platform(services);
    provider = platform != NULL
        ? umi_studio_ai_platform_default_provider(platform)
        : NULL;

    status = property_boolean(*out_view, "available", platform != NULL);
    if (status == UMI_STATUS_OK) {
        status = property_string(*out_view, "default-provider",
                                 provider != NULL ? provider : "not configured");
    }
    if (status == UMI_STATUS_OK) {
        status = property_boolean(*out_view, "helix-runtime",
                                  platform != NULL &&
                                  umi_studio_ai_platform_helix(platform) != NULL);
    }
    return status;
}

typedef struct StudioViewDefinition {
    const char *view_type;
    StudioViewCreateFn create;
} StudioViewDefinition;

static const StudioViewDefinition DEFINITIONS[] = {
    { VIEW_EXPLORER, create_explorer },
    { VIEW_SEARCH, create_search },
    { VIEW_SOURCE_CTRL, create_source_control },
    { VIEW_RUN_DEBUG, create_run_debug },
    { VIEW_TESTING, create_testing },
    { VIEW_DESIGNER, create_designer },
    { VIEW_APPLICATIONS, create_applications },
    { VIEW_FRAMEWORK, create_framework },
    { VIEW_AI, create_ai }
};

UmiStatus umi_studio_workbench_views_register(
    UmiUiWorkbench *workbench,
    UmiStudioServices *services)
{
    UmiUiViewFactoryRegistry *registry;
    size_t index;

    if (workbench == NULL || services == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    registry = umi_ui_workbench_view_factories(workbench);
    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]);
         ++index) {
        UmiUiViewFactoryDescriptor descriptor = {0};
        UmiStatus status;

        (void)snprintf(descriptor.view_type, sizeof(descriptor.view_type),
                       "%s", DEFINITIONS[index].view_type);
        (void)snprintf(descriptor.provider_id, sizeof(descriptor.provider_id),
                       "%s", "org.umicom.studio.workbench");
        descriptor.create = DEFINITIONS[index].create;
        descriptor.user_data = services;

        status = umi_ui_view_factory_register(registry, &descriptor);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

size_t umi_studio_workbench_view_definition_count(void)
{
    return sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]);
}
