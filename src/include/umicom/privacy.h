#ifndef UMICOM_PRIVACY_H
#define UMICOM_PRIVACY_H
/* Created by: Umicom Foundation | Author: Sammy Hegab | Date: 2025-10-02 | MIT */
#include <stdbool.h>

bool umi_privacy_is_local_only(void);
bool umi_privacy_allow_url(const char *url, char *errbuf, unsigned errcap);

#endif
