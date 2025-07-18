/*
 * Copyright (c) 2022 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBAGREEN2_AMEBA_AUDIO_STREAM_CAPTURE_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBAGREEN2_AMEBA_AUDIO_STREAM_CAPTURE_H

#include "time.h"

#include "audio_hw_compat.h"
#include "ameba_audio_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CaptureStream {
	Stream stream;
} CaptureStream;

Stream *ameba_audio_stream_rx_init(uint32_t device, StreamConfig config);
uint32_t ameba_audio_stream_rx_complete(void *data);
void ameba_audio_stream_rx_start(Stream *stream);
int64_t ameba_audio_stream_rx_get_trigger_time(Stream *stream);
void ameba_audio_stream_rx_stop(Stream *stream);
int32_t  ameba_audio_stream_rx_read(Stream *stream, void *data, uint32_t bytes, uint32_t time_out_ms);
void ameba_audio_stream_rx_close(Stream *stream);
int32_t  ameba_audio_stream_rx_get_position(Stream *stream, uint64_t *captured_frames, struct timespec *tstamp);
int32_t  ameba_audio_stream_rx_get_time(Stream *stream, int64_t *now_ns, int64_t *audio_ns);
void ameba_audio_stream_rx_sync_start(Stream *stream, uint32_t sport_index, uint32_t sport_index_extra);
void ameba_audio_stream_rx_sync_stop(Stream *stream, uint32_t sport_index, uint32_t sport_index_extra);
void ameba_audio_stream_rx_mask_gdma_irq(Stream *stream);
void ameba_audio_stream_rx_unmask_gdma_irq(Stream *stream);

#ifdef __cplusplus
}
#endif

#endif
