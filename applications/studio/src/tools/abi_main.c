/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/tools/abi_main.c
 *
 * PURPOSE:
 *   Report the Framework ABI and current platform from Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This command exposes one compatibility check in a form that is easy to run from PowerShell, CI, or the future Studio workbench UI.
 */
#include <stdio.h>
#include "umicom/umicom.h"
int main(void){UmiAbiPlatform p=umi_abi_platform_current();printf("Framework ABI: %u\nPlatform: %s/%s\nPointer size: %zu\n",UMICOM_FRAMEWORK_ABI_VERSION,p.os,p.architecture,p.pointer_size);return 0;}
