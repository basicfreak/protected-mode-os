//COMMONVARS.H

#ifndef				COMMONVARS_H_INCLUDED
	#define				COMMONVARS_H_INCLUDED
	
	#define				debug					1

	#define				TRUE					1
	#define				true					1
	#define				false					0
	#define				FALSE					0

//1
	#define				Heads_Floppy_360		2
	#define				Sectors_Floppy_360		9
	#define				Cylinders_Floppy_360	40
	#define				Datarate_Floppy_360		2
//2
	#define				Heads_Floppy_12			2
	#define				Sectors_Floppy_12		15
	#define				Cylinders_Floppy_12		80
	#define				Datarate_Floppy_12		0
//3
	#define				Heads_Floppy_720		2
	#define				Sectors_Floppy_720		9
	#define				Cylinders_Floppy_720	80
	#define				Datarate_Floppy_720		1
//4
	#define				Heads_Floppy_144		2
	#define				Sectors_Floppy_144		18
	#define				Cylinders_Floppy_144	80
	#define				Datarate_Floppy_144		0
//5
	#define				Heads_Floppy_288		2
	#define				Sectors_Floppy_288		36
	#define				Cylinders_Floppy_288	80
	#define				Datarate_Floppy_288		3
	
	uint64_t			TotalRAM;
	unsigned char		*RAM;
	char 				explode_cmd[50][100];
	unsigned int 		curX;
	unsigned int 		curY;
	int					COLS;
	int					ROWS;
	int					color;
	int					movcur_disable;
	int					timer_ticks;
	int					timer_seconds;
	int					timer_minutes;
	int					timer_hours;
	int					timer_days;
	int					timer_years;
	char				keyboard_buffer;
	int					keyboard_ctrl;
	int					keyboard_alt;
	int					keyboard_shift;
	char				*cmd_command[1024];
	bool				CMD_ACTIVE;
	
	int					CMOS_Floppy_Master;			//0=none 1=360KB 5.25 2=1.2MB 5.25 3=720KB 3.5 4=1.44MB 3.5 5=2.88MB 3.5
	int					CMOS_Floppy_Slave;			//0=none 1=360KB 5.25 2=1.2MB 5.25 3=720KB 3.5 4=1.44MB 3.5 5=2.88MB 3.5
	int					floppy_drive_number;
	int 				curFloppy_sectors, curFloppy_tracks, curFloppy_cylinders, curFloppy_speed, curFloppy_type;
	

	enum KEYCODE {

	// Alphanumeric keys ////////////////

		KEY_SPACE             = ' ',
		KEY_0                 = '0',
		KEY_1                 = '1',
		KEY_2                 = '2',
		KEY_3                 = '3',
		KEY_4                 = '4',
		KEY_5                 = '5',
		KEY_6                 = '6',
		KEY_7                 = '7',
		KEY_8                 = '8',
		KEY_9                 = '9',

		KEY_A                 = 'a',
		KEY_B                 = 'b',
		KEY_C                 = 'c',
		KEY_D                 = 'd',
		KEY_E                 = 'e',
		KEY_F                 = 'f',
		KEY_G                 = 'g',
		KEY_H                 = 'h',
		KEY_I                 = 'i',
		KEY_J                 = 'j',
		KEY_K                 = 'k',
		KEY_L                 = 'l',
		KEY_M                 = 'm',
		KEY_N                 = 'n',
		KEY_O                 = 'o',
		KEY_P                 = 'p',
		KEY_Q                 = 'q',
		KEY_R                 = 'r',
		KEY_S                 = 's',
		KEY_T                 = 't',
		KEY_U                 = 'u',
		KEY_V                 = 'v',
		KEY_W                 = 'w',
		KEY_X                 = 'x',
		KEY_Y                 = 'y',
		KEY_Z                 = 'z',

		KEY_RETURN            = '\r',
		KEY_ESCAPE            = 0x1001,
		KEY_BACKSPACE         = '\b',

	// Arrow keys ////////////////////////

		KEY_UP                = 0x1100,
		KEY_DOWN              = 0x1101,
		KEY_LEFT              = 0x1102,
		KEY_RIGHT             = 0x1103,

	// Function keys /////////////////////

		KEY_F1                = 0x1201,
		KEY_F2                = 0x1202,
		KEY_F3                = 0x1203,
		KEY_F4                = 0x1204,
		KEY_F5                = 0x1205,
		KEY_F6                = 0x1206,
		KEY_F7                = 0x1207,
		KEY_F8                = 0x1208,
		KEY_F9                = 0x1209,
		KEY_F10               = 0x120a,
		KEY_F11               = 0x120b,
		KEY_F12               = 0x120b,
		KEY_F13               = 0x120c,
		KEY_F14               = 0x120d,
		KEY_F15               = 0x120e,

		KEY_DOT               = '.',
		KEY_COMMA             = ',',
		KEY_COLON             = ':',
		KEY_SEMICOLON         = ';',
		KEY_SLASH             = '/',
		KEY_BACKSLASH         = '\\',
		KEY_PLUS              = '+',
		KEY_MINUS             = '-',
		KEY_ASTERISK          = '*',
		KEY_EXCLAMATION       = '!',
		KEY_QUESTION          = '?',
		KEY_QUOTEDOUBLE       = '\"',
		KEY_QUOTE             = '\'',
		KEY_EQUAL             = '=',
		KEY_HASH              = '#',
		KEY_PERCENT           = '%',
		KEY_AMPERSAND         = '&',
		KEY_UNDERSCORE        = '_',
		KEY_LEFTPARENTHESIS   = '(',
		KEY_RIGHTPARENTHESIS  = ')',
		KEY_LEFTBRACKET       = '[',
		KEY_RIGHTBRACKET      = ']',
		KEY_LEFTCURL          = '{',
		KEY_RIGHTCURL         = '}',
		KEY_DOLLAR            = '$',
		KEY_POUND             = '£',
		KEY_EURO              = '$',
		KEY_LESS              = '<',
		KEY_GREATER           = '>',
		KEY_BAR               = '|',
		KEY_GRAVE             = '`',
		KEY_TILDE             = '~',
		KEY_AT                = '@',
		KEY_CARRET            = '^',

	// Numeric keypad //////////////////////

		KEY_KP_0              = '0',
		KEY_KP_1              = '1',
		KEY_KP_2              = '2',
		KEY_KP_3              = '3',
		KEY_KP_4              = '4',
		KEY_KP_5              = '5',
		KEY_KP_6              = '6',
		KEY_KP_7              = '7',
		KEY_KP_8              = '8',
		KEY_KP_9              = '9',
		KEY_KP_PLUS           = '+',
		KEY_KP_MINUS          = '-',
		KEY_KP_DECIMAL        = '.',
		KEY_KP_DIVIDE         = '/',
		KEY_KP_ASTERISK       = '*',
		KEY_KP_NUMLOCK        = 0x300f,
		KEY_KP_ENTER          = 0x3010,

		KEY_TAB               = 0x4000,
		KEY_CAPSLOCK          = 0x4001,

	// Modify keys ////////////////////////////

		KEY_LSHIFT            = 0x4002,
		KEY_LCTRL             = 0x4003,
		KEY_LALT              = 0x4004,
		KEY_LWIN              = 0x4005,
		KEY_RSHIFT            = 0x4006,
		KEY_RCTRL             = 0x4007,
		KEY_RALT              = 0x4008,
		KEY_RWIN              = 0x4009,

		KEY_INSERT            = 0x400a,
		KEY_DELETE            = 0x400b,
		KEY_HOME              = 0x400c,
		KEY_END               = 0x400d,
		KEY_PAGEUP            = 0x400e,
		KEY_PAGEDOWN          = 0x400f,
		KEY_SCROLLLOCK        = 0x4010,
		KEY_PAUSE             = 0x4011,

		KEY_UNKNOWN,
		KEY_NUMKEYCODES
	};

#endif