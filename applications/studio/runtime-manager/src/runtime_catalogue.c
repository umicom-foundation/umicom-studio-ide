/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/runtime-manager/src/runtime_catalogue.c
 *
 * PURPOSE:
 *   Build the default Umicom application catalogue displayed by Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This file is part of the Studio Application Hub / Runtime Manager.
 * The code is intentionally explicit and bounded so a new C developer can
 * follow the data flow without hidden ownership or private product coupling.
 */

#include "umicom/studio/runtime_catalogue.h"

#include <stdio.h>
#include <string.h>

typedef struct CatalogueDefinition {
    const char *id;
    const char *name;
    const char *executable;
    UmiStudioRuntimeCategory category;
    const char *description;
    const char *capabilities[6];
} CatalogueDefinition;

static const CatalogueDefinition DEFINITIONS[] = {
    {
        "umicom.studio-ide",
        "Umicom Studio IDE",
        "umicom-studio-ide",
        UMI_STUDIO_RUNTIME_CATEGORY_DEVELOPMENT,
        "C23-first IDE, visual development environment and Umicom application workbench.",
        {"studio.ide", "developer.tools", "ui.gtk4", "integration.client", NULL}
    },
    {
        "umicom.trader",
        "Umicom Trader",
        "umicom-trader",
        UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
        "Trading workstation for market data, execution, risk, replay and research.",
        {"finance.trading", "market.data", "risk", "research", NULL}
    },
    {
        "umicom.tms",
        "Umicom TMS",
        "umicom-tms",
        UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
        "Trade-management system for products, books, lifecycle, workflow and settlements.",
        {"finance.tms", "trade.lifecycle", "workflow", NULL}
    },
    {
        "umicom.bank",
        "Umicom Bank",
        "umicom-bank",
        UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
        "Banking application foundation for accounts, payments, ledger and controls.",
        {"finance.bank", "accounts", "payments", "ledger", NULL}
    },
    {
        "umicom.llm",
        "Umicom LLM",
        "umicom-llm",
        UMI_STUDIO_RUNTIME_CATEGORY_AI,
        "Local and remote language-model workbench, inference and knowledge tooling.",
        {"ai.inference", "ai.models", "ai.rag", NULL}
    },
    {
        "umicom.authorengine",
        "Umicom AuthorEngine AI",
        "uaengine",
        UMI_STUDIO_RUNTIME_CATEGORY_SERVICE,
        "Shared AI orchestration, document ingestion and model-provider service.",
        {"ai.orchestration", "documents.ingest", "ai.provider", NULL}
    },
    {
        "umicom.book",
        "Umicom Book",
        "umicom-book",
        UMI_STUDIO_RUNTIME_CATEGORY_CREATOR,
        "Structured book, research, authoring and publishing application.",
        {"creator.book", "documents", "publishing", NULL}
    },
    {
        "umicom.browser",
        "Umicom Browser",
        "umicom-browser",
        UMI_STUDIO_RUNTIME_CATEGORY_CREATOR,
        "Browser and web-application shell with Umicom integration bridge.",
        {"browser", "web", "integration.client", NULL}
    },
    {
        "umicom.studio-media",
        "Umicom Studio Media",
        "umicom-studio-media",
        UMI_STUDIO_RUNTIME_CATEGORY_CREATOR,
        "Media production and asset-management workspace.",
        {"creator.media", "media.assets", NULL}
    },
    {
        "umicom.studio-music",
        "Umicom Studio Music",
        "umicom-studio-music",
        UMI_STUDIO_RUNTIME_CATEGORY_CREATOR,
        "Music, audio and composition workspace.",
        {"creator.music", "audio", NULL}
    },
    {
        "umicom.umicoin",
        "UmiCoin",
        "umicoin",
        UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
        "UmiCoin wallet, node and network application.",
        {"finance.coin", "wallet", "network", NULL}
    },
    {
        "umicom.commodity-exchange",
        "Umicom Commodity Exchange",
        "umicom-commodity-exchange",
        UMI_STUDIO_RUNTIME_CATEGORY_FINANCE,
        "Commodity-contract, market and exchange application.",
        {"finance.exchange", "orderbook", "market.data", NULL}
    },
    {
        "umicom.os",
        "Umicom OS",
        "umicom-os",
        UMI_STUDIO_RUNTIME_CATEGORY_SYSTEM,
        "Linux-based Umicom operating-system user-space and desktop environment.",
        {"system.os", "ui.gtk4", "application.host", NULL}
    }
};

static UmiStatus add_definition(UmiStudioRuntimeManager *manager,
                                const CatalogueDefinition *definition)
{
    UmiIntegrationApplication application;
    size_t capability_index;
    UmiStatus status;

    umi_integration_application_init(&application);
    status = umi_integration_application_set_identity(
        &application,
        definition->id,
        definition->name
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }

    (void)snprintf(application.executable,
                   sizeof(application.executable),
                   "%s",
                   definition->executable);

    application.enabled = false;

    for (capability_index = 0U;
         definition->capabilities[capability_index] != NULL;
         ++capability_index) {
        status = umi_integration_application_add_capability(
            &application,
            definition->capabilities[capability_index]
        );
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    return umi_studio_runtime_manager_register(manager,
                                               &application,
                                               definition->category,
                                               definition->description);
}

UmiStatus umi_studio_runtime_catalogue_populate(
    UmiStudioRuntimeManager *manager)
{
    size_t index;
    UmiStatus status;

    if (manager == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    for (index = 0U; index < sizeof(DEFINITIONS) / sizeof(DEFINITIONS[0]);
         ++index) {
        status = add_definition(manager, &DEFINITIONS[index]);
        if (status != UMI_STATUS_OK) {
            return status;
        }
    }

    /* Studio itself is the current process when the Hub runs from Studio. */
    status = umi_studio_runtime_manager_set_state(
        manager,
        "umicom.studio-ide",
        UMI_INTEGRATION_APP_RUNNING
    );
    if (status != UMI_STATUS_OK) {
        return status;
    }

    {
        UmiStudioRuntimeEntry *studio =
            umi_studio_runtime_manager_find(manager, "umicom.studio-ide");
        if (studio != NULL) {
            studio->installed = true;
            studio->favourite = true;
        }
    }

    return UMI_STATUS_OK;
}
