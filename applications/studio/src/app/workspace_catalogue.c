/* Umicom Studio IDE | Workspace catalogue v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_catalogue.h"
static const UmiUiWindowDescriptor WINDOWS[] = {
    {"project-explorer","Project Explorer","Browse workspace projects and files","folder",UMI_UI_WINDOW_CATEGORY_NAVIGATION,false,0.20,0.75},
    {"editor","Editor","Tabbed source editor","text-editor",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.55,0.75},
    {"file-compare","File Compare","Side-by-side file and directory comparison","compare",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.41,0.80},
    {"terminal","Terminal","Shell, task and process terminal","terminal",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,true,0.50,0.25},
    {"build","Build","Configure, compile, package and deploy","build",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.50,0.25},
    {"tests","Test Explorer","Discover and execute tests","test",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.35,0.50},
    {"debug","Debug","Breakpoints, stack, variables and watches","debug",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.40,0.50},
    {"source-control","Source Control","Changes, history and branches","source-control",UMI_UI_WINDOW_CATEGORY_DEVELOPMENT,false,0.30,0.60},
    {"output","Output","Build, task, extension and run output","output",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,1.00,0.25},
    {"metrics","Metrics","Runtime counters and gauges","metrics",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
    {"traces","Traces","Correlated operation traces","traces",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
    {"profiler","Profiler","CPU, memory, process and task profiles","speedometer",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
    {"health","Health and Resilience","Readiness, recovery, retry and circuit evidence","health",UMI_UI_WINDOW_CATEGORY_OPERATIONS,false,0.50,0.50},
    {"database-explorer","Database Explorer","Connections, queries, data and migrations","database",UMI_UI_WINDOW_CATEGORY_DATA,false,0.40,0.70},
    {"authorengine","AI and AuthorEngine","Context-aware assistant, agents and generation","sparkles",UMI_UI_WINDOW_CATEGORY_AI,false,0.25,0.75},
    {"chart","Chart","Reusable financial and operational chart","chart",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.50,0.50},
    {"watchlist","Watchlist","Linked instruments and alerts","watchlist",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.30,0.60},
    {"order-entry","Order Entry","Linked trading order ticket","order",UMI_UI_WINDOW_CATEGORY_TRADING,true,0.30,0.50}
};
UmiStatus umi_studio_workspace_catalogue_seed(UmiStudioProfessionalWorkspace *workspace)
{
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    size_t index;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < sizeof(WINDOWS) / sizeof(WINDOWS[0]); ++index) {
        UmiStatus status = umi_ui_window_catalogue_register(&model->windows,&WINDOWS[index]);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}
