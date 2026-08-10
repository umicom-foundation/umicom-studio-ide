/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/designer_catalogue.c
 *
 * PURPOSE:
 *   Format Framework component metadata into a concise Studio catalogue used by tools and future GTK palette views.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * This Studio adapter keeps product composition thin: it delegates reusable
 * behaviour to Framework services and adds only Studio-specific coordination.
 */

#include "umicom/studio/designer_catalogue.h"
#include <stdio.h>
#include <string.h>
UmiStatus umi_studio_designer_catalogue_report(UmiStudioDeclarative *service,const char *filter,char *out,size_t cap)
{
    UmiDesignerPalette palette;
    size_t i,used=0U;
    UmiStatus s;
    if(service==NULL||out==NULL||cap==0U)return UMI_STATUS_INVALID_ARGUMENT;
    s=umi_designer_palette_build(umi_studio_declarative_components(service),filter,&palette);
    if(s!=UMI_STATUS_OK)return s;
    out[0]='\0';
    for(i=0U;i<palette.count;++i){
        char line[384];
        int n=snprintf(line,sizeof(line),"%-14s %-24s [%s]\n",palette.items[i].component_type,palette.items[i].display_name,palette.items[i].category);
        if(n<0||used+(size_t)n+1U>cap){umi_designer_palette_dispose(&palette);return UMI_STATUS_CAPACITY_EXCEEDED;}
        (void)memcpy(out+used,line,(size_t)n);used+=(size_t)n;out[used]='\0';
    }
    umi_designer_palette_dispose(&palette);
    return UMI_STATUS_OK;
}
