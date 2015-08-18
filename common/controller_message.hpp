
struct SendStruct {
	int type;
	int x;
	int y;
	int button;
	int keycode;
};

enum messageType {
	MOUSE_MOTION,
	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_UP,
	KEY_DOWN,
	KEY_UP,
	STREAMER_START,
	STREAMER_STOP
};