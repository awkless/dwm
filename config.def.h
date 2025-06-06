/* See LICENSE file for copyright and license details. */

/* Multimedia keys... */
#include <X11/XF86keysym.h>

/* appearance */
static const unsigned int borderpx  = 4;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayonleft  = 0;   /* 0: systray in the right corner, >0: systray on left of status text */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray        = 1;        /* 0 means no systray */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const int vertpad            = 16;       /* vertical padding of bar */
static const int sidepad            = 16;       /* horizontal padding of bar */
static const int horizpadbar        = 8;        /* horizontal padding for statusbar */
static const int vertpadbar         = 8;        /* vertical padding for statusbar */
static const unsigned int gappih    = 16;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 16;       /* vert inner gap between windows */
static const unsigned int gappoh    = 16;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 16;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const char *fonts[]          = { "IosevkaTerm Nerd Font:style=Regular:size=16" };
static const char dmenufont[]       = "IosevkaTerm Nerd Font:style=Regular:size=16";

static const char fg_norm[]      = "#D700FF";
static const char fg_sel[]       = "#D7005F";
static const char bg_norm[]      = "#0E0224";
static const char border_norm[]  = "#5F5FFF";
static const char border_sel[]   = "#D700FF";
static const char *colors[][3]      = {
	[SchemeNorm] = { fg_norm, bg_norm,     border_norm },
	[SchemeSel]  = { fg_sel,  border_norm, border_sel },
};

static const char *tags[] = { "󰋜", "", "󰖟", "󰇮", "", "󱎓", "", "", ""};
static const char *tagsel[][2] = {
	{ "#00D700", "#0E0224" },
	{ "#FFAF00", "#0E0224" },
	{ "#D7005F", "#0E0224" },
	{ "#5F5FFF", "#0E0224" },
	{ "#D7005F", "#0E0224" },
	{ "#D7005F", "#0E0224" },
	{ "#D7005F", "#0E0224" },
	{ "#D7005F", "#0E0224" },
	{ "#D7005F", "#0E0224" },
};

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 2;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall = 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class      instance    title       tags mask     isfloating   monitor */
	/* Rules for image processing... */
	{ "gimp", NULL, NULL, 1 << 4, 1, 0 },
	{ "Gimp", NULL, NULL, 1 << 4, 1, 0 },
	{ "feh",  NULL, NULL, 1 << 4, 1, 0 },

	/* Rules for Word and PDF editing/viewing applications... */
	{ "libreoffice", NULL, NULL, 1 << 3, 0, 0 },
	{ "okular",      NULL, NULL, 1 << 3, 0, 0 },

	/* Rules for ST... */
	{ "st-256color", NULL, "mutt", 1 << 3, 0, 0 },
	{ "st-256color", NULL, "aerc", 1 << 3, 0, 0 },
	{ "st-256color", NULL, "st",   1 << 1, 0, 0 },
	{ "st-256color", NULL, "tmux", 1 << 1, 0, 0 },

	/* Music audio...*/
	{ "pavucontrol", NULL, NULL,   1 << 6, 0, 0 },
	{ "Pavucontrol", NULL, NULL,   1 << 6, 0, 0 },
	{ "st-256color", NULL, "cmus", 1 << 6, 0, 0 },

	/* Rules for Firefox web browser... */
	{ "firefox",  NULL, NULL, 1 << 2, 0, 0 },
	{ "Firefox",  NULL, NULL, 1 << 2, 0, 0 },
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 1;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[󰐕]",      tile },    /* first entry is default */
	{ "[󰊓]",      NULL },    /* no layout function means floating behavior */
	{ "[]",      monocle },
};

/* key definitions */
#define MODKEY  Mod4Mask
#define ALT     Mod1Mask
#define CTRL ControlMask
#define SHIFT   ShiftMask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,            KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|CTRL,       KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|SHIFT,      KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|CTRL|SHIFT, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, NULL };
static const char *termcmd[]  = { "st", NULL };
static const char *up_vol[]   = {"pamixer", "-i", "5", NULL};
static const char *down_vol[] = {"pamixer", "-d", "5", NULL};
static const char *mute_vol[] = {"pamixer", "--toggle-mute", NULL};
static const char *passmenu[] =
	{ "passmenu2", "-m", dmenumon, "-g", "1", "-l", "5", NULL};
static const char *scrot[]    =
	{"scrot", "/home/awkless/pictures/screenshots/%Y-%m-%d-%T.jpg", NULL};

static const Key keys[] = {
	/* modifier         key                      function        argument */
	{ 0,                XF86XK_AudioMute,        spawn,          {.v = mute_vol} },
	{ 0,                XF86XK_AudioLowerVolume, spawn,          {.v = down_vol} },
	{ 0,                XF86XK_AudioRaiseVolume, spawn,          {.v = up_vol} },
	{ MODKEY,           XK_F2,                   spawn,          {.v = down_vol} },
	{ MODKEY,           XK_F3,                   spawn,          {.v = up_vol} },
	{ MODKEY,           XK_F1,                   spawn,          {.v = mute_vol} },
	{ MODKEY,           XK_p,                    spawn,          {.v = passmenu } },
	{ MODKEY,           XK_r,                    spawn,          {.v = dmenucmd } },
	{ MODKEY|SHIFT,     XK_Return,               spawn,          {.v = termcmd } },
	{ 0,                XK_Print,                spawn,          {.v = scrot} },
	{ MODKEY,           XK_b,                    togglebar,      {0} },
	{ MODKEY,           XK_j,                    focusstack,     {.i = +1 } },
	{ MODKEY,           XK_k,										 focusstack,     {.i = -1 } },
	{ MODKEY,           XK_i,										 incnmaster,     {.i = +1 } },
	{ MODKEY,           XK_d,										 incnmaster,     {.i = -1 } },
	{ MODKEY,           XK_h,										 setmfact,       {.f = -0.05} },
	{ MODKEY,           XK_l,										 setmfact,       {.f = +0.05} },
	{ MODKEY|ALT,       XK_h,										 incrgaps,       {.i = +1 } },
	{ MODKEY|ALT,       XK_l,										 incrgaps,       {.i = -1 } },
	{ MODKEY|ALT|SHIFT, XK_h,										 incrogaps,      {.i = +1 } },
	{ MODKEY|ALT|SHIFT, XK_l,										 incrogaps,      {.i = -1 } },
	{ MODKEY|ALT|CTRL,  XK_h,										 incrigaps,      {.i = +1 } },
	{ MODKEY|ALT|CTRL,  XK_l,										 incrigaps,      {.i = -1 } },
	{ MODKEY|ALT,       XK_0,                    togglegaps,     {0} },
	{ MODKEY|ALT|SHIFT, XK_0,										 defaultgaps,    {0} },
	{ MODKEY,           XK_y,										 incrihgaps,     {.i = +1 } },
	{ MODKEY,           XK_o,										 incrihgaps,     {.i = -1 } },
	{ MODKEY|CTRL,      XK_y,										 incrivgaps,     {.i = +1 } },
	{ MODKEY|CTRL,      XK_o,										 incrivgaps,     {.i = -1 } },
	{ MODKEY|ALT,       XK_y,										 incrohgaps,     {.i = +1 } },
	{ MODKEY|ALT,       XK_o,										 incrohgaps,     {.i = -1 } },
	{ MODKEY|SHIFT,     XK_y,										 incrovgaps,     {.i = +1 } },
	{ MODKEY|SHIFT,     XK_o,										 incrovgaps,     {.i = -1 } },
	{ MODKEY,           XK_Return,							 zoom,           {0} },
	{ MODKEY,           XK_Tab,									 view,           {0} },
	{ MODKEY|SHIFT,     XK_c,										 killclient,     {0} },
	{ MODKEY,           XK_t,										 setlayout,      {.v = &layouts[0]} },
	{ MODKEY,           XK_f,										 setlayout,      {.v = &layouts[1]} },
	{ MODKEY,           XK_m,										 setlayout,      {.v = &layouts[2]} },
	{ MODKEY,           XK_space,								 setlayout,      {0} },
	{ MODKEY|SHIFT,     XK_space,								 togglefloating, {0} },
	{ MODKEY,           XK_0,										 view,           {.ui = ~0 } },
	{ MODKEY|SHIFT,     XK_0,										 tag,            {.ui = ~0 } },
	{ MODKEY,           XK_comma,								 focusmon,       {.i = -1 } },
	{ MODKEY,           XK_period,							 focusmon,       {.i = +1 } },
	{ MODKEY|SHIFT,     XK_comma,								 tagmon,         {.i = -1 } },
	{ MODKEY|SHIFT,     XK_period,							 tagmon,         {.i = +1 } },
	{ MODKEY,           XK_q,                    quit,           {1} },
	{ MODKEY|SHIFT,     XK_q,                    quit,           {0} },
	TAGKEYS(XK_1, 0)
	TAGKEYS(XK_2, 1)
	TAGKEYS(XK_3, 2)
	TAGKEYS(XK_4, 3)
	TAGKEYS(XK_5, 4)
	TAGKEYS(XK_6, 5)
	TAGKEYS(XK_7, 6)
	TAGKEYS(XK_8, 7)
	TAGKEYS(XK_9, 8)
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};
