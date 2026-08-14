/* Umicom Studio IDE | Native Quality Centre command | Sammy Hegab | Umicom Foundation | MIT */
#include <stdio.h>
#include "umicom/studio/bootstrap.h"
#include "umicom/studio/quality_commands.h"
int main(void)
{
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioQualityCentre *centre = NULL;
    UmiCodeGuardQualityEvaluation evaluation;
    UmiStatus status = umi_studio_bootstrap_create(&bootstrap);
    if (status == UMI_STATUS_OK) status = umi_studio_quality_centre_create(umi_studio_bootstrap_services(bootstrap),&centre);
    if (status == UMI_STATUS_OK) status = umi_studio_quality_execute(centre,UMI_STUDIO_QUALITY_COMMAND_SEED_EVIDENCE,NULL);
    if (status == UMI_STATUS_OK) status = umi_studio_quality_execute(centre,UMI_STUDIO_QUALITY_COMMAND_EVALUATE,&evaluation);
    if (status == UMI_STATUS_OK) (void)printf("Quality Centre: %s\n",evaluation.summary);
    umi_studio_quality_centre_destroy(centre);
    umi_studio_bootstrap_destroy(bootstrap);
    return status == UMI_STATUS_OK ? 0 : 1;
}
