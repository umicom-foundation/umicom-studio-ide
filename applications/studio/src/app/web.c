/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/web.c
 *
 * PURPOSE:
 *   Register Studio REST endpoints and process requests through the Framework web runtime.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

/* BEGINNER NOTE:
 * Handlers below translate Studio state into web responses. Parsing, routing and response formatting stay in Framework.
 */

#include "umicom/studio/web.h"
#include "umicom/studio/version.h"
#include <string.h>
static UmiStatus health(const UmiWebRequest *req,UmiWebResponse *res,void *data){char json[256];(void)req;(void)data;if(umi_web_json_object1("status","ready",json,sizeof(json))!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;return umi_web_rest_json(res,200,json);}
static UmiStatus version(const UmiWebRequest *req,UmiWebResponse *res,void *data){char json[256];(void)req;(void)data;if(umi_web_json_object1("studio",umi_studio_version_string(),json,sizeof(json))!=UMI_STATUS_OK)return UMI_STATUS_INTERNAL_ERROR;return umi_web_rest_json(res,200,json);}
UmiStatus umi_studio_web_register_defaults(UmiStudioWebPlatform *p){UmiWebService *s;UmiWebEndpoint e;UmiStatus st;if(p==NULL)return UMI_STATUS_INVALID_ARGUMENT;s=umi_studio_web_platform_web(p);st=umi_web_rest_register(umi_web_service_router(s),UMI_HTTP_METHOD_GET,"/api/health",health,p);if(st==UMI_STATUS_OK)st=umi_web_rest_register(umi_web_service_router(s),UMI_HTTP_METHOD_GET,"/api/version",version,p);if(st!=UMI_STATUS_OK)return st;(void)memset(&e,0,sizeof(e));(void)umi_web_copy_text(e.id,sizeof(e.id),"studio.health");e.method=UMI_HTTP_METHOD_GET;(void)umi_web_copy_text(e.path,sizeof(e.path),"/api/health");st=umi_web_endpoint_register(umi_web_service_endpoints(s),&e);if(st!=UMI_STATUS_OK)return st;(void)memset(&e,0,sizeof(e));(void)umi_web_copy_text(e.id,sizeof(e.id),"studio.version");e.method=UMI_HTTP_METHOD_GET;(void)umi_web_copy_text(e.path,sizeof(e.path),"/api/version");return umi_web_endpoint_register(umi_web_service_endpoints(s),&e);}
UmiStatus umi_studio_web_process(UmiStudioWebPlatform *p,const char *request,char *out,size_t cap){size_t n=0U;if(p==NULL||request==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;return umi_web_runtime_process(umi_studio_web_platform_web(p),request,strlen(request),out,cap,&n);}
