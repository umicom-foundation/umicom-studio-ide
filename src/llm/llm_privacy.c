#include "umicom/privacy.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static int starts_with(const char *s, const char *p){ return s && p && strncmp(s,p,strlen(p))==0; }

bool umi_privacy_is_local_only(void){
  const char *m = getenv("UMI_PRIVACY_MODE");
  if(!m) return false;
  return (strcasecmp(m,"local-only")==0) || (strcasecmp(m,"local")==0) ||
         (strcmp(m,"1")==0) || (strcasecmp(m,"true")==0);
}

bool umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap){
  if(!umi_privacy_is_local_only()) return true;
  if(!url){ if(errbuf) snprintf(errbuf, errcap, "privacy: empty URL"); return false; }
  const char *p = strstr(url, "://");
  const char *host = p? p+3 : url;
  const char *slash = strchr(host, '/');
  const char *colon = strchr(host, ':');
  unsigned len = 0;
  if(slash && colon) len = (unsigned)((slash<colon?slash:colon) - host);
  else if(slash) len = (unsigned)(slash - host);
  else if(colon) len = (unsigned)(colon - host);
  else len = (unsigned)strlen(host);
  if(len==0){ if(errbuf) snprintf(errbuf, errcap, "privacy: malformed URL"); return false; }
  char h[128]; len = len<sizeof(h)-1?len:sizeof(h)-1; memcpy(h, host, len); h[len]=0;

  if(strcmp(h,"localhost")==0 || starts_with(h,"127.")){
    return true;
  }
  if(errbuf) snprintf(errbuf, errcap, "privacy: blocked remote host '%s' (UMI_PRIVACY_MODE=local-only)", h);
  return false;
}
