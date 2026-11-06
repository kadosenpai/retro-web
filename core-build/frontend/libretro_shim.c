/*
 * libretro_shim.c
 *
 * Frontend libretro tối giản, dùng chung cho MỌI nhân libretro biên dịch sang Wasm.
 * Nhân (vd. mgba_libretro.bc) được liên kết tĩnh; shim nhận callback video/audio/input/
 * environment của libretro và xuất một API C nhỏ cho JavaScript gọi.
 *
 * Quy ước bộ nhớ: mọi con trỏ trả về trỏ vào heap Wasm. Heap có thể đổi chỗ khi tự
 * giãn nở, nên phía JS phải lấy lại view (Module.HEAPU8...) SAU MỖI lần gọi shim_run.
 */
#include <stdarg.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "libretro.h"

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#define EXPORT EMSCRIPTEN_KEEPALIVE
#else
#define EXPORT
#endif

#define AUDIO_CAPACITY_FRAMES 16384

/* ---------- Trạng thái ---------- */

static enum retro_pixel_format g_pixelFormat = RETRO_PIXEL_FORMAT_0RGB1555;

static uint8_t* g_frame = NULL;
static size_t g_frameCap = 0;
static unsigned g_frameW = 0, g_frameH = 0, g_framePitch = 0;
static int g_frameUpdated = 0;

static int16_t g_audio[AUDIO_CAPACITY_FRAMES * 2];
static size_t g_audioFrames = 0;

static uint32_t g_inputMask = 0;
static int g_gameLoaded = 0;

/* ---------- Callback libretro ---------- */

static unsigned bytesPerPixel(void) {
	return g_pixelFormat == RETRO_PIXEL_FORMAT_XRGB8888 ? 4 : 2;
}

static void log_cb(enum retro_log_level level, const char* fmt, ...) {
	if (level < RETRO_LOG_WARN) {
		return;
	}
	va_list args;
	va_start(args, fmt);
	fputs("[core] ", stderr);
	vfprintf(stderr, fmt, args);
	va_end(args);
}

static bool environment_cb(unsigned cmd, void* data) {
	switch (cmd) {
	case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
		g_pixelFormat = *(const enum retro_pixel_format*) data;
		return true;
	case RETRO_ENVIRONMENT_GET_CAN_DUPE:
		*(bool*) data = true;
		return true;
	case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
	case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
	case RETRO_ENVIRONMENT_GET_CORE_ASSETS_DIRECTORY:
		*(const char**) data = "/";
		return true;
	case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
		((struct retro_log_callback*) data)->log = log_cb;
		return true;
	case RETRO_ENVIRONMENT_GET_INPUT_BITMASKS:
		return true;
	case RETRO_ENVIRONMENT_GET_VARIABLE:
		((struct retro_variable*) data)->value = NULL; /* dùng giá trị mặc định của nhân */
		return false;
	case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
		*(bool*) data = false;
		return true;
	case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
	case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
	case RETRO_ENVIRONMENT_SET_GEOMETRY:
		return true;
	default:
		return false;
	}
}

static void video_refresh_cb(const void* data, unsigned width, unsigned height, size_t pitch) {
	if (!data) {
		return; /* khung trùng lặp: giữ nguyên khung cũ */
	}
	unsigned bpp = bytesPerPixel();
	size_t rowBytes = (size_t) width * bpp;
	size_t need = rowBytes * height;
	if (need > g_frameCap) {
		uint8_t* grown = realloc(g_frame, need);
		if (!grown) {
			return;
		}
		g_frame = grown;
		g_frameCap = need;
	}
	const uint8_t* src = data;
	for (unsigned y = 0; y < height; y++) {
		memcpy(g_frame + y * rowBytes, src + y * pitch, rowBytes);
	}
	g_frameW = width;
	g_frameH = height;
	g_framePitch = (unsigned) rowBytes;
	g_frameUpdated = 1;
}

static size_t audio_batch_cb(const int16_t* data, size_t frames) {
	size_t room = AUDIO_CAPACITY_FRAMES - g_audioFrames;
	size_t take = frames < room ? frames : room;
	memcpy(g_audio + g_audioFrames * 2, data, take * 2 * sizeof(int16_t));
	g_audioFrames += take;
	return frames;
}

static void audio_sample_cb(int16_t left, int16_t right) {
	int16_t pair[2] = {left, right};
	audio_batch_cb(pair, 1);
}

static void input_poll_cb(void) {}

static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id) {
	(void) index;
	if (port != 0 || device != RETRO_DEVICE_JOYPAD) {
		return 0;
	}
	if (id == RETRO_DEVICE_ID_JOYPAD_MASK) {
		return (int16_t) (g_inputMask & 0xFFFF);
	}
	return id < 16 ? (int16_t) ((g_inputMask >> id) & 1) : 0;
}

/* ---------- API xuất cho JavaScript ---------- */

EXPORT int shim_init(void) {
	if (retro_api_version() != RETRO_API_VERSION) {
		return 0;
	}
	retro_set_environment(environment_cb);
	retro_set_video_refresh(video_refresh_cb);
	retro_set_audio_sample(audio_sample_cb);
	retro_set_audio_sample_batch(audio_batch_cb);
	retro_set_input_poll(input_poll_cb);
	retro_set_input_state(input_state_cb);
	retro_init();
	return 1;
}

EXPORT int shim_load_game(const uint8_t* data, size_t size) {
	struct retro_game_info info = {"game.rom", data, size, NULL};
	if (!retro_load_game(&info)) {
		return 0;
	}
	retro_set_controller_port_device(0, RETRO_DEVICE_JOYPAD);
	g_gameLoaded = 1;
	g_frameUpdated = 0;
	g_audioFrames = 0;
	return 1;
}

EXPORT void shim_unload_game(void) {
	if (g_gameLoaded) {
		retro_unload_game();
		g_gameLoaded = 0;
	}
}

EXPORT void shim_reset(void) {
	if (g_gameLoaded) {
		retro_reset();
	}
}

EXPORT void shim_set_input(uint32_t mask) {
	g_inputMask = mask;
}

/* Chạy đúng 1 khung hình. Âm thanh của khung này nằm ở shim_audio_ptr/frames. */
EXPORT void shim_run(void) {
	if (!g_gameLoaded) {
		return;
	}
	g_audioFrames = 0;
	g_frameUpdated = 0;
	retro_run();
}

/* Video: ảnh đã gói chặt (pitch = width * bytes/pixel). */
EXPORT const uint8_t* shim_frame_ptr(void) { return g_frame; }
EXPORT unsigned shim_frame_width(void) { return g_frameW; }
EXPORT unsigned shim_frame_height(void) { return g_frameH; }
EXPORT unsigned shim_frame_pitch(void) { return g_framePitch; }
EXPORT int shim_frame_updated(void) { return g_frameUpdated; }
/* 0 = 0RGB1555, 1 = XRGB8888, 2 = RGB565 (theo enum retro_pixel_format) */
EXPORT int shim_pixel_format(void) { return (int) g_pixelFormat; }

/* Âm thanh: int16 xen kẽ L,R. */
EXPORT const int16_t* shim_audio_ptr(void) { return g_audio; }
EXPORT size_t shim_audio_frames(void) { return g_audioFrames; }

/* Thông số thật do nhân báo (chỉ hợp lệ sau shim_load_game). */
EXPORT double shim_fps(void) {
	struct retro_system_av_info av;
	memset(&av, 0, sizeof(av));
	retro_get_system_av_info(&av);
	return av.timing.fps;
}

EXPORT double shim_sample_rate(void) {
	struct retro_system_av_info av;
	memset(&av, 0, sizeof(av));
	retro_get_system_av_info(&av);
	return av.timing.sample_rate;
}

/* Battery save (SRAM/Flash/EEPROM). */
EXPORT size_t shim_sram_size(void) { return retro_get_memory_size(RETRO_MEMORY_SAVE_RAM); }
EXPORT void* shim_sram_ptr(void) { return retro_get_memory_data(RETRO_MEMORY_SAVE_RAM); }

/* Cheat code. code: GameShark / Action Replay / CodeBreaker; nhiều dòng ngăn bằng '+'. */
EXPORT void shim_cheat_reset(void) { retro_cheat_reset(); }
EXPORT void shim_cheat_set(unsigned index, int enabled, const char* code) {
	retro_cheat_set(index, enabled != 0, code);
}

/* Save state. */
EXPORT size_t shim_state_size(void) { return retro_serialize_size(); }
EXPORT int shim_state_save(void* buf, size_t size) { return retro_serialize(buf, size) ? 1 : 0; }
EXPORT int shim_state_load(const void* buf, size_t size) {
	return retro_unserialize(buf, size) ? 1 : 0;
}
