/*-----------------------------------------------------------------------------
 * Umicom Studio IDE : OpenSource IDE for developers and Content Creators
 * Repository: https://github.com/umicom-foundation/umicom-studio-ide
 * File: src/include/umi_diagnostics_all.h
 *
 * PURPOSE:
 *   Convenience umbrella that re-exports the canonical diagnostics model
 *   (UmiDiag/UmiDiagSeverity) and the lightweight UmiOutputSink interface.
 *   Use this in modules that need both; otherwise prefer including the
 *   specific header you need to keep compile dependencies minimal.
 *
 * DESIGN:
 *   - Zero logic; just includes. Keeps layering intact:
 *       umi_output_sink.h -> #include "umi_diagnostics.h"
 *     This header includes them in the correct order.
 *
 * USAGE:
 *   // Instead of:
 *   //   #include "umi_diagnostics.h"
 *   //   #include "umi_output_sink.h"
 *   // you can do:
 *   //   #include "umi_diagnostics_all.h"
 *
 * Created by: Umicom Foundation | Developer: Sammy Hegab | Date: 2025-10-13 | MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_USIDE_UMI_DIAGNOSTICS_ALL_H
#define UMICOM_USIDE_UMI_DIAGNOSTICS_ALL_H

#include "umi_diagnostics.h"
#include "umi_output_sink.h"

#endif /* UMICOM_USIDE_UMI_DIAGNOSTICS_ALL_H */
