#-----------------------------------------------------------------------------
# Umicom Trader
# File: applications/trader/CMakeLists.txt
#
# PURPOSE:
#   Build the independent headless Umicom Trader foundation over public
#   Umicom Framework finance, trading, chart, messaging and data contracts.
#
# Created by: Sammy Hegab
# Organisation: Umicom Foundation
# Licence: MIT
#-----------------------------------------------------------------------------

# BEGINNER NOTE:
# This directory is an application composition, not another Framework copy.
# Reusable market data, order, risk, replay and research code remains in the
# Framework libraries linked below.

if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    cmake_minimum_required(VERSION 3.24)
    project(UmicomTrader LANGUAGES C)
    set(CMAKE_C_STANDARD 23)
    set(CMAKE_C_STANDARD_REQUIRED ON)
    set(CMAKE_C_EXTENSIONS OFF)
    include(CTest)

    if(NOT TARGET Umicom::Framework)
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../framework/CMakeLists.txt")
            add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../framework"
                             "${CMAKE_CURRENT_BINARY_DIR}/framework")
        else()
            find_package(UmicomFramework CONFIG REQUIRED)
        endif()
    endif()

    if(NOT COMMAND umicom_apply_warnings)
        function(umicom_apply_warnings target)
            if(MSVC)
                target_compile_options(${target} PRIVATE /W4)
            else()
                target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Wshadow)
            endif()
        endfunction()
    endif()

    if(NOT COMMAND umicom_apply_sanitizers)
        function(umicom_apply_sanitizers target)
        endfunction()
    endif()
endif()

add_library(umicom_trader_core STATIC
    src/app/application.c
    src/app/broker_settings.c
    src/app/commands.c
    src/app/market_workspace.c
    src/app/order_ticket.c
    src/app/positions.c
    src/app/replay_lab.c
    src/app/research.c
    src/app/research_report.c
    src/app/risk_dashboard.c
    src/app/services.c
    src/app/status.c
    src/app/watchlist.c
)
add_library(Umicom::TraderCore ALIAS umicom_trader_core)
target_include_directories(umicom_trader_core PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_link_libraries(umicom_trader_core PUBLIC
    Umicom::Framework
    Umicom::finance
    Umicom::trading
)
umicom_apply_warnings(umicom_trader_core)
umicom_apply_sanitizers(umicom_trader_core)

function(umicom_add_trader_command target source)
    add_executable(${target} ${source})
    target_link_libraries(${target} PRIVATE Umicom::TraderCore)
    umicom_apply_warnings(${target})
    umicom_apply_sanitizers(${target})
endfunction()

umicom_add_trader_command(umicom-trader src/cli/main.c)
umicom_add_trader_command(umicom-trader-research src/cli/research_main.c)
umicom_add_trader_command(umicom-trader-replay src/cli/replay_main.c)
umicom_add_trader_command(umicom-trader-risk src/cli/risk_main.c)
umicom_add_trader_command(umicom-trader-commands src/cli/commands_main.c)

if(BUILD_TESTING)
    function(umicom_add_trader_test target test_name source)
        add_executable(${target} ${source})
        target_link_libraries(${target} PRIVATE Umicom::TraderCore)
        umicom_apply_warnings(${target})
        umicom_apply_sanitizers(${target})
        add_test(NAME ${test_name} COMMAND ${target})
    endfunction()
    umicom_add_trader_test(umicom-trader-application-test trader.application tests/test_application.c)
    umicom_add_trader_test(umicom-trader-broker-settings-test trader.broker_settings tests/test_broker_settings.c)
    umicom_add_trader_test(umicom-trader-commands-test trader.commands tests/test_commands.c)
    umicom_add_trader_test(umicom-trader-market-workspace-test trader.market_workspace tests/test_market_workspace.c)
    umicom_add_trader_test(umicom-trader-replay-lab-test trader.replay_lab tests/test_replay_lab.c)
    umicom_add_trader_test(umicom-trader-research-test trader.research tests/test_research.c)
    umicom_add_trader_test(umicom-trader-risk-dashboard-test trader.risk_dashboard tests/test_risk_dashboard.c)
    umicom_add_trader_test(umicom-trader-status-test trader.status tests/test_status.c)
endif()
