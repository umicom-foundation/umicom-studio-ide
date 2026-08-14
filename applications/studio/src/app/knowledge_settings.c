/*-----------------------------------------------------------------------------
 * Umicom Studio IDE
 * File: applications/studio/src/app/knowledge_settings.c
 * PURPOSE: Read validated Knowledge Centre settings into AI platform config.
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/studio/knowledge_settings.h"

#include <stdint.h>

UmiStatus umi_studio_knowledge_settings_apply(
    const UmiSettings *settings,
    UmiStudioAiPlatformConfig *config)
{
    int64_t sources = 0;
    int64_t vectors = 0;
    int64_t chunk_bytes = 0;
    int64_t overlap_bytes = 0;
    int64_t results = 0;
    int offline_only = 0;
    UmiStatus status;
    if (settings == NULL || config == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_settings_get_text(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_ARCHIVE_PATH,
        config->knowledge_archive_path,
        sizeof(config->knowledge_archive_path));
    if (status == UMI_STATUS_OK) status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_SOURCE_CAPACITY, &sources);
    if (status == UMI_STATUS_OK) status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_VECTOR_CAPACITY, &vectors);
    if (status == UMI_STATUS_OK) status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_CHUNK_BYTES, &chunk_bytes);
    if (status == UMI_STATUS_OK) status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_OVERLAP_BYTES, &overlap_bytes);
    if (status == UMI_STATUS_OK) status = umi_settings_get_integer(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_RESULT_LIMIT, &results);
    if (status == UMI_STATUS_OK) status = umi_settings_get_boolean(
        settings, UMI_STUDIO_SETTING_KNOWLEDGE_OFFLINE_ONLY, &offline_only);
    if (status != UMI_STATUS_OK) return status;
    if (sources <= 0 || vectors <= 0 || chunk_bytes <= 0 ||
        overlap_bytes < 0 || overlap_bytes >= chunk_bytes || results <= 0) {
        return UMI_STATUS_INVALID_STATE;
    }
    config->knowledge_source_capacity = (size_t)sources;
    config->knowledge_vector_capacity = (size_t)vectors;
    config->knowledge_chunk_bytes = (size_t)chunk_bytes;
    config->knowledge_overlap_bytes = (size_t)overlap_bytes;
    config->knowledge_result_limit = (size_t)results;
    config->knowledge_offline_only = offline_only;
    return UMI_STATUS_OK;
}
