/*----------------------------------------------------------------------------- 
 * Umicom Studio IDE
 * File: tests/test_llm_body.c
 * PURPOSE: Test stub to satisfy umi_llm_cfg_init_from_env symbol (weak no-op)
 * Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-07 | MIT
 *---------------------------------------------------------------------------*/
#include <glib.h>
#include <umicom/llm.h>

#ifndef UMI_TEST_STUB_LLM_CFG
#define UMI_TEST_STUB_LLM_CFG
__attribute__((weak)) void umi_llm_cfg_init_from_env(UmiLlmCfg *cfg) { (void)cfg; }
#endif