/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/trading.c
 *
 * PURPOSE:
 *   Create Studio's simulation-first professional trading workspace and seed
 *   deterministic market evidence for immediate, offline-safe exploration.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/studio/trading.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiStudioTradingService {
    UmiTradingWorkspace *workspace;
};

static UmiInstrument make_instrument(const char *instrument_id,
                                     const char *symbol,
                                     const char *venue,
                                     const char *currency,
                                     double multiplier,
                                     int32_t expiry_yyyymmdd)
{
    UmiInstrument instrument = {0};
    (void)snprintf(instrument.instrument_id.value,
                   sizeof(instrument.instrument_id.value), "%s",
                   instrument_id);
    (void)snprintf(instrument.symbol, sizeof(instrument.symbol), "%s",
                   symbol);
    (void)snprintf(instrument.venue, sizeof(instrument.venue), "%s", venue);
    (void)snprintf(instrument.currency.code,
                   sizeof(instrument.currency.code), "%s", currency);
    instrument.multiplier = multiplier;
    instrument.expiry_yyyymmdd = expiry_yyyymmdd;
    return instrument;
}

/* Every price below is deterministic reference data, not a live quote. This
 * makes tests reproducible and prevents Studio from implying broker access. */
static UmiStatus seed_market(UmiTradingWorkspace *workspace,
                             const UmiInstrument *instrument,
                             double bid, double ask, double previous_close,
                             double open, double high, double low, double close,
                             double volume, int64_t event_time_ms)
{
    UmiQuote quote = {0};
    UmiBar bar = {0};
    UmiMarketDepth depth = {0};
    double price_step = (ask - bid) > 0.0 ? (ask - bid) : 0.01;
    UmiStatus status = umi_trading_workspace_add_instrument(
        workspace, instrument);

    if (status != UMI_STATUS_OK) return status;
    quote.instrument = *instrument;
    quote.bid = bid;
    quote.ask = ask;
    quote.bid_size = 18.0;
    quote.ask_size = 16.0;
    quote.event_time_ms = event_time_ms;
    status = umi_trading_workspace_update_quote(workspace, &quote);

    if (status == UMI_STATUS_OK) {
        bar.instrument = *instrument;
        bar.open = open;
        bar.high = high;
        bar.low = low;
        bar.close = close;
        bar.volume = volume;
        bar.start_time_ms = event_time_ms - 60000;
        bar.end_time_ms = event_time_ms;
        status = umi_trading_workspace_update_bar(workspace, &bar,
                                                  previous_close);
    }
    if (status == UMI_STATUS_OK) {
        depth.instrument = *instrument;
        depth.bid_count = 3U;
        depth.ask_count = 3U;
        depth.bids[0].price = bid;
        depth.bids[0].size = 18.0;
        depth.bids[1].price = bid - price_step;
        depth.bids[1].size = 24.0;
        depth.bids[2].price = bid - price_step * 2.0;
        depth.bids[2].size = 31.0;
        depth.asks[0].price = ask;
        depth.asks[0].size = 16.0;
        depth.asks[1].price = ask + price_step;
        depth.asks[1].size = 22.0;
        depth.asks[2].price = ask + price_step * 2.0;
        depth.asks[2].size = 29.0;
        depth.event_time_ms = event_time_ms;
        status = umi_trading_workspace_update_depth(workspace, &depth);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_set_market_state(
            workspace, instrument->instrument_id.value, UMI_MARKET_PREOPEN);
    }
    if (status == UMI_STATUS_OK) {
        status = umi_trading_workspace_set_market_state(
            workspace, instrument->instrument_id.value, UMI_MARKET_OPEN);
    }
    return status;
}

UmiStatus umi_studio_trading_service_create(
    UmiStudioTradingService **out_service)
{
    UmiStudioTradingService *service;
    UmiTradingWorkspaceConfig config;
    UmiInstrument es;
    UmiInstrument nq;
    UmiInstrument eurusd;
    UmiStatus status;

    if (out_service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_service = NULL;
    service = (UmiStudioTradingService *)calloc(1U, sizeof(*service));
    if (service == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    config = umi_trading_workspace_config_default();
    (void)snprintf(config.account_id.value, sizeof(config.account_id.value),
                   "%s", "studio.simulation");
    config.environment = UMI_TRADING_SIMULATION;
    status = umi_trading_workspace_create(&config, &service->workspace);

    es = make_instrument("CME.ES.REFERENCE", "ES", "CME", "USD",
                         50.0, 0);
    nq = make_instrument("CME.NQ.REFERENCE", "NQ", "CME", "USD",
                         20.0, 0);
    eurusd = make_instrument("FX.EURUSD.SPOT", "EURUSD", "FX", "USD",
                             1.0, 0);
    if (status == UMI_STATUS_OK) {
        status = seed_market(service->workspace, &es,
            5624.75, 5625.00, 5610.25, 5612.00, 5631.50, 5608.25,
            5624.75, 128450.0, 1000000);
    }
    if (status == UMI_STATUS_OK) {
        status = seed_market(service->workspace, &nq,
            20324.25, 20324.75, 20270.50, 20282.00, 20351.25, 20261.75,
            20324.50, 86420.0, 1000000);
    }
    if (status == UMI_STATUS_OK) {
        status = seed_market(service->workspace, &eurusd,
            1.09120, 1.09124, 1.08980, 1.09010, 1.09205, 1.08972,
            1.09122, 42150.0, 1000000);
    }
    if (status == UMI_STATUS_OK) {
        /* Simulation needs market and risk readiness only. Broker readiness
         * remains false, keeping paper/live submission unavailable. */
        status = umi_trading_workspace_set_health(service->workspace,
                                                   1, 0, 1);
    }
    if (status != UMI_STATUS_OK) {
        umi_studio_trading_service_destroy(service);
        return status;
    }
    *out_service = service;
    return UMI_STATUS_OK;
}

void umi_studio_trading_service_destroy(UmiStudioTradingService *service)
{
    if (service == NULL) return;
    umi_trading_workspace_destroy(service->workspace);
    free(service);
}

UmiTradingWorkspace *umi_studio_trading_service_workspace(
    UmiStudioTradingService *service)
{
    return service != NULL ? service->workspace : NULL;
}

UmiStatus umi_studio_trading_service_snapshot(
    UmiStudioTradingService *service,
    UmiTradingWorkspaceSnapshot *out_snapshot)
{
    if (service == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_trading_workspace_snapshot(service->workspace, out_snapshot);
}
