/* Umicom Studio IDE | Workspace layout presets v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/workspace_layouts.h"
#include <stdio.h>
#include <string.h>
static UmiStatus add_window(UmiUiWorkspaceLayout *layout,const char *window_id,const char *title,const char *tool_id,const char *group_id,double x,double y,double width,double height,bool closable,int32_t z_order)
{
    UmiUiWorkspaceWindow window;
    (void)memset(&window,0,sizeof(window));
    (void)snprintf(window.window_id,sizeof(window.window_id),"%s",window_id);
    (void)snprintf(window.title,sizeof(window.title),"%s",title);
    (void)snprintf(window.tool_id,sizeof(window.tool_id),"%s",tool_id);
    if (group_id != NULL) (void)snprintf(window.group_id,sizeof(window.group_id),"%s",group_id);
    window.x = x; window.y = y; window.width = width; window.height = height;
    window.visible = true; window.closable = closable; window.z_order = z_order;
    return umi_ui_workspace_layout_add_window(layout,&window);
}
static UmiStatus develop_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-develop","Develop Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"project-explorer","Project Explorer","project-explorer","project-blue",0.0,0.0,0.18,0.76,false,1);
    if (status == UMI_STATUS_OK) status = add_window(layout,"editor-main","Editor","editor","project-blue",0.18,0.0,0.62,0.76,true,2);
    if (status == UMI_STATUS_OK) status = add_window(layout,"authorengine","AI and AuthorEngine","authorengine","project-blue",0.80,0.0,0.20,0.76,true,3);
    if (status == UMI_STATUS_OK) status = add_window(layout,"output","Output, Problems and Terminal","output","run-green",0.0,0.76,1.0,0.24,true,4);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}

/* Editor-only focus mode follows the distraction-free layouts offered by
 * professional IDEs while retaining the same underlying editor tool ID. */
static UmiStatus focus_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-focus","Focus Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"focus-editor","Editor","editor","project-blue",0.0,0.0,1.0,1.0,false,1);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}

/* Debug mode gives execution state the right and bottom tool regions without
 * allowing those regions to squeeze the central source editor excessively. */
static UmiStatus debug_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-debug","Debug Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"debug-explorer","Project Explorer","project-explorer","debug-orange",0.0,0.0,0.18,0.68,true,1);
    if (status == UMI_STATUS_OK) status = add_window(layout,"debug-editor","Editor","editor","debug-orange",0.18,0.0,0.58,0.68,false,2);
    if (status == UMI_STATUS_OK) status = add_window(layout,"debug-state","Variables and Call Stack","debug","debug-orange",0.76,0.0,0.24,0.68,true,3);
    if (status == UMI_STATUS_OK) status = add_window(layout,"debug-console","Debug Console, Problems and Terminal","terminal","debug-orange",0.0,0.68,1.0,0.32,true,4);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}
static UmiStatus operations_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-operations","Operations Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"metrics","Metrics","metrics","run-green",0.0,0.0,0.5,0.5,true,1);
    if (status == UMI_STATUS_OK) status = add_window(layout,"traces","Traces","traces","run-green",0.5,0.0,0.5,0.5,true,2);
    if (status == UMI_STATUS_OK) status = add_window(layout,"profiler","Profiler","profiler","run-green",0.0,0.5,0.5,0.5,true,3);
    if (status == UMI_STATUS_OK) status = add_window(layout,"health","Health and Resilience","health","run-green",0.5,0.5,0.5,0.5,true,4);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}

/* Trading mode adopts the TWS Mosaic principle of named, task-focused tools:
 * market selection, chart analysis, order entry, portfolio and live activity. */
static UmiStatus trading_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-trading","Trading Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"trading-watchlists","Watchlists","watchlist","trading-teal",0.0,0.0,0.18,0.72,true,1);
    if (status == UMI_STATUS_OK) status = add_window(layout,"trading-chart","Chart and Analytics","chart","trading-teal",0.18,0.0,0.52,0.72,false,2);
    if (status == UMI_STATUS_OK) status = add_window(layout,"trading-order-entry","Order Entry","order-entry","trading-teal",0.70,0.0,0.30,0.36,true,3);
    if (status == UMI_STATUS_OK) status = add_window(layout,"trading-portfolio","Portfolio and Risk","portfolio","trading-teal",0.70,0.36,0.30,0.36,true,4);
    if (status == UMI_STATUS_OK) status = add_window(layout,"trading-activity","Orders, Executions and Messages","trading-activity","trading-teal",0.0,0.72,1.0,0.28,true,5);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}
static UmiStatus compare_layout(UmiUiWorkspaceLayout *layout)
{
    UmiStatus status = umi_ui_workspace_layout_init(layout,"preset-compare","Compare Template");
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,false);
    if (status == UMI_STATUS_OK) status = add_window(layout,"compare-explorer","Folders","project-explorer","compare-orange",0.0,0.0,0.18,1.0,true,1);
    if (status == UMI_STATUS_OK) status = add_window(layout,"compare-original","Original","file-compare","compare-orange",0.18,0.0,0.41,0.80,true,2);
    if (status == UMI_STATUS_OK) status = add_window(layout,"compare-modified","Modified","file-compare","compare-orange",0.59,0.0,0.41,0.80,true,3);
    if (status == UMI_STATUS_OK) status = add_window(layout,"compare-results","Comparison Results","output","compare-orange",0.18,0.80,0.82,0.20,true,4);
    if (status == UMI_STATUS_OK) status = umi_ui_workspace_layout_set_locked(layout,true);
    return status;
}
static UmiStatus register_preset(UmiUiWorkspaceCustomisation *model,const char *preset_id,const char *category,const char *description,const UmiUiWorkspaceLayout *layout,const char *instance_id,const char *instance_name)
{
    UmiUiLayoutLibraryItem item;
    UmiUiWorkspaceLayout instance;
    UmiStatus status;
    (void)memset(&item,0,sizeof(item));
    (void)snprintf(item.preset_id,sizeof(item.preset_id),"%s",preset_id);
    (void)snprintf(item.category,sizeof(item.category),"%s",category);
    (void)snprintf(item.description,sizeof(item.description),"%s",description);
    item.layout = *layout;
    status = umi_ui_layout_library_add(&model->library,&item);
    if (status != UMI_STATUS_OK) return status;
    status = umi_ui_layout_library_instantiate(&model->library,preset_id,instance_id,instance_name,&instance);
    if (status != UMI_STATUS_OK) return status;
    return umi_ui_workspace_customisation_add_layout(model,&instance);
}
UmiStatus umi_studio_workspace_layouts_seed(UmiStudioProfessionalWorkspace *workspace)
{
    UmiUiWorkspaceCustomisation *model = umi_studio_professional_workspace_model(workspace);
    UmiUiWorkspaceLayout develop;
    UmiUiWorkspaceLayout focus;
    UmiUiWorkspaceLayout debug;
    UmiUiWorkspaceLayout operations;
    UmiUiWorkspaceLayout trading;
    UmiUiWorkspaceLayout compare;
    UmiStatus status;
    if (model == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = develop_layout(&develop); if (status != UMI_STATUS_OK) return status;
    status = focus_layout(&focus); if (status != UMI_STATUS_OK) return status;
    status = debug_layout(&debug); if (status != UMI_STATUS_OK) return status;
    status = operations_layout(&operations); if (status != UMI_STATUS_OK) return status;
    status = trading_layout(&trading); if (status != UMI_STATUS_OK) return status;
    status = compare_layout(&compare); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-develop","Development","Editor-first coding workspace with compact project, tool and output regions",&develop,"develop","Develop"); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-focus","Development","Distraction-free editor workspace for concentrated implementation and review",&focus,"focus","Focus"); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-debug","Development","Source, execution state and console workspace for debugging",&debug,"debug","Debug"); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-operations","Operations","Metrics, traces, profiler, health and resilience dashboard",&operations,"operations","Operations"); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-trading","Trading","TWS-inspired watchlist, analytics, order, portfolio and activity workspace",&trading,"trading","Trading"); if (status != UMI_STATUS_OK) return status;
    status = register_preset(model,"studio-compare","Comparison","Beyond Compare-inspired side-by-side file comparison workspace",&compare,"compare","Compare"); if (status != UMI_STATUS_OK) return status;
    return umi_ui_workspace_customisation_activate(model,"develop");
}
