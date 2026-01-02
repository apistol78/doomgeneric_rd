#include "config.h"

#include <stdio.h>
#include <stdlib.h>

#include <Runtime/Audio.h>
#include <Runtime/Kernel.h>

#include "deh_str.h"
#include "i_sound.h"
#include "i_system.h"
#include "i_swap.h"
#include "m_argv.h"
#include "m_misc.h"
#include "w_wad.h"
#include "z_zone.h"

#include "doomtype.h"

#define MIXER_FREQ 22050

typedef struct
{
	int32_t nsamples;
	int16_t* samples;
}
rv_sound_data_t;

static boolean use_sfx_prefix = false;

static void GetSfxLumpName(sfxinfo_t *sfx, char *buf, size_t buf_len)
{
	if (sfx->link != NULL)
		sfx = sfx->link;

	if (use_sfx_prefix)
		M_snprintf(buf, buf_len, "ds%s", DEH_String(sfx->name));
	else
		M_StringCopy(buf, DEH_String(sfx->name), buf_len);
}

static rv_sound_data_t* AllocateSound(int32_t len)
{
	rv_sound_data_t* sd = (rv_sound_data_t*)malloc(sizeof(rv_sound_data_t) + len * sizeof(int16_t));
	sd->nsamples = len;
	sd->samples = (int16_t*)(sd + 1);
	return sd;
}

static boolean ExpandSoundData(sfxinfo_t *sfxinfo,
								   byte *data,
								   int samplerate,
								   int length)
{
	rv_sound_data_t* sd;
	uint32_t expanded_length;
 
	// Calculate the length of the expanded version of the sample.    
	expanded_length = (uint32_t) ((((uint64_t) length) * MIXER_FREQ) / samplerate);

	// // Double up twice: 8 -> 16 bit and mono -> stereo
	// expanded_length *= 4;

	// Allocate a chunk in which to expand the sound
	sd = AllocateSound(expanded_length);
	if (sd == 0)
	{
		printf("*** ExpandSoundData; failed to allocate driver data %d\n", expanded_length);
		return false;
	}

	int expand_ratio;
	int i;

	expanded_length = ((uint64_t) length * MIXER_FREQ) / samplerate;
	expand_ratio = (length << 8) / expanded_length;

	for (i = 0; i < expanded_length; ++i)
	{
		int16_t sample;
		int src;

		src = (i * expand_ratio) >> 8;

		sample = data[src] | (data[src] << 8);
		sample -= 32768;

		sd->samples[i] = sample;
	}
	
	sfxinfo->driver_data = sd;
	return true;
}

static boolean CacheSFX(sfxinfo_t *sfxinfo)
{
	int lumpnum;
	unsigned int lumplen;
	int samplerate;
	unsigned int length;
	byte *data;

	if (sfxinfo->driver_data != 0)
		return true;

	lumpnum = sfxinfo->lumpnum;
	data = W_CacheLumpNum(lumpnum, PU_STATIC);
	lumplen = W_LumpLength(lumpnum);

	if (lumplen < 8 || data[0] != 0x03 || data[1] != 0x00)
	{
		printf("*** CacheSFX; invalid lump\n");
		return false;
	}

	// 16 bit sample rate field, 32 bit length field
	samplerate = (data[3] << 8) | data[2];
	length = (data[7] << 24) | (data[6] << 16) | (data[5] << 8) | data[4];

	if (length > lumplen - 8 || length <= 48)
	{
		printf("*** CacheSFX; invalid length in lump\n");
		return false;
	}

	data += 16;
	length -= 32;

	// Sample rate conversion
	if (!ExpandSoundData(sfxinfo, data + 8, samplerate, length))
		return false;

	// don't need the original lump any more
	W_ReleaseLumpNum(lumpnum);
	return true;
}

///

#define NUM_CHANNELS 4

static boolean I_SDL_InitSound(boolean _use_sfx_prefix)
{
	use_sfx_prefix = _use_sfx_prefix;
	return true;
}

static void I_SDL_ShutdownSound(void)
{ 
}

static int I_SDL_GetSfxLumpNum(sfxinfo_t *sfx)
{
	char namebuf[9];
	GetSfxLumpName(sfx, namebuf, sizeof(namebuf));
	return W_GetNumForName(namebuf);
}

static void I_SDL_UpdateSound(void)
{
}

static void I_SDL_UpdateSoundParams(int handle, int vol, int sep)
{
	rt_audio_set_channel_volume(handle, (uint8_t)(vol >> 2));
}

static int I_SDL_StartSound(sfxinfo_t *sfxinfo, int channel, int vol, int sep)
{
	if (!CacheSFX(sfxinfo) || channel >= NUM_CHANNELS)
		return -1;

	const rv_sound_data_t* snd = sfxinfo->driver_data;

	rt_audio_play(channel, snd->samples, snd->nsamples, RT_AUDIO_MODE_REPLACE | RT_AUDIO_MODE_MONO);
	rt_audio_set_channel_volume(channel, (uint8_t)(vol >> 2));

	return channel;
}

static void I_SDL_StopSound(int handle)
{
}

static boolean I_SDL_SoundIsPlaying(int handle)
{
	return rt_audio_is_channels_busy(1 << handle);
}

static void I_SDL_PrecacheSounds(sfxinfo_t *sounds, int num_sounds)
{
}

static snddevice_t sound_sdl_devices[] = 
{
	SNDDEVICE_SB,
	SNDDEVICE_PAS,
	SNDDEVICE_GUS,
	SNDDEVICE_WAVEBLASTER,
	SNDDEVICE_SOUNDCANVAS,
	SNDDEVICE_AWE32,
};

sound_module_t DG_sound_module = 
{
	sound_sdl_devices,
	arrlen(sound_sdl_devices),
	I_SDL_InitSound,
	I_SDL_ShutdownSound,
	I_SDL_GetSfxLumpNum,
	I_SDL_UpdateSound,
	I_SDL_UpdateSoundParams,
	I_SDL_StartSound,
	I_SDL_StopSound,
	I_SDL_SoundIsPlaying,
	I_SDL_PrecacheSounds,
};

