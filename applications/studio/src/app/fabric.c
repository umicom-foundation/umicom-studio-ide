/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/fabric.c
 *
 * PURPOSE:
 *   Register and validate Studio message schemas and topics used by commands, events, history and workflows.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/fabric.h"

#include <string.h>

#include "umicom/studio/messages.h"

static UmiStatus add_schema(UmiSchemaRegistry *registry,
                            const char *schema_id,
                            UmiMessageKind kind,
                            size_t maximum_payload)
{
    UmiSchemaDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.schema_id = schema_id;
    descriptor.version = 1U;
    descriptor.kind = kind;
    descriptor.maximum_payload_size = maximum_payload;
    descriptor.compatibility = UMI_SCHEMA_EXACT;
    return umi_schema_registry_register(registry, &descriptor);
}

static UmiStatus add_topic(UmiTopicRegistry *registry,
                           const char *topic,
                           const char *schema_id,
                           int durable)
{
    UmiTopicDescriptor descriptor;
    (void)memset(&descriptor, 0, sizeof(descriptor));
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.topic = topic;
    descriptor.schema_id = schema_id;
    descriptor.partitions = 1U;
    descriptor.durable = durable;
    return umi_topic_registry_register(registry, &descriptor);
}

UmiStatus umi_studio_fabric_register_defaults(UmiStudioServices *services)
{
    UmiSchemaRegistry *schemas;
    UmiTopicRegistry *topics;
    UmiStatus status;
    if (services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    schemas = umi_studio_services_schema_registry(services);
    topics = umi_studio_services_topics(services);
#define ADD_SCHEMA(id, kind, limit)                                          \
    do {                                                                     \
        status = add_schema(schemas, (id), (kind), (limit));                 \
        if (status != UMI_STATUS_OK) return status;                          \
    } while (0)
    ADD_SCHEMA(UMI_STUDIO_SCHEMA_EVENT, UMI_MESSAGE_EVENT, 65536U);
    ADD_SCHEMA(UMI_STUDIO_SCHEMA_COMMAND, UMI_MESSAGE_COMMAND, 65536U);
    ADD_SCHEMA(UMI_STUDIO_SCHEMA_HISTORY, UMI_MESSAGE_EVENT, 65536U);
    ADD_SCHEMA(UMI_STUDIO_SCHEMA_WORKFLOW, UMI_MESSAGE_WORKFLOW, 65536U);
#undef ADD_SCHEMA
#define ADD_TOPIC(name, schema, durable)                                     \
    do {                                                                     \
        status = add_topic(topics, (name), (schema), (durable));             \
        if (status != UMI_STATUS_OK) return status;                          \
    } while (0)
    ADD_TOPIC("studio.events", UMI_STUDIO_SCHEMA_EVENT, 1);
    ADD_TOPIC("studio.commands", UMI_STUDIO_SCHEMA_COMMAND, 1);
    ADD_TOPIC("studio.history", UMI_STUDIO_SCHEMA_HISTORY, 1);
    ADD_TOPIC("studio.workflows", UMI_STUDIO_SCHEMA_WORKFLOW, 1);
#undef ADD_TOPIC
    return UMI_STATUS_OK;
}

UmiStatus umi_studio_fabric_validate(UmiStudioServices *services)
{
    if (services == NULL ||
        umi_schema_registry_count(umi_studio_services_schema_registry(services)) < 4U ||
        umi_topic_registry_count(umi_studio_services_topics(services)) < 4U ||
        umi_studio_services_dispatcher(services) == NULL ||
        umi_studio_services_journal(services) == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    return UMI_STATUS_OK;
}
