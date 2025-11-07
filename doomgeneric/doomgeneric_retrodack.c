#include <stdio.h>
#include "doomgeneric.h"
#include "doomkeys.h"

#include <Runtime/Input.h>
#include <Runtime/Runtime.h>
#include <Runtime/Timer.h>
#include <Runtime/Video.h>

void DG_Init()
{
	runtime_init();
	rt_video_set_mode(VMODE_320_200_8);
}

void DG_DrawFrame()
{
	static int count = 0;
	if (++count >= 60)
	{
		static uint32_t last_ms = 0;
		uint32_t ms = rt_timer_get_ms();
		printf("%d fps\n", (60 * 1000) / (ms - last_ms));
		last_ms = ms;
		count = 0;
	}

	struct color {
	    uint32_t b:8;
	    uint32_t g:8;
	    uint32_t r:8;
	    uint32_t a:8;
	};
	extern struct color colors[256];
	for (uint32_t i = 0; i < 256; ++i)
		rt_video_set_palette(i, *(uint32_t*)&colors[i]);

	rt_video_blit(DG_ScreenBuffer);
	rt_video_wait();
	rt_video_present(0);
}

void DG_SleepMs(uint32_t ms)
{
	rt_timer_wait_ms(ms);
}

uint32_t DG_GetTicksMs()
{
	return rt_timer_get_ms();
}

int DG_GetKey(int* pressed, unsigned char* doomKey)
{
	rt_event_t ev;
	if (rt_input_get_event(&ev))
	{
		switch (ev.button)
		{
		case RT_INPUT_BUTTON_A:
			*doomKey = KEY_ENTER;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_BUTTON_B:
			*doomKey = KEY_FIRE;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_BUTTON_C:
			*doomKey = KEY_USE;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_BUTTON_D:
			*doomKey = KEY_ESCAPE;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_DPAD_W:
			*doomKey = KEY_LEFTARROW;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_DPAD_E:
			*doomKey = KEY_RIGHTARROW;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_DPAD_N:
			*doomKey = KEY_UPARROW;
			*pressed = ev.pressed;
			return 1;

		case RT_INPUT_DPAD_S:
			*doomKey = KEY_DOWNARROW;
			*pressed = ev.pressed;
			return 1;
		}
	}

	// static uint8_t lst = 0;
	// const uint8_t st = (uint8_t)rt_input_get_state();

	// if (st != 0)
	// {
	// 	*doomKey = KEY_ENTER;
	// 	if (lst != st)
	// 		*pressed = 1;
	// 	else
	// 		*pressed = 0;

	// 	lst = st;
	// 	return 1;
	// }
	// lst = st;

	// uint8_t ikc;
	// uint8_t im;
	// uint8_t ip;

	// if (input_get_kb_event(&ikc, &im, &ip))
	// {
	// 	switch (ikc)
	// 	{
	// 	case RT_KEY_LEFT:
	// 		*doomKey = KEY_LEFTARROW;
	// 		break;

	// 	case RT_KEY_A:
	// 		*doomKey = KEY_STRAFE_L;
	// 		break;

	// 	case RT_KEY_RIGHT:
	// 		*doomKey = KEY_RIGHTARROW;
	// 		break;

	// 	case RT_KEY_D:
	// 		*doomKey = KEY_STRAFE_R;
	// 		break;

	// 	case RT_KEY_UP:
	// 	case RT_KEY_W:
	// 		*doomKey = KEY_UPARROW;
	// 		break;

	// 	case RT_KEY_DOWN:
	// 	case RT_KEY_S:
	// 		*doomKey = KEY_DOWNARROW;
	// 		break;

	// 	case RT_KEY_E:
	// 		*doomKey = KEY_USE;
	// 		break;

	// 	case RT_KEY_SPACE:
	// 		*doomKey = KEY_FIRE;
	// 		break;

	// 	case RT_KEY_ESCAPE:
	// 		*doomKey = KEY_ESCAPE;
	// 		break;

	// 	case RT_KEY_RETURN:
	// 	case RT_KEY_ENTER:
	// 		*doomKey = KEY_ENTER;
	// 		break;
	// 	}

	// 	*pressed = ip;
	// 	return 1;
	// }
	
	return 0;
}

void DG_SetWindowTitle(const char* title)
{
}


int main(int argc, char **argv)
{
    doomgeneric_Create(0, 0);
    for (int i = 0; ; i++)
    {
        doomgeneric_Tick();
    }
    return 0;
}