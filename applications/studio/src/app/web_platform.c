/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/web_platform.c
 *
 * PURPOSE:
 *   Compose the reusable Framework web, browser, charting and frontend services for Studio.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Studio owns how these services are combined, but it does not reimplement their generic algorithms.
 */

#include "umicom/studio/web_platform.h"
#include "umicom/studio/web.h"
#include "umicom/studio/charting.h"
#include "umicom/studio/frontend.h"
#include <stdlib.h>
#include <string.h>
struct UmiStudioWebPlatform{UmiWebService *web;UmiBrowser browser;UmiChartModel chart;UmiFrontendPlan frontend;UmiFrontendStudio frontend_studio;};
static void browser_sink(const UmiBrowserEvent *event,void *user_data){(void)event;(void)user_data;}
UmiStatus umi_studio_web_platform_create(UmiStudioWebPlatform **out){UmiStudioWebPlatform *p;UmiStatus s;if(out==NULL)return UMI_STATUS_INVALID_ARGUMENT;*out=NULL;p=(UmiStudioWebPlatform*)calloc(1U,sizeof(*p));if(p==NULL)return UMI_STATUS_OUT_OF_MEMORY;s=umi_web_service_create(&p->web);if(s==UMI_STATUS_OK)s=umi_browser_headless_create(browser_sink,p,&p->browser);if(s==UMI_STATUS_OK)s=umi_chart_model_init(&p->chart,"Umicom Studio");if(s==UMI_STATUS_OK)s=umi_frontend_plan_init(&p->frontend,"org.umicom.studio",UMI_FRONTEND_KIND_WEB);if(s==UMI_STATUS_OK)s=umi_frontend_studio_init(&p->frontend_studio,NULL);if(s==UMI_STATUS_OK)s=umi_studio_web_register_defaults(p);if(s!=UMI_STATUS_OK){umi_studio_web_platform_destroy(p);return s;}*out=p;return UMI_STATUS_OK;}
void umi_studio_web_platform_destroy(UmiStudioWebPlatform *p){if(p==NULL)return;if(p->browser.instance!=NULL&&p->browser.destroy!=NULL)p->browser.destroy(p->browser.instance);umi_web_service_destroy(p->web);free(p);}
UmiStatus umi_studio_web_platform_snapshot(const UmiStudioWebPlatform *p,UmiStudioWebPlatformSnapshot *out){if(p==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(out,0,sizeof(*out));out->routes=umi_web_router_count(umi_web_service_router(p->web));out->endpoints=umi_web_endpoint_count(umi_web_service_endpoints(p->web));out->sessions=umi_web_session_count(umi_web_service_sessions(p->web));out->requests=umi_web_service_metrics(p->web)->requests;out->chart_series=p->chart.series_count;out->frontend_pages=p->frontend.page_count;if(p->browser.current_url!=NULL){const char *u=p->browser.current_url(p->browser.instance);if(u!=NULL)(void)umi_web_copy_text(out->browser_url,sizeof(out->browser_url),u);}return UMI_STATUS_OK;}
UmiWebService *umi_studio_web_platform_web(UmiStudioWebPlatform *p){return p!=NULL?p->web:NULL;}
UmiBrowser *umi_studio_web_platform_browser(UmiStudioWebPlatform *p){return p!=NULL?&p->browser:NULL;}
UmiChartModel *umi_studio_web_platform_chart(UmiStudioWebPlatform *p){return p!=NULL?&p->chart:NULL;}
UmiFrontendPlan *umi_studio_web_platform_frontend(UmiStudioWebPlatform *p){return p!=NULL?&p->frontend:NULL;}
UmiFrontendStudio *umi_studio_web_platform_frontend_studio(UmiStudioWebPlatform *p){return p!=NULL?&p->frontend_studio:NULL;}
