/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/tests/test_plugins.c
 *
 * PURPOSE:
 *   Verify Studio manifest discovery, permission evaluation and catalogue
 *   reporting without loading executable plug-in code.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdio.h>

#include "umicom/studio/bootstrap.h"
#include "umicom/studio/plugins.h"

int main(void)
{
    const char *root = "studio-plugin-test";
    const char *path = "studio-plugin-test/example.umicom-plugin";
    FILE *stream;
    UmiStudioBootstrap *bootstrap = NULL;
    UmiStudioPluginReport report;

    (void)umi_fs_remove_tree(root);
    assert(umi_fs_make_directories(root) == UMI_STATUS_OK);
    stream = fopen(path, "wb");
    assert(stream != NULL);
    assert(fputs("id=org.umicom.studio.example\n"
                 "name=Studio Example\n"
                 "version=1.0.0\n"
                 "abi=2\n"
                 "library=example-plugin.so\n"
                 "permission=studio.commands\n"
                 "capability=studio.example\n",
                 stream) >= 0);
    assert(fclose(stream) == 0);

    assert(umi_studio_bootstrap_create(&bootstrap) == UMI_STATUS_OK);
    assert(umi_studio_plugins_discover(
               umi_studio_bootstrap_services(bootstrap),
               root,
               1,
               &report) == UMI_STATUS_OK);
    assert(report.registered == 1U);
    assert(report.manifests_loaded == 1U);
    umi_studio_bootstrap_destroy(bootstrap);
    assert(umi_fs_remove_tree(root) == UMI_STATUS_OK);
    return 0;
}
