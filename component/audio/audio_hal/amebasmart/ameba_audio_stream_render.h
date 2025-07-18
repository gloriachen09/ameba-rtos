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
#ifndef AMEBA_AUDIO_AUDIO_HAL_AMEBASMART_AMEBA_AUDIO_STREAM_RENDER_H
#define AMEBA_AUDIO_AUDIO_HAL_AMEBASMART_AMEBA_AUDIO_STREAM_RENDER_H

#include "audio_hw_compat.h"
#include "ameba_audio_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _RenderStream {
	Stream stream;
	uint64_t write_cnt;
	uint64_t total_written_from_tx_start;
	bool delay_start;
} RenderStream;

Stream *ameba_audio_stream_tx_init(uint32_t device, StreamConfig config);
void ameba_audio_stream_tx_start(Stream *stream, int32_t state);
int32_t ameba_audio_stream_tx_get_buffer_status(Stream *stream);
int32_t  ameba_audio_stream_tx_write(Stream *stream, const void *data, uint32_t bytes, bool block);
uint32_t ameba_audio_stream_tx_complete(void *data);
void ameba_audio_stream_tx_stop(Stream *stream, int32_t state);
void ameba_audio_stream_tx_standby(Stream *stream);
void ameba_audio_stream_tx_close(Stream *stream);
int32_t  ameba_audio_stream_tx_set_amp_state(bool state);
int32_t  ameba_audio_stream_tx_get_htimestamp(Stream *stream, uint32_t *avail, struct timespec *tstamp);
int32_t  ameba_audio_stream_tx_get_position(Stream *stream, uint64_t *rendered_frames, struct timespec *tstamp);
int32_t  ameba_audio_stream_tx_get_time(Stream *stream, int64_t *now_ns, int64_t *audio_ns);
int64_t ameba_audio_stream_tx_sport_rendered_frames(Stream *stream);
int64_t ameba_audio_stream_tx_get_frames_written(Stream *stream);
int64_t ameba_audio_stream_tx_get_trigger_time(Stream *stream);
void ameba_audio_stream_tx_set_delay_start(Stream *stream, bool should_delay);
void ameba_audio_stream_tx_buffer_flush(Stream *stream);

#ifdef __cplusplus
}
#endif

#endif
