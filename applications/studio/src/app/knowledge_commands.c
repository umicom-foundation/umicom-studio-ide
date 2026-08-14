/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/knowledge_commands.c
 * PURPOSE: Implement palette, console and frontend-neutral knowledge commands.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/knowledge_commands.h"

#include <stdio.h>
#include <string.h>

#include "umicom/studio/commands.h"
#include "umicom/studio/knowledge_centre.h"

static size_t split_fields(char *text, char **fields, size_t capacity)
{
    size_t count = 0U;
    char *cursor = text;
    if (text == NULL || fields == NULL || capacity == 0U) return 0U;
    fields[count++] = cursor;
    while (*cursor != '\0' && count < capacity) {
        if (*cursor == '|') {
            *cursor = '\0';
            fields[count++] = cursor + 1;
        }
        ++cursor;
    }
    return count;
}

static UmiStatus collection_create_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    char buffer[UMI_KNOWLEDGE_TEXT_CAPACITY];
    char *fields[3];
    size_t count;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0' ||
        snprintf(buffer, sizeof(buffer), "%s", argument) < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = split_fields(buffer, fields, 3U);
    if (count < 2U || fields[0][0] == '\0' || fields[1][0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_knowledge_centre_add_collection(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        fields[0], fields[1], count > 2U ? fields[2] : "");
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       status == UMI_STATUS_OK
                           ? "Knowledge collection %s is ready"
                           : "Knowledge collection: %s",
                       status == UMI_STATUS_OK ? fields[0]
                                               : umi_status_text(status));
    }
    return status;
}

static UmiStatus ingest_handler(void *user_data, const char *argument,
                                char *out_message, size_t message_capacity)
{
    char buffer[UMI_KNOWLEDGE_TEXT_CAPACITY];
    char *fields[5];
    UmiStudioKnowledgeSourceInput input = {0};
    UmiKnowledgeIngestionReport report;
    size_t count;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0' ||
        snprintf(buffer, sizeof(buffer), "%s", argument) < 0) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    count = split_fields(buffer, fields, 5U);
    if (count != 5U || fields[4][0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)snprintf(input.collection_id, sizeof(input.collection_id),
                   "%s", fields[0]);
    (void)snprintf(input.source_id, sizeof(input.source_id), "%s", fields[1]);
    (void)snprintf(input.title, sizeof(input.title), "%s", fields[2]);
    (void)snprintf(input.uri, sizeof(input.uri), "%s", fields[3]);
    (void)snprintf(input.language, sizeof(input.language), "%s", "und");
    (void)snprintf(input.media_type, sizeof(input.media_type), "%s",
                   "text/plain");
    input.kind = UMI_KNOWLEDGE_SOURCE_DOCUMENT;
    input.classification = UMI_AI_DATA_INTERNAL;
    status = umi_studio_knowledge_centre_ingest_text(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        &input, fields[4], &report);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK) {
            (void)snprintf(out_message, message_capacity,
                           "Knowledge source %s: %zu chunk(s), revision %llu",
                           fields[1], report.chunks_created,
                           (unsigned long long)report.index_revision);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Knowledge ingest: %s", umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus refresh_handler(void *user_data, const char *argument,
                                 char *out_message, size_t message_capacity)
{
    UmiKnowledgeServiceSnapshot snapshot;
    UmiKnowledgeService *service = umi_studio_ai_platform_knowledge(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data));
    UmiStatus status;
    (void)argument;
    status = umi_knowledge_service_snapshot(service, &snapshot);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Knowledge refresh queue: %zu source(s), %zu chunk(s)",
                       snapshot.source_count, snapshot.chunk_count);
    }
    return status;
}

static UmiStatus search_handler(void *user_data, const char *argument,
                                char *out_message, size_t message_capacity)
{
    UmiKnowledgeMatch matches[UMI_KNOWLEDGE_QUERY_RESULT_MAX];
    size_t count = 0U;
    UmiStatus status;
    if (argument == NULL || argument[0] == '\0') {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_studio_knowledge_centre_search(
        umi_studio_services_ai_platform((UmiStudioServices *)user_data),
        argument, NULL, matches, UMI_KNOWLEDGE_QUERY_RESULT_MAX, &count);
    if (out_message != NULL && message_capacity > 0U) {
        if (status == UMI_STATUS_OK && count > 0U) {
            (void)snprintf(out_message, message_capacity,
                           "Knowledge search: %zu result(s); best %.4f from %s",
                           count, matches[0].score, matches[0].citation.title);
        } else {
            (void)snprintf(out_message, message_capacity,
                           "Knowledge search: %s",
                           status == UMI_STATUS_OK ? "no results"
                                                   : umi_status_text(status));
        }
    }
    return status;
}

static UmiStatus archive_handler(void *user_data, const char *argument,
                                 char *out_message, size_t message_capacity,
                                 int load)
{
    UmiStudioAiPlatform *platform = umi_studio_services_ai_platform(
        (UmiStudioServices *)user_data);
    UmiStatus status = load
        ? umi_studio_knowledge_centre_load(platform, argument)
        : umi_studio_knowledge_centre_save(platform, argument);
    if (out_message != NULL && message_capacity > 0U) {
        (void)snprintf(out_message, message_capacity,
                       "Knowledge archive %s: %s", load ? "load" : "save",
                       umi_status_text(status));
    }
    return status;
}

static UmiStatus archive_save_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    return archive_handler(user_data, argument, out_message,
                           message_capacity, 0);
}

static UmiStatus archive_load_handler(
    void *user_data, const char *argument,
    char *out_message, size_t message_capacity)
{
    return archive_handler(user_data, argument, out_message,
                           message_capacity, 1);
}

static UmiStatus register_command(
    UmiCommandRegistry *registry, UmiStudioServices *services,
    const char *id, const char *title, const char *description,
    uint32_t flags, UmiRegisteredCommandHandler handler)
{
    UmiCommandDescriptor descriptor = {0};
    descriptor.structure_size = (uint32_t)sizeof(descriptor);
    descriptor.command_id = id;
    descriptor.title = title;
    descriptor.category = "Knowledge Centre";
    descriptor.description = description;
    descriptor.required_permission = "studio.knowledge.use";
    descriptor.flags = flags;
    descriptor.handler = handler;
    descriptor.user_data = services;
    return umi_command_registry_register(registry, &descriptor);
}

UmiStatus umi_studio_knowledge_commands_register(
    UmiCommandRegistry *registry,
    UmiStudioServices *services)
{
    UmiStatus status;
    if (registry == NULL || services == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_COLLECTION_CREATE,
        "Create Knowledge Collection",
        "Create collection using id|name|description.",
        UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
        collection_create_handler);
    if (status == UMI_STATUS_OK) status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_INGEST,
        "Ingest Knowledge Source",
        "Ingest collection|source|title|uri|text with incremental refresh.",
        UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED, ingest_handler);
    if (status == UMI_STATUS_OK) status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_REFRESH,
        "Refresh Knowledge Sources", "Inspect the incremental refresh queue.",
        UMI_COMMAND_NONE, refresh_handler);
    if (status == UMI_STATUS_OK) status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_SEARCH,
        "Search Knowledge", "Search indexed evidence with citations.",
        UMI_COMMAND_NONE, search_handler);
    if (status == UMI_STATUS_OK) status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_SAVE,
        "Save Knowledge Archive", "Save the offline local index archive.",
        UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
        archive_save_handler);
    if (status == UMI_STATUS_OK) status = register_command(
        registry, services, UMI_STUDIO_COMMAND_KNOWLEDGE_ARCHIVE_LOAD,
        "Load Knowledge Archive", "Restore an offline local index archive.",
        UMI_COMMAND_MUTATES_STATE | UMI_COMMAND_AUDITED,
        archive_load_handler);
    return status;
}
