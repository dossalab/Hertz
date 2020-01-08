#ifndef ENGINE_EVENTS_H
#define ENGINE_EVENTS_H

enum event_type {
	ENG_EVENT_MOUSE,
	ENG_EVENT_KEYBOARD,

	ENG_EVENT_COUNT
};

struct mouse_data {
	double x, y;
};

struct keyboard_data {
	int key;
	int action;
	int mod;
};

struct event_data {
	int type;

	union {
		struct mouse_data mouse;
		struct keyboard_data keyboard;
	};
};

void engine_subscribe(int event, void (*handler)(struct event_data *));
void engine_events_init(void);

#endif

