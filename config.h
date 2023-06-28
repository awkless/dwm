/* See LICENSE file for copyright and license details. */

/* Multimedia keys... */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int gappx     = 5;        /* gaps between windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 8;        /* vertical padding of bar */
static const int sidepad            = 16;       /* horizontal padding of bar */
static const char *fonts[] = { "Hurmit Nerd Font:style=Regular:size=12" };
static const char fg_norm[]      = "#fce8c3";
static const char fg_sel[]       = "#f75341";
static const char bg_norm[]      = "#1c1b19";
static const char border_norm[]  = "#918175";
static const char border_sel[]   = "#3a3a3a";
static const char *colors[][3]   = {
	/*               fg         bg         border   */
	[SchemeNorm] = { fg_norm, bg_norm, bg_norm },
	[SchemeSel]  = { fg_sel, border_sel,  border_sel  },
};

/* tagging */
static const char *tags[] =
	{ "", "󰻣", "", "", "", "󰊴", "", "", ""};

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* Layout: class, instance, title, tags mask, isfloating, monitor */
	/* NOTE: monitor of -1 will make application appear on currently focused
	 * monitor, while 1 will make application appear in primary monitor. */

	/* Rules for Gimp... */
	{ "gimp", NULL, NULL, 1 << 7, 1, -1 },
	{ "Gimp", NULL, NULL, 1 << 7, 1, -1 },
	{ "feh", NULL, NULL, 1 << 7, 1, -1 },

	/* Rules for Word and PDF editing/viewing applications... */
	{ "libreoffice", NULL, NULL, 1 << 4, 1, -1 },
	{ "okular", NULL, NULL, 1 << 4, 1, -1 },

	/* Rules for ST... */
	{ "st-256color", NULL, "calcurse", 0, 0, -1 },
	{ "st-256color", NULL, "mutt", 1 << 1, 0, -1 },
	{ "st-256color", NULL, "st", 1 << 2, 0, -1 },
	{ "st-256color", NULL, "tmux", 1 << 2,  0, -1 },

	/* Music audio...*/
	{ "pavucontrol", NULL, NULL, 1 << 6, 0, -1 },
	{ "Pavucontrol", NULL, NULL, 1 << 6, 0, -1 },
	{ "st-256color", NULL, "cmus", 1 << 6, 0, -1 },

	/* Rules for Firefox web browser... */
	{ "firefox",  NULL,       NULL, 1 << 3, 0, -1 },
	{ "Firefox",  NULL,       NULL, 1 << 3, 0, -1 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[  ]",      tile },    /* first entry is default */
	{ "[  ]",      NULL },    /* no layout function means floating behavior */
	{ "[  ]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask    /* Super/Windows key */
#define CONTROL ControlMask /* Control key */
#define SHIFT ShiftMask   /* Shift key*/
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,               KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|CONTROL,       KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|SHIFT,         KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|CONTROL|SHIFT, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *passmenu[] = { "passmenu2", "-m", dmenumon, "-g", "1", "-l", "5", NULL };
static const char *termcmd[]  = { "st", NULL };

/* Commands to manipulate volume... */
static const char *up_vol[]   =
	{"pactl", "set-sink-volume", "0", "+5%", NULL};
static const char *down_vol[] =
	{"pactl", "set-sink-volume", "0", "-5%", NULL};
static const char *mute_vol[] =
	{"pactl", "set-sink-mute", "0", "toggle", NULL};

/* Commands to manipulate screen brightness... */
static const char *brighter[] = {"brightnessctl", "set", "5%+", NULL};
static const char *dimmer[]   = {"brightnessctl", "set", "5%-", NULL};

/* Use scrot for screen shots... */
static const char *screen_shot[] =
	{"scrot", "/home/awkless/pictures/screenshots/%Y-%m-%d-%T.jpg", NULL};

/* Lock screen... */
static const char *slock[] = {"slock", NULL};

/* Reboot... */
static const char *reboot[] = {"reboot", NULL};

/* Shutdown... */
static const char *shutdown[] = {"shutdown", "-h", "now", NULL};

/* Keys follow this pattern:
 * modifier, key, function, argument
 */
static const Key keys[] = {
	/* Volume control keys... */
	{0, XF86XK_AudioMute, spawn, {.v = mute_vol}},
	{0, XF86XK_AudioLowerVolume, spawn, {.v = down_vol}},
	{0, XF86XK_AudioRaiseVolume, spawn, {.v = up_vol}},
	{ MODKEY, XK_F2, spawn, {.v = down_vol}},
	{ MODKEY, XK_F3, spawn, {.v = up_vol}},
	{ MODKEY, XK_F1, spawn, {.v = mute_vol}},

	/* Print screen... */
	{ 0, XK_Print, spawn, {.v = screen_shot}},

	/* Lock screen... */
	{MODKEY, XK_Escape, spawn, {.v = slock}},

	/* Reboot... */
	{MODKEY|ShiftMask, XK_Escape, spawn, {.v = reboot}},

	/* Shutdown... */
	{MODKEY|ControlMask|ShiftMask, XK_Escape, spawn, {.v = shutdown}},

	/* Screen brightness control keys... */
	{0, XF86XK_MonBrightnessDown, spawn, {.v = dimmer}},
	{0, XF86XK_MonBrightnessUp, spawn, {.v = brighter}},
	{ MODKEY, XK_F8, spawn, {.v = dimmer}},
	{ MODKEY, XK_F9, spawn, {.v = brighter}},

	/* Open password manager... */
	{ MODKEY, XK_p, spawn, {.v = passmenu} },

	/* Open dmenu... */
	{ MODKEY, XK_r, spawn, {.v = dmenucmd} },

	/* Open a terminal... */
	{ MODKEY|ShiftMask, XK_Return, spawn, {.v = termcmd } },

	/* Show/hide status bar... */
	{ MODKEY, XK_b, togglebar, {0} },

	/* Focus on next/previous window in current tag... */
	{ MODKEY, XK_j, focusstack, {.i = -1 } },
	{ MODKEY, XK_k, focusstack, {.i = +1 } },

	/* Increase/decrease number of windows on master... */
	{ MODKEY, XK_i, incnmaster, {.i = +1 } },
	{ MODKEY, XK_d, incnmaster, {.i = -1 } },

	/* Increase/decrease master size... */
	{ MODKEY, XK_h, setmfact, {.f = -0.05} },
	{ MODKEY, XK_l, setmfact, {.f = +0.05} },

	/* Push active window from stack to master, or pulls last used window
	 * from stack onto master. */
	{ MODKEY, XK_Return, zoom, {0} },

	/* Switch to previous tag... */
	{ MODKEY, XK_Tab, view, {0} },

	/* Kill focused window... */
	{ MODKEY|ShiftMask, XK_c, killclient,     {0} },

	/* Enter tile mode... */
	{ MODKEY, XK_t, setlayout, {.v = &layouts[0]} },

	/* Enter floating mode...*/
	{ MODKEY, XK_f, setlayout, {.v = &layouts[1]} },

	/* Make individual window float... */
	{ MODKEY|ShiftMask, XK_space, togglefloating, {0} },

	/* Enter monical mode... */
	{ MODKEY, XK_m, setlayout, {.v = &layouts[2]} },

	/* Toggle previous layout mode... */
	{ MODKEY, XK_space, setlayout, {0} },

	/* View all windows on screen... */
	{ MODKEY, XK_0, view, {.ui = ~0 } },

	/* Make focused window appear on all tags... */
	{ MODKEY|ShiftMask, XK_0, tag, {.ui = ~0 } },

	/* Move focus between screens (multi monitor setup)... */
	{ MODKEY, XK_comma, focusmon, {.i = -1 } },
	{ MODKEY, XK_period, focusmon, {.i = +1 } },

	/* Move active window to different screen... */
	{ MODKEY|ShiftMask, XK_comma, tagmon, {.i = -1 } },
	{ MODKEY|ShiftMask, XK_period, tagmon, {.i = +1 } },

	/* Manipulate window gaps... */
	{ MODKEY, XK_minus, setgaps, {.i = -1 } },
	{ MODKEY, XK_equal, setgaps, {.i = +1 } },
	{ MODKEY|ShiftMask, XK_equal, setgaps, {.i = 0  } },

	/* Change tabs... */
	TAGKEYS(XK_1, 0)
	TAGKEYS(XK_2, 1)
	TAGKEYS(XK_3, 2)
	TAGKEYS(XK_4, 3)
	TAGKEYS(XK_5, 4)
	TAGKEYS(XK_6, 5)
	TAGKEYS(XK_7, 6)
	TAGKEYS(XK_8, 7)
	TAGKEYS(XK_9, 8)

	/* Exit DWM... */
	{ MODKEY|ShiftMask, XK_q, quit, {0} },

	/* Restart DWM... */
	{ MODKEY|ControlMask|ShiftMask, XK_q, quit, {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle,
 * ClkClientWin, or ClkRootWin */
/* Format of click, event mask, button, function, argument... */
static const Button buttons[] = {
	/* Left clicking on layout symbol on bar toggles previous layout... */
	{ ClkLtSymbol, 0, Button1, setlayout, {0} },

	/* Right clicking on layout symbol switches to monical mode... */
	{ ClkLtSymbol, 0, Button3, setlayout, {.v = &layouts[2]} },

	/* Middle clicking on window title pushes active window from stack to
	 * master, or pulls last used window from stack onto master. */
	{ ClkWinTitle, 0, Button2, zoom, {0} },

	/* Middle clicking on status text in bar opens a terminal... */
	{ ClkStatusText, 0, Button2, spawn, {.v = termcmd } },

	/* MODKEY + left click moves a window by mouse... */
	{ ClkClientWin, MODKEY, Button1, movemouse, {0} },

	/* MODKEY + middle click toggles focused window into floating mode... */
	{ ClkClientWin, MODKEY, Button2, togglefloating, {0} },

	/* MODKEY + right click resizes focused window by mouse... */
	{ ClkClientWin, MODKEY, Button3, resizemouse, {0} },

	/* Left click a tag in bar switches view to that tag... */
	{ ClkTagBar, 0, Button1, view, {0} },

	/* Right click a tag in bar toggles view of windows in that tag... */
	{ ClkTagBar, 0, Button3, toggleview, {0} },

	/* MODKEY + Left click on tag in bar moves focused window into that
	 * tag... */
	{ ClkTagBar, MODKEY, Button1, tag, {0} },

	/* MODKEY + Right click on tag will make focused window appear in that
	 * tag as well as current tag it resides in... */
	{ ClkTagBar, MODKEY, Button3, toggletag, {0} },
};

