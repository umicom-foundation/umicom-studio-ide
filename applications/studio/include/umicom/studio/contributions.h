/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/include/umicom/studio/contributions.h
 *
 * PURPOSE:
 *   Declare Studio contributions for panes, actions, menus, toolbars, status
 *   items, Activity Bar destinations and extension-point metadata on the
 *   Framework workbench.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Pane IDs are Studio product composition. The pane model, visibility, layout,
 * activity and view-container behaviour remain reusable Umicom Framework code.
 */

#ifndef UMICOM_STUDIO_CONTRIBUTIONS_H
#define UMICOM_STUDIO_CONTRIBUTIONS_H

#include "umicom/umicom.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UMI_STUDIO_PANE_EXPLORER "studio.pane.explorer"
#define UMI_STUDIO_PANE_EDITOR "studio.pane.editor"
#define UMI_STUDIO_PANE_OUTPUT "studio.pane.output"
#define UMI_STUDIO_PANE_PROBLEMS "studio.pane.problems"
#define UMI_STUDIO_PANE_SEARCH "studio.pane.search"
#define UMI_STUDIO_PANE_TERMINAL "studio.pane.terminal"
#define UMI_STUDIO_PANE_PROCESSES "studio.pane.processes"
#define UMI_STUDIO_PANE_TASKS "studio.pane.tasks"
#define UMI_STUDIO_PANE_TERMINAL_HISTORY "studio.pane.terminal-history"
#define UMI_STUDIO_PANE_CHAT "studio.pane.chat"
#define UMI_STUDIO_PANE_ARCHITECTURE "studio.pane.architecture"

/* Batch 23 primary-sidebar views. */
#define UMI_STUDIO_PANE_SOURCE_CONTROL "studio.pane.source-control"
#define UMI_STUDIO_PANE_VCS_COMMIT "studio.pane.vcs-commit"
#define UMI_STUDIO_PANE_VCS_HISTORY "studio.pane.vcs-history"
#define UMI_STUDIO_PANE_VCS_BRANCHES "studio.pane.vcs-branches"
#define UMI_STUDIO_PANE_VCS_REMOTES "studio.pane.vcs-remotes"
#define UMI_STUDIO_PANE_VCS_CONFLICTS "studio.pane.vcs-conflicts"
#define UMI_STUDIO_PANE_VCS_DIFF "studio.pane.vcs-diff"
#define UMI_STUDIO_PANE_VCS_OPERATIONS "studio.pane.vcs-operations"
#define UMI_STUDIO_PANE_RUN_DEBUG "studio.pane.run-debug"
#define UMI_STUDIO_PANE_DEBUG_CALL_STACK "studio.pane.debug-call-stack"
#define UMI_STUDIO_PANE_DEBUG_VARIABLES "studio.pane.debug-variables"
#define UMI_STUDIO_PANE_DEBUG_WATCHES "studio.pane.debug-watches"
#define UMI_STUDIO_PANE_DEBUG_BREAKPOINTS "studio.pane.debug-breakpoints"
#define UMI_STUDIO_PANE_DEBUG_CONSOLE "studio.pane.debug-console"
#define UMI_STUDIO_PANE_TESTING "studio.pane.testing"
#define UMI_STUDIO_PANE_TEST_RESULTS "studio.pane.test-results"
#define UMI_STUDIO_PANE_TEST_FAILURES "studio.pane.test-failures"
#define UMI_STUDIO_PANE_TEST_OUTPUT "studio.pane.test-output"
#define UMI_STUDIO_PANE_TEST_COVERAGE "studio.pane.test-coverage"
#define UMI_STUDIO_PANE_TEST_RUNS "studio.pane.test-runs"
#define UMI_STUDIO_PANE_BUILD_DASHBOARD "studio.pane.build-dashboard"
#define UMI_STUDIO_PANE_BUILD_GRAPH "studio.pane.build-graph"
#define UMI_STUDIO_PANE_BUILD_HISTORY "studio.pane.build-history"
#define UMI_STUDIO_PANE_BUILD_OUTPUT "studio.pane.build-output"
#define UMI_STUDIO_PANE_BUILD_ARTIFACTS "studio.pane.build-artifacts"
#define UMI_STUDIO_PANE_BUILD_TASKS "studio.pane.build-tasks"
#define UMI_STUDIO_PANE_TRADING_DASHBOARD "studio.pane.trading-dashboard"
#define UMI_STUDIO_PANE_TRADING_WATCHLIST "studio.pane.trading-watchlist"
#define UMI_STUDIO_PANE_TRADING_DEPTH "studio.pane.trading-depth"
#define UMI_STUDIO_PANE_TRADING_CHART "studio.pane.trading-chart"
#define UMI_STUDIO_PANE_TRADING_ORDER_TICKET \
    "studio.pane.trading-order-ticket"
#define UMI_STUDIO_PANE_TRADING_ORDERS "studio.pane.trading-orders"
#define UMI_STUDIO_PANE_TRADING_EXECUTIONS "studio.pane.trading-executions"
#define UMI_STUDIO_PANE_TRADING_PORTFOLIO_RISK \
    "studio.pane.trading-portfolio-risk"
#define UMI_STUDIO_PANE_DESIGNER "studio.pane.designer"
#define UMI_STUDIO_PANE_APPLICATIONS "studio.pane.applications"
#define UMI_STUDIO_PANE_APPLICATION_COMPONENTS \
    "studio.pane.application-components"
#define UMI_STUDIO_PANE_GTK4_COVERAGE "studio.pane.gtk4-coverage"
#define UMI_STUDIO_PANE_FRAMEWORK "studio.pane.framework"
#define UMI_STUDIO_PANE_AI "studio.pane.ai"
#define UMI_STUDIO_PANE_AI_RUNTIMES "studio.pane.ai-runtimes"
#define UMI_STUDIO_PANE_AI_CONTEXT "studio.pane.ai-context"
#define UMI_STUDIO_PANE_AI_SESSIONS "studio.pane.ai-sessions"
#define UMI_STUDIO_PANE_AI_PRIVACY "studio.pane.ai-privacy"
#define UMI_STUDIO_PANE_AI_CODING "studio.pane.ai-coding"
#define UMI_STUDIO_PANE_AI_CODING_CONTEXT "studio.pane.ai-coding-context"
#define UMI_STUDIO_PANE_AI_PATCH_REVIEW "studio.pane.ai-patch-review"
#define UMI_STUDIO_PANE_KNOWLEDGE "studio.pane.knowledge"
#define UMI_STUDIO_PANE_KNOWLEDGE_COLLECTIONS \
    "studio.pane.knowledge-collections"
#define UMI_STUDIO_PANE_KNOWLEDGE_SOURCES "studio.pane.knowledge-sources"
#define UMI_STUDIO_PANE_KNOWLEDGE_SEARCH "studio.pane.knowledge-search"
#define UMI_STUDIO_PANE_KNOWLEDGE_SOURCE "studio.pane.knowledge-source"
#define UMI_STUDIO_PANE_EXTENSIONS "studio.pane.extensions"
#define UMI_STUDIO_PANE_EXTENSION_CATALOGUE "studio.pane.extension-catalogue"
#define UMI_STUDIO_PANE_EXTENSION_PERMISSIONS "studio.pane.extension-permissions"
#define UMI_STUDIO_PANE_EXTENSION_AUDIT "studio.pane.extension-audit"
#define UMI_STUDIO_PANE_PRODUCT_MARKETPLACE "studio.pane.product-marketplace"
#define UMI_STUDIO_PANE_PRODUCTS_INSTALLED "studio.pane.products-installed"
#define UMI_STUDIO_PANE_PRODUCT_UPDATES "studio.pane.product-updates"
#define UMI_STUDIO_PANE_PRODUCT_TRANSACTIONS "studio.pane.product-transactions"
#define UMI_STUDIO_PANE_PRODUCT_EVIDENCE "studio.pane.product-evidence"

UmiStatus umi_studio_contributions_register(UmiUiWorkbench *workbench);
UmiStatus umi_studio_contributions_register_layout(UmiUiWorkbench *workbench);
size_t umi_studio_contribution_definition_count(void);

#ifdef __cplusplus
}
#endif

#endif
