#pragma once

typedef enum {
	MIDNIGHT = 0, MORNING, NOON, EVENING
	// 12am, 6am, 12pm, 6pm
} daypart_t;

typedef struct {
	float flTimeOfDay;
	float flLastTimeOfDay;
	daypart_t iDayPart;
} skyinfo_t;

extern skyinfo_t g_SkyInfo;