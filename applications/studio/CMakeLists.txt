#-----------------------------------------------------------------------------
# Umicom Studio IDE application targets
# File: applications/studio/CMakeLists.txt
#
# PURPOSE:
#   Create explicit Framework-based Studio targets.  The current Studio source
#   is treated as the active product implementation, not as a disposable legacy
#   library.  Reusable capabilities will continue to migrate into Framework in
#   small, independently tested vertical slices.
#
# AUTHOR AND ORGANISATION:
#   Sammy Hegab
#   Umicom Foundation
#
# LICENCE:
#   MIT
#-----------------------------------------------------------------------------
set(UMICOM_STUDIO_VERSION "0.23.0")

# -----------------------------------------------------------------------------
# Studio composition root
# -----------------------------------------------------------------------------
add_library(umicom_studio_core STATIC
    src/app/bootstrap.c
    src/app/federated_content.c
    src/app/federated_interactions.c
    src/app/federated_workspace_session.c
    src/app/services.c
    src/app/trading.c
    src/app/version.c
    src/app/doctor.c
    src/app/diagnostics.c
    src/app/settings.c
    src/app/platform.c
    src/app/toolchain_centre.c
    src/app/session.c
    src/app/documents.c
    src/app/tasks.c
    src/app/commands.c
    src/app/state.c
    src/app/workspace.c
    src/app/files.c
    src/app/search.c
    src/app/processes.c
    src/app/watcher.c
    src/app/data.c
    src/app/database_explorer_centre.c
    src/app/database_explorer_connections.c
    src/app/database_explorer_queries.c
    src/app/database_explorer_migrations.c
    src/app/database_explorer_transfer.c
    src/app/database_explorer_views.c
    src/app/database_explorer_commands.c
    src/app/integration_designer_centre.c
    src/app/integration_designer_connections.c
    src/app/integration_designer_api.c
    src/app/integration_designer_mapping.c
    src/app/integration_designer_workflow.c
    src/app/integration_designer_execution.c
    src/app/integration_designer_monitoring.c
    src/app/integration_designer_views.c
    src/app/integration_designer_commands.c
    src/app/messages.c
    src/app/history.c
    src/app/replay.c
    src/app/fabric.c
    src/app/operations.c
    src/app/operations_centre.c
    src/app/operations_views.c
    src/app/operations_commands.c
    src/app/security.c
    src/app/security_centre.c
    src/app/security_identities.c
    src/app/security_roles.c
    src/app/security_secrets.c
    src/app/security_approvals.c
    src/app/security_packages.c
    src/app/security_audit_view.c
    src/app/security_views.c
    src/app/security_commands.c
    src/app/plugins.c
    src/app/observability.c
    src/app/resilience.c
    src/app/ui.c
    src/app/workbench.c
    src/app/professional_workspace.c
    src/app/workspace_catalogue.c
    src/app/workspace_layouts.c
    src/app/workspace_profiles.c
    src/app/workspace_profile_session.c
    src/app/editor_layout_session.c
    src/app/editor_command_contribution.c
    src/app/source_navigation_contribution.c
    src/app/navigation_view_contribution.c
    src/app/editor_intelligence_contribution.c
    src/app/editor_assistance_contribution.c
    src/app/editor_productivity_contribution.c
    src/app/editor_refactoring_contribution.c
    src/app/debug_orchestration_contribution.c
    src/app/workspace_groups.c
    src/app/workspace_themes.c
    src/app/appearance_centre.c
    src/app/workspace_persistence.c
    src/app/workspace_views.c
    src/app/workspace_commands.c
    src/app/compiler_settings.c
    src/app/compiler_catalogue.c
    src/app/compiler_project_binding.c
    src/app/compiler_templates.c
    src/app/compiler_views.c
    src/app/compiler_commands.c
    src/app/compiler_diagnostics.c
    src/app/compiler_interop.c
    src/app/umicc_integration.c
    src/app/polyglot_workspace.c
    src/app/compiler_export.c
    src/app/compiler_centre.c
    src/app/sdk_centre.c
    src/app/package_project.c
    src/app/dependency_centre.c
    src/app/bundle_designer.c
    src/app/installer_centre.c
    src/app/supply_chain.c
    src/app/release_channels.c
    src/app/upgrade_centre.c
    src/app/distribution_centre.c
    src/app/distribution_views.c
    src/app/distribution_commands.c
    src/app/perspectives.c
    src/app/contributions.c
    src/app/view_models.c
    src/app/workbench_commands.c
    src/app/platform_contract.c
    src/app/workbench_shell_catalogue.c
    src/app/workbench_views.c
    src/app/platform_shell.c
    src/app/command_centre.c
    src/app/resource_explorer.c
    src/app/global_search.c
    src/app/extension_centre.c
    src/app/extension_platform.c
    src/app/product_centre.c
    src/app/application_centre.c
    src/app/settings_centre.c
    src/app/welcome_centre.c
    src/app/developer_dashboard.c
    src/app/editor_workspace.c
    src/app/chart_workspace.c
    src/app/designer_workspace.c
    src/app/ai_workspace.c
    src/app/project_centre.c
    src/app/language_intelligence_centre.c
    src/app/debug_centre.c
    src/app/source_control_centre.c
    src/app/test_explorer_centre.c
    src/app/problems_centre.c
    src/app/output_centre.c
    src/app/progress_centre.c
    src/app/developer_workbench.c
    src/app/developer_session.c
    src/app/developer_pipeline_centre.c
    src/app/developer_task_centre.c
    src/app/developer_run_centre.c
    src/app/developer_workspace_state_centre.c
    src/app/ui_catalogue.c
    src/app/build.c
    src/app/build_workbench.c
    src/app/tests.c
    src/app/terminal.c
    src/app/language.c
    src/app/debugger.c
    src/app/debug_workspace_views.c
    src/app/source_control.c
    src/app/developer_platform.c
    src/app/declarative.c
    src/app/designer.c
    src/app/designer_catalogue.c
    src/app/designer_preview.c
    src/app/designer_project.c
    src/app/new_project.c
    src/app/designer_state.c
    src/app/web_platform.c
    src/app/web.c
    src/app/browser.c
    src/app/charting.c
    src/app/frontend.c
    src/app/frontend_studio_settings.c
    src/app/frontend_studio_preview.c
    src/app/frontend_studio_inspector.c
    src/app/frontend_studio_centre.c
    src/app/frontend_studio_views.c
    src/app/frontend_studio_commands.c
    src/app/ai_chat.c
    src/app/ai_evaluation.c
    src/app/ai_platform.c
    src/app/coding_assistant.c
    src/app/knowledge_centre.c
    src/app/knowledge_settings.c
    src/app/knowledge_views.c
    src/app/knowledge_commands.c
    src/app/ai_retrieval.c
    src/app/ai_sources.c
    src/app/ai_tools.c
    src/app/authorengine.c
    src/app/helix.c
    src/app/helix_agents.c
    src/app/helix_candidate.c
    src/app/helix_evidence.c
    src/app/helix_fitness.c
    src/app/helix_generation.c
    src/app/helix_agent_adapters.c
    src/app/helix_agent_centre.c
    src/app/helix_agent_commands.c
    src/app/helix_agent_operations.c
    src/app/helix_agent_views.c
    src/app/helix_agent_candidates.c
    src/app/helix_agent_journal.c
    src/app/helix_agent_settings.c
    src/app/delivery_platform.c
    src/app/compatibility_platform.c
    src/app/sdk.c
    src/app/products.c
    src/app/conformance.c
    src/app/architecture_guard.c
    src/app/architecture_quality.c
    src/app/quality_gate.c
    src/app/duplicate_review.c
    src/app/consolidation.c
    src/app/codeguard.c
    src/app/quality_centre.c
    src/app/quality_evidence.c
    src/app/quality_navigation.c
    src/app/quality_remediation.c
    src/app/quality_duplicates.c
    src/app/quality_compatibility.c
    src/app/quality_gates.c
    src/app/quality_views.c
    src/app/quality_commands.c
    src/app/delivery_release.c
    src/app/delivery_package.c
    src/app/delivery_update.c
    src/app/delivery_report.c
    src/app/delivery_distribution.c
    src/app/delivery_evidence.c
    src/app/visual_builder_centre.c
    src/app/visual_builder_tree.c
    src/app/visual_builder_properties.c
    src/app/visual_builder_bindings.c
    src/app/visual_builder_interactions.c
    src/app/visual_builder_drag_drop.c
    src/app/visual_builder_clipboard.c
    src/app/visual_builder_templates.c
    src/app/visual_builder_layout.c
    src/app/visual_builder_preview.c
    src/app/visual_builder_source.c
    src/app/visual_builder_views.c
    src/app/visual_builder_commands.c
    src/app/visual_builder_settings.c
)
add_library(Umicom::StudioCore ALIAS umicom_studio_core)

target_include_directories(umicom_studio_core PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

target_link_libraries(umicom_studio_core PUBLIC
    Umicom::Framework
    Umicom::ui
    Umicom::ui_headless
)

umicom_apply_warnings(umicom_studio_core)
umicom_apply_sanitizers(umicom_studio_core)

# A small console frontend verifies Framework lifecycle without GTK4.
add_executable(umicom-studio-console
    src/console/main.c
)
target_link_libraries(umicom-studio-console PRIVATE
    Umicom::StudioCore
)
umicom_apply_warnings(umicom-studio-console)
umicom_apply_sanitizers(umicom-studio-console)

# Native project-health command.  This is intentionally implemented in C23 so
# normal repository validation does not depend on Python.
add_executable(umicom-studio-doctor
    src/tools/doctor_main.c
)
target_link_libraries(umicom-studio-doctor PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-doctor)
umicom_apply_sanitizers(umicom-studio-doctor)

# Native retained-diagnostics inspection command.
add_executable(umicom-studio-diagnostics
    src/tools/diagnostics_main.c
)
target_link_libraries(umicom-studio-diagnostics PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-diagnostics)
umicom_apply_sanitizers(umicom-studio-diagnostics)


# Native typed-settings inspection and editing command.
add_executable(umicom-studio-settings
    src/tools/settings_main.c
)
target_link_libraries(umicom-studio-settings PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-settings)
umicom_apply_sanitizers(umicom-studio-settings)

# Native Framework platform and toolchain inspection command.
add_executable(umicom-studio-platform
    src/tools/platform_main.c
)
target_link_libraries(umicom-studio-platform PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-platform)
umicom_apply_sanitizers(umicom-studio-platform)

# Native multi-profile Toolchain & Environment Centre inspection command.
# The reusable discovery/catalogue/environment mechanisms remain Framework
# owned; this target is Studio's composition and operator-facing boundary.
add_executable(umicom-studio-toolchain
    src/tools/toolchain_main.c
)
target_link_libraries(umicom-studio-toolchain PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-toolchain)
umicom_apply_sanitizers(umicom-studio-toolchain)

# Native Framework and Studio operational-state command.
add_executable(umicom-studio-state
    src/tools/state_main.c
)
target_link_libraries(umicom-studio-state PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-state)
umicom_apply_sanitizers(umicom-studio-state)

# Native workspace, index and search inspection command.
add_executable(umicom-studio-workspace
    src/tools/workspace_main.c
)
target_link_libraries(umicom-studio-workspace PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-workspace)
umicom_apply_sanitizers(umicom-studio-workspace)

# Native supervised process execution command.
add_executable(umicom-studio-process
    src/tools/process_main.c
)
target_link_libraries(umicom-studio-process PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-process)
umicom_apply_sanitizers(umicom-studio-process)

# Native Data Server inspection command.
add_executable(umicom-studio-data src/tools/data_main.c)
target_link_libraries(umicom-studio-data PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-data)
umicom_apply_sanitizers(umicom-studio-data)

# Database Explorer stays a Studio composition over the authoritative
# Framework Data Server; it never exposes provider connection handles.
add_executable(umicom-studio-database-explorer
    src/tools/database_explorer_main.c)
target_link_libraries(umicom-studio-database-explorer
    PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-database-explorer)
umicom_apply_sanitizers(umicom-studio-database-explorer)

# Integration Designer composes provider-neutral Framework contracts. The
# executable performs only deterministic planning and simulation by default.
add_executable(umicom-studio-integration-designer
    src/tools/integration_designer_main.c)
target_link_libraries(umicom-studio-integration-designer
    PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-integration-designer)
umicom_apply_sanitizers(umicom-studio-integration-designer)

# Native Integration Fabric inspection and publication command.
add_executable(umicom-studio-messages src/tools/messages_main.c)
target_link_libraries(umicom-studio-messages PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-messages)
umicom_apply_sanitizers(umicom-studio-messages)

# Native durable replay command.
add_executable(umicom-studio-replay src/tools/replay_main.c)
target_link_libraries(umicom-studio-replay PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-replay)
umicom_apply_sanitizers(umicom-studio-replay)

# Native Studio operational-history command.
add_executable(umicom-studio-history src/tools/history_main.c)
target_link_libraries(umicom-studio-history PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-history)
umicom_apply_sanitizers(umicom-studio-history)

# Native security, plug-in, observability and resilience commands.
foreach(UMICOM_STUDIO_OPERATION_TOOL
        security
        plugins
        observability
        resilience
        operations)
    add_executable(umicom-studio-${UMICOM_STUDIO_OPERATION_TOOL}
        src/tools/${UMICOM_STUDIO_OPERATION_TOOL}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_OPERATION_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_OPERATION_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_OPERATION_TOOL})
endforeach()

# Native Security, Identity, Secrets and Policy Centre command.
add_executable(umicom-studio-security-centre src/tools/security_centre_main.c)
target_link_libraries(umicom-studio-security-centre PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-security-centre)
umicom_apply_sanitizers(umicom-studio-security-centre)

# Native Operations Centre and Professional Workspace Designer commands.
add_executable(umicom-studio-operations-centre src/tools/operations_centre_main.c)
target_link_libraries(umicom-studio-operations-centre PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-operations-centre)
umicom_apply_sanitizers(umicom-studio-operations-centre)

add_executable(umicom-studio-workspace-designer src/tools/workspace_designer_main.c)
target_link_libraries(umicom-studio-workspace-designer PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-workspace-designer)
umicom_apply_sanitizers(umicom-studio-workspace-designer)

# Batch 58 native Compiler Centre and polyglot planning commands.
add_executable(umicom-studio-compiler-centre src/tools/compiler_centre_main.c)
target_link_libraries(umicom-studio-compiler-centre PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-compiler-centre)
umicom_apply_sanitizers(umicom-studio-compiler-centre)

add_executable(umicom-studio-polyglot-compiler src/tools/polyglot_compiler_main.c)
target_link_libraries(umicom-studio-polyglot-compiler PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-polyglot-compiler)
umicom_apply_sanitizers(umicom-studio-polyglot-compiler)

# Batch 59 native SDK, package, installer and distribution commands.
add_executable(umicom-studio-distribution-centre
    src/tools/distribution_centre_main.c)
target_link_libraries(umicom-studio-distribution-centre
    PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-distribution-centre)
umicom_apply_sanitizers(umicom-studio-distribution-centre)

add_executable(umicom-studio-package-inspector
    src/tools/package_inspector_main.c)
target_link_libraries(umicom-studio-package-inspector
    PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-package-inspector)
umicom_apply_sanitizers(umicom-studio-package-inspector)

# Native toolkit-neutral workbench and UI catalogue commands.
add_executable(umicom-studio-workbench src/tools/workbench_main.c)
target_link_libraries(umicom-studio-workbench PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-workbench)
umicom_apply_sanitizers(umicom-studio-workbench)

add_executable(umicom-studio-ui-catalogue src/tools/ui_catalogue_main.c)
target_link_libraries(umicom-studio-ui-catalogue PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-ui-catalogue)
umicom_apply_sanitizers(umicom-studio-ui-catalogue)

# Native developer-platform commands for build, test, terminal, language,
# debugging, source control and the combined platform report.
foreach(UMICOM_STUDIO_DEVELOPER_TOOL
        build
        tests
        terminal
        language
        debugger
        source-control
        developer
        developer-pipeline
        developer-state)
    string(REPLACE "-" "_" UMICOM_STUDIO_DEVELOPER_SOURCE
        "${UMICOM_STUDIO_DEVELOPER_TOOL}")
    add_executable(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TOOL}
        src/tools/${UMICOM_STUDIO_DEVELOPER_SOURCE}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TOOL})
endforeach()

# Native declarative-template and visual-designer commands.
foreach(UMICOM_STUDIO_DESIGNER_TOOL
        template
        designer
        designer-catalogue
        visual-builder
        new-project)
    string(REPLACE "-" "_" UMICOM_STUDIO_DESIGNER_SOURCE
        "${UMICOM_STUDIO_DESIGNER_TOOL}")
    add_executable(umicom-studio-${UMICOM_STUDIO_DESIGNER_TOOL}
        src/tools/${UMICOM_STUDIO_DESIGNER_SOURCE}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_DESIGNER_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DESIGNER_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DESIGNER_TOOL})
endforeach()


# Native Web Server and multi-frontend platform commands.
foreach(UMICOM_STUDIO_WEB_TOOL
        web
        browser
        chart
        frontend
        frontend-studio
        web-platform)
    string(REPLACE "-" "_" UMICOM_STUDIO_WEB_SOURCE
        "${UMICOM_STUDIO_WEB_TOOL}")
    add_executable(umicom-studio-${UMICOM_STUDIO_WEB_TOOL}
        src/tools/${UMICOM_STUDIO_WEB_SOURCE}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_WEB_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_WEB_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_WEB_TOOL})
endforeach()


# Native AI, AuthorEngine and Helix commands.
foreach(UMICOM_STUDIO_AI_TOOL
        ai
        retrieval
        authorengine
        knowledge
        helix
        helix-evidence
        helix-generation
        helix-agent-runtime)
    string(REPLACE "-" "_" UMICOM_STUDIO_AI_SOURCE
        "${UMICOM_STUDIO_AI_TOOL}")
    add_executable(umicom-studio-${UMICOM_STUDIO_AI_TOOL}
        src/tools/${UMICOM_STUDIO_AI_SOURCE}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_AI_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_AI_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_AI_TOOL})
endforeach()

# -----------------------------------------------------------------------------
# GTK4 Studio product implementation
# -----------------------------------------------------------------------------

# Native application-delivery, packaging, update and release-engineering commands.
foreach(UMICOM_STUDIO_DELIVERY_TOOL
        delivery
        package
        release
        update
        distribution
        evidence
        rollback
        pipeline)
    add_executable(umicom-studio-${UMICOM_STUDIO_DELIVERY_TOOL}
        src/tools/${UMICOM_STUDIO_DELIVERY_TOOL}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_DELIVERY_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DELIVERY_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DELIVERY_TOOL})
endforeach()


# Native CodeGuard and consolidation commands.
add_executable(umicom-studio-codeguard src/tools/codeguard_main.c)
target_link_libraries(umicom-studio-codeguard PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-codeguard)
umicom_apply_sanitizers(umicom-studio-codeguard)
add_executable(umicom-studio-consolidation src/tools/consolidation_main.c)
target_link_libraries(umicom-studio-consolidation PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-consolidation)
umicom_apply_sanitizers(umicom-studio-consolidation)
add_executable(umicom-studio-duplicates src/tools/duplicates_main.c)
target_link_libraries(umicom-studio-duplicates PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-duplicates)
umicom_apply_sanitizers(umicom-studio-duplicates)
add_executable(umicom-studio-quality-centre src/tools/quality_centre_main.c)
target_link_libraries(umicom-studio-quality-centre PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-quality-centre)
umicom_apply_sanitizers(umicom-studio-quality-centre)
add_executable(umicom-studio-quality src/tools/quality_main.c)
target_link_libraries(umicom-studio-quality PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-quality)
umicom_apply_sanitizers(umicom-studio-quality)

if(UMICOM_STUDIO_BUILD_GTK)
    find_package(PkgConfig REQUIRED)

    pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)
    pkg_check_modules(GOBJECT REQUIRED IMPORTED_TARGET gobject-2.0)
    pkg_check_modules(GIO REQUIRED IMPORTED_TARGET gio-2.0)
    pkg_check_modules(JSON_GLIB REQUIRED IMPORTED_TARGET json-glib-1.0)
    pkg_check_modules(SOUP3 REQUIRED IMPORTED_TARGET libsoup-3.0)
    pkg_check_modules(CURL REQUIRED IMPORTED_TARGET libcurl)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4>=4.10)
    pkg_check_modules(PIXBUF REQUIRED IMPORTED_TARGET gdk-pixbuf-2.0)
    pkg_check_modules(CAIRO REQUIRED IMPORTED_TARGET cairo)
    pkg_check_modules(PANGO REQUIRED IMPORTED_TARGET pango)
    pkg_check_modules(HARFBUZZ REQUIRED IMPORTED_TARGET harfbuzz)
    pkg_check_modules(GRAPHENE REQUIRED IMPORTED_TARGET graphene-1.0)
    pkg_check_modules(SOURCEVIEW5 QUIET IMPORTED_TARGET gtksourceview-5)

    include("${CMAKE_CURRENT_SOURCE_DIR}/cmake/StudioSources.cmake")

    add_library(umicom_studio_product STATIC
        ${UMICOM_STUDIO_PRODUCT_SOURCES}
    )
    add_library(Umicom::StudioProduct ALIAS umicom_studio_product)

    # These paths preserve the current product's include relationships.  Later
    # extraction commits will reduce this list as each bounded module receives
    # a public contract and its own CMake target.
    target_include_directories(umicom_studio_product PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/build/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/core/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/editor/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/chrome/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/dialogs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/llm_lab/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/menu/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/prefs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/theme"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/includes"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/llm/providers"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/chat/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/output/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/panes/problems/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/transpile"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/plugins/transpile/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/search"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/search/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/resources"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/fs/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/git/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/log/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/sys/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/util/watchers/include"
    )

    target_compile_definitions(umicom_studio_product PRIVATE
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=17
        UMICOM_VERSION_PATCH=0
        gtk_widget_destroy=g_object_unref
    )

    target_link_libraries(umicom_studio_product
        PUBLIC
            Umicom::Framework
        PRIVATE
            PkgConfig::GLIB
            PkgConfig::GOBJECT
            PkgConfig::GIO
            PkgConfig::JSON_GLIB
            PkgConfig::SOUP3
            PkgConfig::CURL
            PkgConfig::GTK4
            PkgConfig::PIXBUF
            PkgConfig::CAIRO
            PkgConfig::PANGO
            PkgConfig::HARFBUZZ
            PkgConfig::GRAPHENE
    )

    if(TARGET PkgConfig::SOURCEVIEW5)
        target_link_libraries(umicom_studio_product PRIVATE
            PkgConfig::SOURCEVIEW5
        )
    endif()

    # The product implementation is not warning-clean yet.  Keep a practical
    # warning set here while new Framework and StudioCore code remains strict.
    if(MSVC)
        target_compile_options(umicom_studio_product PRIVATE /W4)
    else()
        target_compile_options(umicom_studio_product PRIVATE
            -Wall
            -Wextra
            -Wpedantic
            -Wno-unused-parameter
        )
    endif()

    add_executable(umicom-studio-workbench-demo
        src/gtk/workbench_main.c
        src/gui/workbench/workbench_window.c
        src/gui/workbench/perspective_switcher.c
        src/gui/workbench/status_strip.c
        src/gui/explorer/project_explorer_view.c
        src/gui/editor/editor_host_view.c
    )
    target_include_directories(umicom-studio-workbench-demo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/workbench/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/explorer/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/editor/include"
    )
    target_link_libraries(umicom-studio-workbench-demo PRIVATE
        Umicom::StudioCore
        Umicom::ui_gtk4
        PkgConfig::GTK4
        PkgConfig::GLIB
        PkgConfig::GIO
    )
    umicom_apply_warnings(umicom-studio-workbench-demo)
    umicom_apply_sanitizers(umicom-studio-workbench-demo)

    add_executable(umicom-studio-designer-demo
        src/gtk/designer_main.c
        src/gui/designer/designer_window.c
        src/gui/designer/palette_view.c
        src/gui/designer/tree_view.c
        src/gui/designer/inspector_view.c
        src/gui/designer/surface_view.c
        src/gui/designer/preview_view.c
    )
    target_include_directories(umicom-studio-designer-demo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/designer/include"
    )
    target_link_libraries(umicom-studio-designer-demo PRIVATE
        Umicom::StudioCore
        Umicom::ui_gtk4
        PkgConfig::GTK4
        PkgConfig::GLIB
    )
    umicom_apply_warnings(umicom-studio-designer-demo)
    umicom_apply_sanitizers(umicom-studio-designer-demo)


    add_executable(umicom-studio-web-platform-demo
        src/gui/web/web_platform_demo.c
        src/gui/web/web_preview.c
        src/gui/web/chart_preview.c
        src/gui/web/frontend_preview.c
    )
    target_include_directories(umicom-studio-web-platform-demo PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/web/include"
    )
    target_link_libraries(umicom-studio-web-platform-demo PRIVATE
        Umicom::StudioCore
        PkgConfig::GTK4
        PkgConfig::GLIB
        PkgConfig::GIO
    )
    umicom_apply_warnings(umicom-studio-web-platform-demo)
    umicom_apply_sanitizers(umicom-studio-web-platform-demo)

    add_executable(umicom-studio-ide
        src/gtk/main.c
        src/gui/workbench/workbench_window.c
    )

    set(UMICOM_STUDIO_WINDOWS_RESOURCE
        "${CMAKE_SOURCE_DIR}/win/umicom.rc"
    )

    if(WIN32 AND EXISTS "${UMICOM_STUDIO_WINDOWS_RESOURCE}")
        target_sources(umicom-studio-ide PRIVATE
            "${UMICOM_STUDIO_WINDOWS_RESOURCE}"
        )
    endif()

    target_include_directories(umicom-studio-ide PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/app/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/gui/workbench/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/ui/include"
        "${CMAKE_SOURCE_DIR}/win"
    )

    target_link_libraries(umicom-studio-ide PRIVATE
        Umicom::StudioCore
        Umicom::StudioProduct
        Umicom::ui_gtk4
        PkgConfig::GTK4
        PkgConfig::GLIB
        PkgConfig::GIO
    )

    target_compile_definitions(umicom-studio-ide PRIVATE
        USIDE_GUI=$<IF:$<BOOL:${UMICOM_STUDIO_GUI}>,1,0>
        NOMINMAX
        _CRT_SECURE_NO_WARNINGS
        UMICOM_VERSION_MAJOR=0
        UMICOM_VERSION_MINOR=17
        UMICOM_VERSION_PATCH=0
    )

    if(WIN32 AND UMICOM_STUDIO_GUI)
        set_property(TARGET umicom-studio-ide PROPERTY WIN32_EXECUTABLE TRUE)
    endif()

    # Keep the canonical trademark assets beside the executable in development
    # and in installed packages. Runtime path discovery avoids leaking a
    # developer's private source-tree path into the binary.
    add_custom_command(TARGET umicom-studio-ide POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory
            "$<TARGET_FILE_DIR:umicom-studio-ide>/branding"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/assets/umicom-icon.svg"
            "$<TARGET_FILE_DIR:umicom-studio-ide>/branding/umicom-icon.svg"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/assets/umicom-logo.svg"
            "$<TARGET_FILE_DIR:umicom-studio-ide>/branding/umicom-logo.svg"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_SOURCE_DIR}/win/umicom-logo.png"
            "$<TARGET_FILE_DIR:umicom-studio-ide>/branding/umicom-logo.png"
        COMMENT "Copying Umicom trademark assets for the Studio workbench")

    install(TARGETS
        umicom-studio-ide
        umicom-studio-workbench-demo
        RUNTIME DESTINATION bin)
    install(FILES
        "${CMAKE_SOURCE_DIR}/assets/umicom-icon.svg"
        "${CMAKE_SOURCE_DIR}/assets/umicom-logo.svg"
        "${CMAKE_SOURCE_DIR}/win/umicom-logo.png"
        "${CMAKE_SOURCE_DIR}/win/umicom.ico"
        DESTINATION bin/branding)
endif()

# -----------------------------------------------------------------------------
# Framework integration tests
# -----------------------------------------------------------------------------

# Native ABI, SDK, reference-product and architecture-conformance commands.
foreach(UMICOM_STUDIO_CONFORMANCE_TOOL
        abi
        sdk
        products
        compatibility
        conformance
        architecture)
    add_executable(umicom-studio-${UMICOM_STUDIO_CONFORMANCE_TOOL}
        src/tools/${UMICOM_STUDIO_CONFORMANCE_TOOL}_main.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_CONFORMANCE_TOOL}
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_CONFORMANCE_TOOL})
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_CONFORMANCE_TOOL})
endforeach()

# -----------------------------------------------------------------------------
# Compatibility fallback for simple native tool sources
# -----------------------------------------------------------------------------
# Batch 26 introduced this generic tool-discovery path.  It is intentionally
# retained rather than deleted.  The explicit target definitions above remain
# canonical because they preserve the existing grouped architecture and
# documentation.  The NOT TARGET guard makes this block additive: it only
# creates a command if a future source exists and no canonical target has
# already claimed that executable name.
set(UMICOM_STUDIO_INSTALL_TARGETS umicom-studio-console)
set(UMICOM_STUDIO_SIMPLE_TOOLS
    doctor diagnostics settings platform state workspace process data messages replay history
    security plugins observability resilience operations workbench ui_catalogue
    build tests terminal language debugger source_control developer template designer designer_catalogue new_project
    web browser chart frontend web_platform ai retrieval authorengine knowledge helix helix_evidence helix_generation
    delivery package release update distribution evidence rollback pipeline abi sdk products compatibility conformance architecture
    codeguard consolidation duplicates quality)
foreach(tool IN LISTS UMICOM_STUDIO_SIMPLE_TOOLS)
    string(REPLACE "_" "-" exe "${tool}")
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/src/tools/${tool}_main.c"
       AND NOT TARGET umicom-studio-${exe})
        add_executable(umicom-studio-${exe} src/tools/${tool}_main.c)
        target_link_libraries(umicom-studio-${exe} PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${exe})
        umicom_apply_sanitizers(umicom-studio-${exe})
        list(APPEND UMICOM_STUDIO_INSTALL_TARGETS umicom-studio-${exe})
    endif()
endforeach()

if(BUILD_TESTING)
    add_executable(umicom-studio-framework-bootstrap-test
        tests/test_framework_bootstrap.c
    )
    target_link_libraries(umicom-studio-framework-bootstrap-test PRIVATE
        Umicom::StudioCore
    )
    umicom_apply_warnings(umicom-studio-framework-bootstrap-test)
    umicom_apply_sanitizers(umicom-studio-framework-bootstrap-test)

    add_test(
        NAME studio.framework_bootstrap
        COMMAND umicom-studio-framework-bootstrap-test
    )

    # Unified workbench and Batch 25 platform shell coverage.
    foreach(UMICOM_STUDIO_PLATFORM_TEST
            workbench_shell_catalogue
            workbench_activity
            workbench_keybinding
            workbench_session_state
            workbench_views
            source_control_workbench
            platform_shell_expansion
            editor_workspace
            editor_workbench_commands
            developer_project_centre
            developer_workspace_model
            developer_workspace_discovery
            developer_workspace_inheritance
            developer_language_intelligence_centre
            developer_debug_centre
            developer_source_control_centre
            developer_test_explorer_centre
            developer_developer_workbench
            developer_developer_session
            developer_pipeline_centre
            developer_runtime_binding
            developer_runtime_execution
            developer_task_centre
            developer_run_centre
            developer_workspace_state_centre
            developer_workflow_centre
            developer_workbench_automation)
        add_executable(umicom-studio-${UMICOM_STUDIO_PLATFORM_TEST}-test
            tests/test_${UMICOM_STUDIO_PLATFORM_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_PLATFORM_TEST}-test PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_PLATFORM_TEST}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_PLATFORM_TEST}-test)
        add_test(NAME studio.platform.${UMICOM_STUDIO_PLATFORM_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_PLATFORM_TEST}-test)
    endforeach()

    foreach(UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST
            integration_designer_centre
            integration_designer_connections_api
            integration_designer_mapping
            integration_designer_workflow
            integration_designer_execution
            integration_designer_monitoring
            integration_designer_views_commands)
        string(REPLACE "_" "-" UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET
            "${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_INTEGRATION_DESIGNER_TEST_TARGET}-test)
    endforeach()

    # Filesystem project-import coverage extends the existing platform tests.
    add_executable(umicom-studio-developer-project-import-test
        tests/test_developer_project_import.c)
    target_link_libraries(umicom-studio-developer-project-import-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-developer-project-import-test)
    umicom_apply_sanitizers(umicom-studio-developer-project-import-test)
    add_test(NAME studio.platform.developer_project_import
        COMMAND umicom-studio-developer-project-import-test)

    # Batch 35 Studio composition coverage: selection/environment snapshots,
    # deterministic per-project binding and compile_commands.json import.
    foreach(UMICOM_STUDIO_TOOLCHAIN_TEST
            centre
            project_binding
            compilation_database)
        add_executable(umicom-studio-toolchain-${UMICOM_STUDIO_TOOLCHAIN_TEST}-test
            tests/test_toolchain_${UMICOM_STUDIO_TOOLCHAIN_TEST}.c)
        target_link_libraries(
            umicom-studio-toolchain-${UMICOM_STUDIO_TOOLCHAIN_TEST}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(
            umicom-studio-toolchain-${UMICOM_STUDIO_TOOLCHAIN_TEST}-test)
        umicom_apply_sanitizers(
            umicom-studio-toolchain-${UMICOM_STUDIO_TOOLCHAIN_TEST}-test)
        add_test(NAME studio.toolchain.${UMICOM_STUDIO_TOOLCHAIN_TEST}
            COMMAND umicom-studio-toolchain-${UMICOM_STUDIO_TOOLCHAIN_TEST}-test)
    endforeach()

    add_executable(umicom-studio-version-test
        tests/test_version.c
    )
    target_link_libraries(umicom-studio-version-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-version-test)
    add_test(NAME studio.version COMMAND umicom-studio-version-test)

    add_executable(umicom-studio-services-test
        tests/test_services.c
    )
    target_link_libraries(umicom-studio-services-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-services-test)
    add_test(NAME studio.services COMMAND umicom-studio-services-test)

    add_executable(umicom-studio-doctor-test
        tests/test_doctor.c
    )
    target_link_libraries(umicom-studio-doctor-test PRIVATE Umicom::StudioCore)
    target_compile_definitions(umicom-studio-doctor-test PRIVATE
        UMICOM_STUDIO_SOURCE_ROOT="${CMAKE_SOURCE_DIR}"
    )
    umicom_apply_warnings(umicom-studio-doctor-test)
    add_test(NAME studio.doctor COMMAND umicom-studio-doctor-test)

    add_executable(umicom-studio-diagnostics-test
        tests/test_diagnostics.c
    )
    target_link_libraries(umicom-studio-diagnostics-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-diagnostics-test)
    umicom_apply_sanitizers(umicom-studio-diagnostics-test)
    add_test(NAME studio.diagnostics COMMAND umicom-studio-diagnostics-test)


    add_executable(umicom-studio-platform-test
        tests/test_platform.c
    )
    target_link_libraries(umicom-studio-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-platform-test)
    umicom_apply_sanitizers(umicom-studio-platform-test)
    add_test(NAME studio.platform COMMAND umicom-studio-platform-test)

    add_executable(umicom-studio-settings-test
        tests/test_settings.c
    )
    target_link_libraries(umicom-studio-settings-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-settings-test)
    umicom_apply_sanitizers(umicom-studio-settings-test)
    add_test(NAME studio.settings COMMAND umicom-studio-settings-test)

    add_executable(umicom-studio-documents-test
        tests/test_documents.c
    )
    target_link_libraries(umicom-studio-documents-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-documents-test)
    umicom_apply_sanitizers(umicom-studio-documents-test)
    add_test(NAME studio.documents COMMAND umicom-studio-documents-test)

    add_executable(umicom-studio-document-workbench-test
        tests/test_document_workbench.c
    )
    target_link_libraries(umicom-studio-document-workbench-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-document-workbench-test)
    umicom_apply_sanitizers(umicom-studio-document-workbench-test)
    add_test(NAME studio.document_workbench COMMAND umicom-studio-document-workbench-test)

    add_executable(umicom-studio-tasks-test
        tests/test_tasks.c
    )
    target_link_libraries(umicom-studio-tasks-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-tasks-test)
    umicom_apply_sanitizers(umicom-studio-tasks-test)
    add_test(NAME studio.tasks COMMAND umicom-studio-tasks-test)

    add_executable(umicom-studio-commands-test
        tests/test_commands.c
    )
    target_link_libraries(umicom-studio-commands-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-commands-test)
    umicom_apply_sanitizers(umicom-studio-commands-test)
    add_test(NAME studio.commands COMMAND umicom-studio-commands-test)

    add_executable(umicom-studio-state-test
        tests/test_state.c
    )
    target_link_libraries(umicom-studio-state-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-state-test)
    umicom_apply_sanitizers(umicom-studio-state-test)
    add_test(NAME studio.state COMMAND umicom-studio-state-test)

    # Runtime composition contract and regression shield.  These tests validate
    # identities and authoritative registry inventories while preserving the
    # extensibility needed by designer, plug-in and future Studio commands.
    add_executable(umicom-studio-platform-contract-test tests/test_platform_contract.c)
    target_link_libraries(umicom-studio-platform-contract-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-platform-contract-test)
    umicom_apply_sanitizers(umicom-studio-platform-contract-test)
    add_test(NAME studio.platform_contract COMMAND umicom-studio-platform-contract-test)

    add_executable(umicom-studio-command-inventory-test tests/test_command_inventory.c)
    target_link_libraries(umicom-studio-command-inventory-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-command-inventory-test)
    umicom_apply_sanitizers(umicom-studio-command-inventory-test)
    add_test(NAME studio.command_inventory COMMAND umicom-studio-command-inventory-test)

    add_executable(umicom-studio-command-inventory-partition-test tests/test_command_inventory_partition.c)
    target_link_libraries(umicom-studio-command-inventory-partition-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-command-inventory-partition-test)
    umicom_apply_sanitizers(umicom-studio-command-inventory-partition-test)
    add_test(NAME studio.command_inventory_partition COMMAND umicom-studio-command-inventory-partition-test)

    add_executable(umicom-studio-service-inventory-test tests/test_service_inventory.c)
    target_link_libraries(umicom-studio-service-inventory-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-service-inventory-test)
    umicom_apply_sanitizers(umicom-studio-service-inventory-test)
    add_test(NAME studio.service_inventory COMMAND umicom-studio-service-inventory-test)

    add_executable(umicom-studio-platform-contract-missing-test tests/test_platform_contract_missing.c)
    target_link_libraries(umicom-studio-platform-contract-missing-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-platform-contract-missing-test)
    umicom_apply_sanitizers(umicom-studio-platform-contract-missing-test)
    add_test(NAME studio.platform_contract_missing COMMAND umicom-studio-platform-contract-missing-test)

    add_executable(umicom-studio-version-contract-test tests/test_version_contract.c)
    target_link_libraries(umicom-studio-version-contract-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-version-contract-test)
    umicom_apply_sanitizers(umicom-studio-version-contract-test)
    add_test(NAME studio.version_contract COMMAND umicom-studio-version-contract-test)

    add_executable(umicom-studio-framework-runtime-contract-test tests/test_framework_runtime_contract.c)
    target_link_libraries(umicom-studio-framework-runtime-contract-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-framework-runtime-contract-test)
    umicom_apply_sanitizers(umicom-studio-framework-runtime-contract-test)
    add_test(NAME studio.framework_runtime_contract COMMAND umicom-studio-framework-runtime-contract-test)

    add_executable(umicom-studio-bootstrap-inventory-test tests/test_bootstrap_inventory.c)
    target_link_libraries(umicom-studio-bootstrap-inventory-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-bootstrap-inventory-test)
    umicom_apply_sanitizers(umicom-studio-bootstrap-inventory-test)
    add_test(NAME studio.bootstrap_inventory COMMAND umicom-studio-bootstrap-inventory-test)

    add_executable(umicom-studio-composition-lifecycle-test tests/test_composition_lifecycle.c)
    target_link_libraries(umicom-studio-composition-lifecycle-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-composition-lifecycle-test)
    umicom_apply_sanitizers(umicom-studio-composition-lifecycle-test)
    add_test(NAME studio.composition_lifecycle COMMAND umicom-studio-composition-lifecycle-test)

    add_executable(umicom-studio-registry-extension-compatibility-test tests/test_registry_extension_compatibility.c)
    target_link_libraries(umicom-studio-registry-extension-compatibility-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-registry-extension-compatibility-test)
    umicom_apply_sanitizers(umicom-studio-registry-extension-compatibility-test)
    add_test(NAME studio.registry_extension_compatibility COMMAND umicom-studio-registry-extension-compatibility-test)

    add_executable(umicom-studio-state-inventory-consistency-test tests/test_state_inventory_consistency.c)
    target_link_libraries(umicom-studio-state-inventory-consistency-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-state-inventory-consistency-test)
    umicom_apply_sanitizers(umicom-studio-state-inventory-consistency-test)
    add_test(NAME studio.state_inventory_consistency COMMAND umicom-studio-state-inventory-consistency-test)

    add_executable(umicom-studio-designer-command-contribution-test tests/test_designer_command_contribution.c)
    target_link_libraries(umicom-studio-designer-command-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-designer-command-contribution-test)
    umicom_apply_sanitizers(umicom-studio-designer-command-contribution-test)
    add_test(NAME studio.designer_command_contribution COMMAND umicom-studio-designer-command-contribution-test)

    add_executable(umicom-studio-editor-command-contribution-test tests/test_editor_command_contribution.c)
    target_link_libraries(umicom-studio-editor-command-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-editor-command-contribution-test)
    umicom_apply_sanitizers(umicom-studio-editor-command-contribution-test)
    add_test(NAME studio.editor_command_contribution COMMAND umicom-studio-editor-command-contribution-test)

    add_executable(umicom-studio-source-navigation-contribution-test tests/test_source_navigation_contribution.c)
    target_link_libraries(umicom-studio-source-navigation-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-source-navigation-contribution-test)
    umicom_apply_sanitizers(umicom-studio-source-navigation-contribution-test)
    add_test(NAME studio.source_navigation_contribution COMMAND umicom-studio-source-navigation-contribution-test)

    add_executable(umicom-studio-navigation-view-contribution-test tests/test_navigation_view_contribution.c)
    target_link_libraries(umicom-studio-navigation-view-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-navigation-view-contribution-test)
    umicom_apply_sanitizers(umicom-studio-navigation-view-contribution-test)
    add_test(NAME studio.navigation_view_contribution COMMAND umicom-studio-navigation-view-contribution-test)

    add_executable(umicom-studio-editor-intelligence-contribution-test tests/test_editor_intelligence_contribution.c)
    target_link_libraries(umicom-studio-editor-intelligence-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-editor-intelligence-contribution-test)
    umicom_apply_sanitizers(umicom-studio-editor-intelligence-contribution-test)
    add_test(NAME studio.editor_intelligence_contribution COMMAND umicom-studio-editor-intelligence-contribution-test)

    add_executable(umicom-studio-editor-assistance-contribution-test tests/test_editor_assistance_contribution.c)
    target_link_libraries(umicom-studio-editor-assistance-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-editor-assistance-contribution-test)
    umicom_apply_sanitizers(umicom-studio-editor-assistance-contribution-test)
    add_test(NAME studio.editor_assistance_contribution COMMAND umicom-studio-editor-assistance-contribution-test)

    add_executable(umicom-studio-editor-productivity-contribution-test tests/test_editor_productivity_contribution.c)
    target_link_libraries(umicom-studio-editor-productivity-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-editor-productivity-contribution-test)
    umicom_apply_sanitizers(umicom-studio-editor-productivity-contribution-test)
    add_test(NAME studio.editor_productivity_contribution COMMAND umicom-studio-editor-productivity-contribution-test)

    add_executable(umicom-studio-editor-refactoring-contribution-test tests/test_editor_refactoring_contribution.c)
    target_link_libraries(umicom-studio-editor-refactoring-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-editor-refactoring-contribution-test)
    umicom_apply_sanitizers(umicom-studio-editor-refactoring-contribution-test)
    add_test(NAME studio.editor_refactoring_contribution COMMAND umicom-studio-editor-refactoring-contribution-test)

    add_executable(umicom-studio-debug-orchestration-contribution-test tests/test_debug_orchestration_contribution.c)
    target_link_libraries(umicom-studio-debug-orchestration-contribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-debug-orchestration-contribution-test)
    umicom_apply_sanitizers(umicom-studio-debug-orchestration-contribution-test)
    add_test(NAME studio.debug_orchestration_contribution COMMAND umicom-studio-debug-orchestration-contribution-test)

    add_executable(umicom-studio-platform-contract-api-bounds-test tests/test_platform_contract_api_bounds.c)
    target_link_libraries(umicom-studio-platform-contract-api-bounds-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-platform-contract-api-bounds-test)
    umicom_apply_sanitizers(umicom-studio-platform-contract-api-bounds-test)
    add_test(NAME studio.platform_contract_api_bounds COMMAND umicom-studio-platform-contract-api-bounds-test)

    add_executable(umicom-studio-service-namespace-inventory-test tests/test_service_namespace_inventory.c)
    target_link_libraries(umicom-studio-service-namespace-inventory-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-service-namespace-inventory-test)
    umicom_apply_sanitizers(umicom-studio-service-namespace-inventory-test)
    add_test(NAME studio.service_namespace_inventory COMMAND umicom-studio-service-namespace-inventory-test)

    add_executable(umicom-studio-workspace-test
        tests/test_workspace.c
    )
    target_link_libraries(umicom-studio-workspace-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-workspace-test)
    umicom_apply_sanitizers(umicom-studio-workspace-test)
    add_test(NAME studio.workspace COMMAND umicom-studio-workspace-test)

    add_executable(umicom-studio-files-test
        tests/test_files.c
    )
    target_link_libraries(umicom-studio-files-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-files-test)
    umicom_apply_sanitizers(umicom-studio-files-test)
    add_test(NAME studio.files COMMAND umicom-studio-files-test)

    add_executable(umicom-studio-search-test
        tests/test_search.c
    )
    target_link_libraries(umicom-studio-search-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-search-test)
    umicom_apply_sanitizers(umicom-studio-search-test)
    add_test(NAME studio.search COMMAND umicom-studio-search-test)

    add_executable(umicom-studio-processes-test
        tests/test_processes.c
    )
    target_link_libraries(umicom-studio-processes-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-processes-test)
    umicom_apply_sanitizers(umicom-studio-processes-test)
    add_test(NAME studio.processes COMMAND umicom-studio-processes-test)

    add_executable(umicom-studio-watcher-test
        tests/test_watcher.c
    )
    target_link_libraries(umicom-studio-watcher-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-watcher-test)
    umicom_apply_sanitizers(umicom-studio-watcher-test)
    add_test(NAME studio.watcher COMMAND umicom-studio-watcher-test)

    add_executable(umicom-studio-data-test tests/test_data.c)
    target_link_libraries(umicom-studio-data-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-data-test)
    add_test(NAME studio.data COMMAND umicom-studio-data-test)

    foreach(UMICOM_STUDIO_DATABASE_TEST
            database_explorer_centre
            database_explorer_connections
            database_explorer_queries
            database_explorer_migrations
            database_explorer_transfer
            database_explorer_views
            database_explorer_commands)
        string(REPLACE "_" "-" UMICOM_STUDIO_DATABASE_TEST_TARGET
            "${UMICOM_STUDIO_DATABASE_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_DATABASE_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_DATABASE_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_DATABASE_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DATABASE_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DATABASE_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_DATABASE_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_DATABASE_TEST_TARGET}-test)
    endforeach()

    add_executable(umicom-studio-messages-test tests/test_messages.c)
    target_link_libraries(umicom-studio-messages-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-messages-test)
    add_test(NAME studio.messages COMMAND umicom-studio-messages-test)

    add_executable(umicom-studio-history-test tests/test_history.c)
    target_link_libraries(umicom-studio-history-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-history-test)
    add_test(NAME studio.history COMMAND umicom-studio-history-test)

    add_executable(umicom-studio-replay-test tests/test_replay.c)
    target_link_libraries(umicom-studio-replay-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-replay-test)
    add_test(NAME studio.replay COMMAND umicom-studio-replay-test)

    add_executable(umicom-studio-fabric-test tests/test_fabric.c)
    target_link_libraries(umicom-studio-fabric-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-fabric-test)
    add_test(NAME studio.fabric COMMAND umicom-studio-fabric-test)

    add_executable(umicom-studio-outbox-test tests/test_outbox.c)
    target_link_libraries(umicom-studio-outbox-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-outbox-test)
    add_test(NAME studio.outbox COMMAND umicom-studio-outbox-test)

    add_executable(umicom-studio-persistence-test tests/test_persistence.c)
    target_link_libraries(umicom-studio-persistence-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-persistence-test)
    add_test(NAME studio.persistence COMMAND umicom-studio-persistence-test)

    add_executable(umicom-studio-dead-letters-test tests/test_dead_letters.c)
    target_link_libraries(umicom-studio-dead-letters-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-dead-letters-test)
    add_test(NAME studio.dead_letters COMMAND umicom-studio-dead-letters-test)

    foreach(UMICOM_STUDIO_OPERATION_TEST
            operations
            security
            plugins
            extension_platform
            product_centre
            observability
            resilience)
        add_executable(umicom-studio-${UMICOM_STUDIO_OPERATION_TEST}-test
            tests/test_${UMICOM_STUDIO_OPERATION_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_OPERATION_TEST}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_OPERATION_TEST}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_OPERATION_TEST}-test)
        add_test(NAME studio.${UMICOM_STUDIO_OPERATION_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_OPERATION_TEST}-test)
    endforeach()

    foreach(UMICOM_STUDIO_UI_TEST
            ui
            workbench
            perspectives
            contributions
            view_models
            ui_catalogue
            ui_headless
            ui_services
            ui_session
            ui_actions
            application_authority
            desktop_shell
            federated_content
            federated_interactions
            federated_workspace_session
            extension_centre
            product_centre_views)
        add_executable(umicom-studio-${UMICOM_STUDIO_UI_TEST}-test
            tests/test_${UMICOM_STUDIO_UI_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_UI_TEST}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_UI_TEST}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_UI_TEST}-test)
        add_test(NAME studio.${UMICOM_STUDIO_UI_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_UI_TEST}-test)
    endforeach()

    foreach(UMICOM_STUDIO_DEVELOPER_TEST
            build_service
            build_workbench
            build_workspace_views
            diagnostics_pipeline
            test_service
            test_explorer_state
            test_explorer_operations
            test_explorer_hierarchy
            test_workspace_views
            test_execution_bridge
            terminal_service
            terminal_workbench
            language_service
            debugger_service
            debug_workspace_views
            source_control_service
            source_control_workspace
            trading_service
            trading_workspace_views
            developer_platform
            universal_development)
        string(REPLACE "_" "-" UMICOM_STUDIO_DEVELOPER_TEST_TARGET
            "${UMICOM_STUDIO_DEVELOPER_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_DEVELOPER_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DEVELOPER_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_DEVELOPER_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_DEVELOPER_TEST_TARGET}-test)
    endforeach()

    foreach(UMICOM_STUDIO_DESIGNER_TEST
            declarative
            designer
            designer_history
            designer_catalogue
            designer_preview
            new_project
            designer_state
            designer_project
            visual_builder_centre
            visual_builder_tree
            visual_builder_properties
            visual_builder_bindings
            visual_builder_interactions
            visual_builder_drag_drop
            visual_builder_clipboard
            visual_builder_templates
            visual_builder_preview_source
            visual_builder_settings_views)
        string(REPLACE "_" "-" UMICOM_STUDIO_DESIGNER_TEST_TARGET
            "${UMICOM_STUDIO_DESIGNER_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_DESIGNER_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_DESIGNER_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_DESIGNER_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DESIGNER_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DESIGNER_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_DESIGNER_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_DESIGNER_TEST_TARGET}-test)
    endforeach()


    foreach(UMICOM_STUDIO_WEB_TEST
            web_platform
            web
            browser
            charting
            charting_indicator
            frontend
            frontend_manifest
            web_metrics
            web_not_found
            web_service_accessor
            frontend_studio_centre
            frontend_studio_settings
            frontend_studio_preview
            frontend_studio_inspector
            frontend_studio_views
            frontend_studio_commands
            frontend_studio_quality)
        string(REPLACE "_" "-" UMICOM_STUDIO_WEB_TEST_TARGET
            "${UMICOM_STUDIO_WEB_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_WEB_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_WEB_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_WEB_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_WEB_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_WEB_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_WEB_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_WEB_TEST_TARGET}-test)
    endforeach()


    foreach(UMICOM_STUDIO_AI_TEST
            ai_platform
            ai_chat
            ai_retrieval
            ai_tools
            authorengine
            authorengine_integration
            authorengine_views
            ai_workspace
            coding_assistant
            ai_coding_views
            knowledge_centre
            knowledge_settings
            knowledge_views
            knowledge_archive
            knowledge_boundary
            helix
            helix_evidence
            helix_fitness
            helix_generation
            helix_agents
            helix_agent_centre
            helix_agent_commands
            helix_agent_adapters
            helix_agent_views
            helix_agent_candidates
            helix_agent_journal
            helix_agent_settings)
        string(REPLACE "_" "-" UMICOM_STUDIO_AI_TEST_TARGET
            "${UMICOM_STUDIO_AI_TEST}")
        add_executable(umicom-studio-${UMICOM_STUDIO_AI_TEST_TARGET}-test
            tests/test_${UMICOM_STUDIO_AI_TEST}.c)
        target_link_libraries(umicom-studio-${UMICOM_STUDIO_AI_TEST_TARGET}-test
            PRIVATE Umicom::StudioCore)
        umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_AI_TEST_TARGET}-test)
        umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_AI_TEST_TARGET}-test)
        add_test(NAME studio.${UMICOM_STUDIO_AI_TEST}
            COMMAND umicom-studio-${UMICOM_STUDIO_AI_TEST_TARGET}-test)
    endforeach()

    # Studio integration tests share the default .umicom session/recovery
    # directory.  A named CTest resource lock preserves parallel Framework
    # testing while preventing Studio tests from deleting one another's state.
    set_tests_properties(
        studio.framework_bootstrap
        studio.version
        studio.services
        studio.doctor
        studio.diagnostics
        studio.platform
        studio.settings
        studio.documents
        studio.document_workbench
        studio.tasks
        studio.commands
        studio.state
        studio.workspace
        studio.files
        studio.search
        studio.processes
        studio.watcher
        studio.data
        studio.messages
        studio.history
        studio.replay
        studio.fabric
        studio.outbox
        studio.persistence
        studio.dead_letters
        studio.operations
        studio.security
        studio.plugins
        studio.extension_platform
        studio.product_centre
        studio.observability
        studio.resilience
        studio.ui
        studio.workbench
        studio.perspectives
        studio.contributions
        studio.view_models
        studio.ui_catalogue
        studio.ui_headless
        studio.ui_services
        studio.ui_session
        studio.ui_actions
        studio.application_authority
        studio.desktop_shell
        studio.federated_content
        studio.federated_interactions
        studio.federated_workspace_session
        studio.extension_centre
        studio.product_centre_views
        studio.trading_service
        studio.trading_workspace_views
        studio.build_service
        studio.build_workbench
        studio.build_workspace_views
        studio.test_service
        studio.test_workspace_views
        studio.terminal_service
        studio.language_service
        studio.debugger_service
        studio.debug_workspace_views
        studio.source_control_service
        studio.developer_platform
        studio.ai_platform
        studio.ai_chat
        studio.ai_retrieval
        studio.ai_tools
        studio.authorengine
        studio.authorengine_integration
        studio.authorengine_views
        studio.ai_workspace
        studio.coding_assistant
        studio.ai_coding_views
        studio.knowledge_centre
        studio.knowledge_settings
        studio.knowledge_views
        studio.knowledge_archive
        studio.knowledge_boundary
        studio.helix
        studio.helix_evidence
        studio.helix_fitness
        studio.helix_generation
        studio.helix_agents
        studio.helix_agent_centre
        studio.helix_agent_commands
        studio.helix_agent_adapters
        studio.helix_agent_views
        studio.helix_agent_candidates
        studio.helix_agent_journal
        studio.helix_agent_settings
        PROPERTIES RESOURCE_LOCK umicom_studio_runtime_state
    )

    # Application delivery and release-engineering integration tests.
    add_executable(umicom-studio-delivery-platform-test tests/test_delivery_platform.c)
    target_link_libraries(umicom-studio-delivery-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-platform-test)
    umicom_apply_sanitizers(umicom-studio-delivery-platform-test)
    add_test(NAME studio.delivery_platform COMMAND umicom-studio-delivery-platform-test)

    add_executable(umicom-studio-delivery-release-test tests/test_delivery_release.c)
    target_link_libraries(umicom-studio-delivery-release-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-release-test)
    umicom_apply_sanitizers(umicom-studio-delivery-release-test)
    add_test(NAME studio.delivery_release COMMAND umicom-studio-delivery-release-test)

    add_executable(umicom-studio-delivery-package-test tests/test_delivery_package.c)
    target_link_libraries(umicom-studio-delivery-package-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-package-test)
    umicom_apply_sanitizers(umicom-studio-delivery-package-test)
    add_test(NAME studio.delivery_package COMMAND umicom-studio-delivery-package-test)

    add_executable(umicom-studio-delivery-update-test tests/test_delivery_update.c)
    target_link_libraries(umicom-studio-delivery-update-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-update-test)
    umicom_apply_sanitizers(umicom-studio-delivery-update-test)
    add_test(NAME studio.delivery_update COMMAND umicom-studio-delivery-update-test)

    add_executable(umicom-studio-delivery-report-test tests/test_delivery_report.c)
    target_link_libraries(umicom-studio-delivery-report-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-report-test)
    umicom_apply_sanitizers(umicom-studio-delivery-report-test)
    add_test(NAME studio.delivery_report COMMAND umicom-studio-delivery-report-test)

    add_executable(umicom-studio-delivery-distribution-test tests/test_delivery_distribution.c)
    target_link_libraries(umicom-studio-delivery-distribution-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-distribution-test)
    umicom_apply_sanitizers(umicom-studio-delivery-distribution-test)
    add_test(NAME studio.delivery_distribution COMMAND umicom-studio-delivery-distribution-test)

    add_executable(umicom-studio-delivery-evidence-test tests/test_delivery_evidence.c)
    target_link_libraries(umicom-studio-delivery-evidence-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-evidence-test)
    umicom_apply_sanitizers(umicom-studio-delivery-evidence-test)
    add_test(NAME studio.delivery_evidence COMMAND umicom-studio-delivery-evidence-test)

    add_executable(umicom-studio-delivery-services-test tests/test_delivery_services.c)
    target_link_libraries(umicom-studio-delivery-services-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-delivery-services-test)
    umicom_apply_sanitizers(umicom-studio-delivery-services-test)
    add_test(NAME studio.delivery_services COMMAND umicom-studio-delivery-services-test)


    # ABI, SDK and reference-product conformance tests.
    add_executable(umicom-studio-compatibility-platform-test tests/test_compatibility_platform.c)
    target_link_libraries(umicom-studio-compatibility-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-compatibility-platform-test)
    add_test(NAME studio.compatibility_platform COMMAND umicom-studio-compatibility-platform-test)

    add_executable(umicom-studio-sdk-platform-test tests/test_sdk_platform.c)
    target_link_libraries(umicom-studio-sdk-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-sdk-platform-test)
    add_test(NAME studio.sdk_platform COMMAND umicom-studio-sdk-platform-test)

    add_executable(umicom-studio-products-platform-test tests/test_products_platform.c)
    target_link_libraries(umicom-studio-products-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-products-platform-test)
    add_test(NAME studio.products_platform COMMAND umicom-studio-products-platform-test)

    add_executable(umicom-studio-conformance-platform-test tests/test_conformance_platform.c)
    target_link_libraries(umicom-studio-conformance-platform-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-conformance-platform-test)
    add_test(NAME studio.conformance_platform COMMAND umicom-studio-conformance-platform-test)

    add_executable(umicom-studio-architecture-guard-test tests/test_architecture_guard.c)
    target_link_libraries(umicom-studio-architecture-guard-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-architecture-guard-test)
    add_test(NAME studio.architecture_guard COMMAND umicom-studio-architecture-guard-test)

    add_executable(umicom-studio-reference-products-test tests/test_reference_products.c)
    target_link_libraries(umicom-studio-reference-products-test PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-reference-products-test)
    add_test(NAME studio.reference_products COMMAND umicom-studio-reference-products-test)

endif()

install(TARGETS
    umicom-studio-console
    umicom-studio-doctor
    umicom-studio-diagnostics
    umicom-studio-settings
    umicom-studio-platform
    umicom-studio-toolchain
    umicom-studio-state
    umicom-studio-workspace
    umicom-studio-process
    umicom-studio-data
    umicom-studio-database-explorer
    umicom-studio-integration-designer
    umicom-studio-quality-centre
    umicom-studio-messages
    umicom-studio-replay
    umicom-studio-history
    umicom-studio-security
    umicom-studio-security-centre
    umicom-studio-operations-centre
    umicom-studio-workspace-designer
    umicom-studio-compiler-centre
    umicom-studio-polyglot-compiler
    umicom-studio-distribution-centre
    umicom-studio-package-inspector
    umicom-studio-plugins
    umicom-studio-observability
    umicom-studio-resilience
    umicom-studio-operations
    umicom-studio-workbench
    umicom-studio-ui-catalogue
    umicom-studio-build
    umicom-studio-tests
    umicom-studio-terminal
    umicom-studio-language
    umicom-studio-debugger
    umicom-studio-source-control
    umicom-studio-developer
    umicom-studio-developer-pipeline
    umicom-studio-developer-state
    umicom-studio-visual-builder
    umicom-studio-ai
    umicom-studio-retrieval
    umicom-studio-authorengine
    umicom-studio-knowledge
    umicom-studio-helix
    umicom-studio-helix-evidence
    umicom-studio-helix-generation
    umicom-studio-delivery
    umicom-studio-package
    umicom-studio-release
    umicom-studio-update
    umicom-studio-distribution
    umicom-studio-evidence
    umicom-studio-rollback
    umicom-studio-pipeline
    umicom-studio-abi
    umicom-studio-sdk
    umicom-studio-products
    umicom-studio-compatibility
    umicom-studio-conformance
    umicom-studio-architecture
    RUNTIME DESTINATION bin
)

# The interactive IDE is optional, so install it only when the GTK product was
# built.  This makes the new Deploy action and documented cmake --install flow
# produce a launchable Studio application instead of tools alone.
if(TARGET umicom-studio-ide)
    install(TARGETS umicom-studio-ide RUNTIME DESTINATION bin)
endif()

if(BUILD_TESTING)
add_executable(umicom-studio-codeguard-test tests/test_codeguard.c)
target_link_libraries(umicom-studio-codeguard-test PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-codeguard-test)
add_test(NAME studio.codeguard_platform COMMAND umicom-studio-codeguard-test)
add_executable(umicom-studio-architecture-quality-test tests/test_architecture_quality.c)
target_link_libraries(umicom-studio-architecture-quality-test PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-architecture-quality-test)
add_test(NAME studio.architecture_quality COMMAND umicom-studio-architecture-quality-test)
add_executable(umicom-studio-duplicate-review-test tests/test_duplicate_review.c)
target_link_libraries(umicom-studio-duplicate-review-test PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-duplicate-review-test)
add_test(NAME studio.duplicate_review COMMAND umicom-studio-duplicate-review-test)
add_executable(umicom-studio-quality-gate-test tests/test_quality_gate.c)
target_link_libraries(umicom-studio-quality-gate-test PRIVATE Umicom::StudioCore)
umicom_apply_warnings(umicom-studio-quality-gate-test)
add_test(NAME studio.quality_gate COMMAND umicom-studio-quality-gate-test)
foreach(UMICOM_STUDIO_QUALITY_TEST
        quality_centre
        quality_evidence_navigation
        quality_remediation
        quality_duplicates
        quality_compatibility
        quality_gates
        quality_views
        quality_commands)
    string(REPLACE "_" "-" UMICOM_STUDIO_QUALITY_TEST_TARGET
        "${UMICOM_STUDIO_QUALITY_TEST}")
    add_executable(umicom-studio-${UMICOM_STUDIO_QUALITY_TEST_TARGET}-test
        tests/test_${UMICOM_STUDIO_QUALITY_TEST}.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_QUALITY_TEST_TARGET}-test
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_QUALITY_TEST_TARGET}-test)
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_QUALITY_TEST_TARGET}-test)
    add_test(NAME studio.${UMICOM_STUDIO_QUALITY_TEST}
        COMMAND umicom-studio-${UMICOM_STUDIO_QUALITY_TEST_TARGET}-test)
endforeach()
endif()

if(BUILD_TESTING)
foreach(UMICOM_STUDIO_SECURITY_TEST
        security_centre
        security_identities_roles
        security_secrets
        security_approvals
        security_packages
        security_audit_view
        security_views
        security_commands)
    string(REPLACE "_" "-" UMICOM_STUDIO_SECURITY_TEST_TARGET
        "${UMICOM_STUDIO_SECURITY_TEST}")
    add_executable(umicom-studio-${UMICOM_STUDIO_SECURITY_TEST_TARGET}-test
        tests/test_${UMICOM_STUDIO_SECURITY_TEST}.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_SECURITY_TEST_TARGET}-test
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_SECURITY_TEST_TARGET}-test)
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_SECURITY_TEST_TARGET}-test)
    add_test(NAME studio.${UMICOM_STUDIO_SECURITY_TEST}
        COMMAND umicom-studio-${UMICOM_STUDIO_SECURITY_TEST_TARGET}-test)
endforeach()
endif()

if(BUILD_TESTING)
foreach(UMICOM_STUDIO_WORKSPACE_TEST
        operations_centre
        operations_views_commands
        workspace_catalogue
        workspace_layouts
        workspace_profiles
        workspace_profile_session
        editor_layout_session
        workspace_groups
        workspace_themes
        appearance_centre
        appearance_session
        editor_theme_integration
        workspace_persistence
        workspace_views_commands)
    string(REPLACE "_" "-" UMICOM_STUDIO_WORKSPACE_TEST_TARGET
        "${UMICOM_STUDIO_WORKSPACE_TEST}")
    add_executable(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test
        tests/test_${UMICOM_STUDIO_WORKSPACE_TEST}.c)
    target_include_directories(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test
        PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/tests")
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test)
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test)
    add_test(NAME studio.${UMICOM_STUDIO_WORKSPACE_TEST}
        COMMAND umicom-studio-${UMICOM_STUDIO_WORKSPACE_TEST_TARGET}-test)
endforeach()
endif()

if(BUILD_TESTING)
foreach(UMICOM_STUDIO_COMPILER_TEST
        compiler_catalogue
        compiler_views_commands
        compiler_templates
        compiler_settings_binding
        compiler_diagnostics
        compiler_interop
        umicc_polyglot
        compiler_centre)
    string(REPLACE "_" "-" UMICOM_STUDIO_COMPILER_TEST_TARGET
        "${UMICOM_STUDIO_COMPILER_TEST}")
    add_executable(umicom-studio-${UMICOM_STUDIO_COMPILER_TEST_TARGET}-test
        tests/test_${UMICOM_STUDIO_COMPILER_TEST}.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_COMPILER_TEST_TARGET}-test
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_COMPILER_TEST_TARGET}-test)
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_COMPILER_TEST_TARGET}-test)
    add_test(NAME studio.${UMICOM_STUDIO_COMPILER_TEST}
        COMMAND umicom-studio-${UMICOM_STUDIO_COMPILER_TEST_TARGET}-test)
endforeach()
endif()

if(BUILD_TESTING)
foreach(UMICOM_STUDIO_DISTRIBUTION_TEST
        sdk_centre
        package_project
        dependency_bundle
        installer_centre
        supply_chain
        release_upgrade
        distribution_centre
        distribution_views_commands)
    string(REPLACE "_" "-" UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET
        "${UMICOM_STUDIO_DISTRIBUTION_TEST}")
    add_executable(umicom-studio-${UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET}-test
        tests/test_${UMICOM_STUDIO_DISTRIBUTION_TEST}.c)
    target_link_libraries(umicom-studio-${UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET}-test
        PRIVATE Umicom::StudioCore)
    umicom_apply_warnings(umicom-studio-${UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET}-test)
    umicom_apply_sanitizers(umicom-studio-${UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET}-test)
    add_test(NAME studio.${UMICOM_STUDIO_DISTRIBUTION_TEST}
        COMMAND umicom-studio-${UMICOM_STUDIO_DISTRIBUTION_TEST_TARGET}-test)
endforeach()
endif()
