/* Umicom Studio IDE | Polyglot interoperability view v2 | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/studio/compiler_interop.h"
#include <stdio.h>
#include <string.h>
static const UmiInteropNode *find_node(const UmiInteropGraph *graph,const char *unit_id) { size_t index; for (index = 0U; index < graph->node_count; ++index) if (strcmp(graph->nodes[index].unit_id,unit_id) == 0) return &graph->nodes[index]; return NULL; }
UmiStatus umi_studio_compiler_interop_summarise(const UmiInteropGraph *graph,UmiStudioCompilerInteropSummary *out_summary)
{
    size_t index; char reason[UMI_COMPILER_TEXT_CAPACITY]; UmiStatus graph_status;
    if (graph == NULL || out_summary == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_summary,0,sizeof(*out_summary));
    out_summary->units = graph->node_count;
    out_summary->boundaries = graph->edge_count;
    for (index = 0U; index < graph->edge_count; ++index) { const UmiInteropNode *producer = find_node(graph,graph->edges[index].producer_id); const UmiInteropNode *consumer = find_node(graph,graph->edges[index].consumer_id); if (graph->edges[index].required) out_summary->required_boundaries += 1U; if (producer != NULL && consumer != NULL && umi_compiler_abi_compatible(&producer->abi,&consumer->abi)) out_summary->stable_boundaries += 1U; else out_summary->incompatible_boundaries += 1U; }
    graph_status = umi_interop_graph_validate(graph,reason,sizeof(reason)); out_summary->buildable = graph_status == UMI_STATUS_OK && out_summary->incompatible_boundaries == 0U; (void)snprintf(out_summary->headline,sizeof(out_summary->headline),"%zu unit(s), %zu ABI boundary/boundaries, %zu incompatible",out_summary->units,out_summary->boundaries,out_summary->incompatible_boundaries); return UMI_STATUS_OK;
}
