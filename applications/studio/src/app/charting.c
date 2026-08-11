/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/charting.c
 *
 * PURPOSE:
 *   Implement Studio chart use cases over the reusable Framework chart model.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * The Studio layer names the series and chooses indicators. It does not own the generic numerical implementation.
 */

#include "umicom/studio/charting.h"
UmiStatus umi_studio_charting_reset(UmiStudioWebPlatform *p,const char *title){UmiChartModel *m;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;m=umi_studio_web_platform_chart(p);return umi_chart_model_init(m,title!=NULL?title:"Umicom Studio");}
UmiStatus umi_studio_charting_add_close(UmiStudioWebPlatform *p,int64_t time_ms,double close){UmiChartModel *m;UmiChartSeries *s;UmiChartSeries created;UmiStatus st;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;m=umi_studio_web_platform_chart(p);s=umi_chart_model_find_series(m,"close");if(s==NULL){st=umi_chart_series_init(&created,"close",UMI_CHART_LINE);if(st!=UMI_STATUS_OK)return st;st=umi_chart_model_add_series(m,&created);if(st!=UMI_STATUS_OK)return st;s=umi_chart_model_find_series(m,"close");}return umi_chart_series_add(s,(UmiChartPoint){time_ms,close});}
UmiStatus umi_studio_charting_add_sma(UmiStudioWebPlatform *p,size_t period){UmiChartModel *m;UmiChartSeries out;UmiChartSeries *close;UmiStatus st;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;m=umi_studio_web_platform_chart(p);close=umi_chart_model_find_series(m,"close");if(close==NULL)return UMI_STATUS_NOT_FOUND;st=umi_chart_indicator_sma(close,period,&out);if(st!=UMI_STATUS_OK)return st;return umi_chart_model_add_series(m,&out);}
