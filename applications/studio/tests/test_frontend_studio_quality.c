/* Umicom Studio IDE Tests | Sammy Hegab | Umicom Foundation | MIT */
#include <assert.h>
#include "umicom/studio/frontend_studio_centre.h"
int main(void){UmiStudioWebPlatform *p=NULL;UmiStudioFrontendCentre c;UmiFrontendQualityMetrics m={0};m.transfer_bytes=1000U;m.script_bytes=500U;m.render_us=10000U;m.accessibility_score=100U;assert(umi_studio_web_platform_create(&p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_init(&c,p)==UMI_STATUS_OK);assert(umi_studio_frontend_centre_audit(&c,&m)==UMI_STATUS_OK&&c.latest_audit.passed);m.console_errors=1U;assert(umi_studio_frontend_centre_audit(&c,&m)==UMI_STATUS_OK&&!c.latest_audit.passed);umi_studio_web_platform_destroy(p);return 0;}
