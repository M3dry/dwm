/* See LICENSE file for copyright and license details. */

/* appearance */
/*  Display modes of the tab bar: never shown, always shown, shown only in  */
/*  monocle mode in the presence of several windows.                        */
/*  Modes after showtab_nmodes are disabled.                                */
enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab			     = showtab_auto; /* Default tab bar show mode */
static const int toptab				     = 0;   /* False means bottom tab bar */
static const unsigned int borderpx       = 2;   /* border pixel of windows */
static const unsigned int snap           = 0;   /* snap pixel */
static const int swallowfloating         = 1;   /* 1 means swallow floating windows by default */
static const int decorhints              = 1;   /* 1 means respect decoration hints */
static const unsigned int gappih         = 5;   /* horiz inner gap between windows */
static const unsigned int gappiv         = 5;   /* vert inner gap between windows */
static const unsigned int gappoh         = 0;   /* horiz outer gap between windows and screen edge */
static const unsigned int gappov         = 0;   /* vert outer gap between windows and screen edge */
static       int smartgaps               = 1;   /* 1 means no outer gap when there is only one window */
static const int focusonwheel            = 0;
static const int showbar                 = 1;   /* 0 means no bar */
static const int topbar                  = 1;   /* 0 means bottom bar */
static const int horizpadbar             = 8;   /* horizontal padding for statusbar */
static const int vertpadbar              = 0;   /* vertical padding for statusbar */
static const int user_bh                 = 24;  /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const unsigned int ulinepad	     = 2;   /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	 = 2;   /* thickness / height of the underline */
static const unsigned int ulinevoffset	 = 0;   /* how far above the bottom of the bar the line should appear */
static const unsigned int systraypinning = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing = 2;   /* systray spacing */
static const int systraypinningfailfirst = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray             = 1;   /* 0 means no systray */
static const char *fonts[]               = { "mononoki Nerd Font Mono:size=12:antialias=true:autohint=true" };

static const char normfg[]                = "#5e5e5e";
static const char selfg[]                 = "#51afef";
static const char normbg[]                = "#111111";
static const char selbg[]                 = "#111111";
static const char normfloatwinborder[]    = "#000000";
static const char selfloatwinborder[]     = "#ffffff";
static const char normstickyborder[]      = "#000000";
static const char selstickyborder[]       = "#98be65";
static const char normstickyfloatborder[] = "#000000";
static const char selstickyfloatborder[]  = "#8acc35";
static const char normfakefullscr[]       = "#408ab2";
static const char selfakefullscr[]        = "#b869e5";
static const char normfakefullscrfloat[]  = "#289fe0";
static const char selfakefullscrfloat[]   = "#9b1be5";
/* Occupied */
static const char occupiedfg[]            = "#ffffff";
static const char occupiedbg[]            = "#111111";
static const char ocinvfg[]               = "#000000";
static const char ocinvbg[]               = "#f0f0f0";
/* Status */
static const char statusfg[]              = "#f0f0f0";
static const char statusbg[]              = "#111111";
/* Tab Bar */
static const char normtabfg[]             = "#ffffff";
static const char seltabfg[]              = "#3071db";
static const char normtabbg[]             = "#111111";
static const char seltabbg[]              = "#111111";
/* Client Indicators */
static const char selindfg[]              = "#51afef";
static const char normindfg[]             = "#ffffff";
static const char incindfg[]              = "#8acc35";
/* Inverter mon */
static const char invmonbg[]              = "#3071db";
static const char invmonfg[]              = "#ffffff";
/* Lay constout borders */
static const char normtileborder[]        = "#292d3e";
static const char normfibonacciborder[]   = "#292d3e";
static const char normfloatborder[]       = "#292d3e";
static const char normdeckborder[]        = "#292d3e";
static const char normnrowgridborder[]    = "#292d3e";
static const char normbstackborder[]      = "#292d3e";
static const char normcenmasterborder[]   = "#292d3e";
static const char normmonocleborder[]     = "#292d3e";
static const char normgaplessgridborder[] = "#292d3e";
static const char seltileborder[]         = "#ff6c6b";
static const char selfibonacciborder[]    = "#ff6c6b";
static const char selfloatborder[]        = "#16cc31";
static const char seldeckborder[]         = "#ff6c6b";
static const char selnrowgridborder[]     = "#ff6c6b";
static const char selbstackborder[]       = "#c678dd";
static const char selcenmasterborder[]    = "#ff6c6b";
static const char selmonocleborder[]      = "#ff6c6b";
static const char selgaplessgridborder[]  = "#ff6c6b";

static const char *colors[][10]  = {
	/* Tags/borders       fg            bg      float               sticky            sticky + float         fakefullscreen   fakefullscreen + float */
	[SchemeNorm]        = { normfg,     normbg, normfloatwinborder, normstickyborder, normstickyfloatborder, normfakefullscr, normfakefullscrfloat },
	[SchemeSel]         = { selfg,      selbg,  selfloatwinborder,  selstickyborder,  selstickyfloatborder,  selfakefullscr,  selfakefullscrfloat },
	[SchemeOccupied]    = { occupiedfg, occupiedbg }, /* Tags that have 1 of more windowns */
	[SchemeOccupiedInv] = { ocinvfg,    ocinvbg }, /* Tags that have 1 of more windowns */
	[SchemeStatus]      = { statusfg,   statusbg }, /* Status / ltsymbol */
	[SchemeTabNorm]     = { normtabfg,  normtabbg }, /* Tabs */
	[SchemeTabSel]      = { seltabfg,   seltabbg}, /* Tabs selected */
	[SchemeClientSel]   = { selindfg }, /* Client indicators */
	[SchemeClientNorm]  = { normindfg }, /* Client indicators unfocused */
	[SchemeClientInc]   = { incindfg }, /* Client indicators unfocused */
	[SchemeInvMon]      = { invmonfg,    invmonbg }, /* Unfocused monitors */
	/* Win borders          tile            fibonacci            float            deck            nrowgrid            bstack            centeredmaster       monocle            gaplessgrid */
	[SchemeNormLayout]  = { normtileborder, normfibonacciborder, normfloatborder, normdeckborder, normnrowgridborder, normbstackborder, normcenmasterborder, normmonocleborder, normgaplessgridborder },
	[SchemeSelLayout]   = { seltileborder,  selfibonacciborder,  selfloatborder,  seldeckborder,  selnrowgridborder,  selbstackborder,  selcenmasterborder,  selmonocleborder,  selgaplessgridborder },
    /* if you change layout order you also need to change the order here */
};

typedef struct {
	const char *name;
	const void *cmd;
} Sp;

const char *spcmd1[] = {"st", "-c", "spterm", "-t", "stSCP", "-g", "144x41", NULL };
const char *spcmd2[] = {"st", "-c", "spmus", "-t", "cmusSCP", "-g", "144x41", "-e", "cmus", NULL };
const char *spcmd3[] = {"qalculate-gtk", "--title", "spcal", NULL };
static Sp scratchpads[] = {
   /* name          cmd  */
   {"spterm",      spcmd1},
   {"spmus",       spcmd2},
   {"spcal",       spcmd3},
};

/* tagging */
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

#define WTYPE "_NET_WM_WINDOW_TYPE_"
static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
     *	_NET_WM_WINDOW_TYPE(ATOM) = wintype
	 */
	/* class      instance    title          wintype    tags mask     switchtotag     iscentered   isfloating   ispermanent   isterminal    noswallow   monitor */
    /* Scratchpads */
	{ "spterm",	  NULL,	      NULL,	         NULL,      SPTAG(0),	  0,              1,           1,			0,            0,            0,          -1 }, /* St */
	{ "spmus",	  NULL,	      NULL,	         NULL,      SPTAG(1),	  0,              1,           1,			0,            0,            0,          -1 }, /* cmus */
	{ NULL,		  NULL,	      "spcal",	     NULL,      SPTAG(2),	  0,              1,           1,			0,            0,            0,          -1 }, /* qalculate-gtk */
    /* Terminals */
	{ "St",       NULL,       NULL,          NULL,      0,            0,              0,           0,           0,            1,            0,          -1 },
	{ "Alacritty",NULL,       NULL,          NULL,      0,            0,              0,           0,           0,            1,            0,          -1 },
	{ "XTerm",    NULL,       NULL,          NULL,      0,            0,              0,           0,           0,            1,            0,          -1 },
    /* Noswallow */
	{ NULL,       "Navigator",NULL,          NULL,      1,            1,              0,           0,           1,            0,            1,          -1 }, /* firefox */
	{ NULL,       "chromium", NULL,          NULL,      1 << 3,       1,              0,           0,           1,            0,            1,          -1 }, /* chromium */
	{ NULL,       NULL,       "Event Tester",NULL,      0,            0,              0,           0,           0,            0,            1,          -1 }, /* xev */
	{ "Xephyr",   NULL,       NULL,          NULL,      0,            0,              1,           1,           0,            0,            1,          -1 }, /* xephyr */
	{ "Gimp",     NULL,       NULL,          NULL,      1 << 8,       3,              1,           1,           0,            0,            1,          -1 }, /* gimp */
	{ NULL,       NULL,       "glxgears",    NULL,      0,            0,              1,           1,           0,            0,            1,          -1 },
    /* Wintype */
	{ NULL,       NULL,       NULL, WTYPE "DIALOG",     0,            0,              1,           1,           0,            0,            0,          -1 },
	{ NULL,       NULL,       NULL, WTYPE "UTILITY",    0,            0,              1,           1,           0,            0,            0,          -1 },
	{ NULL,       NULL,       NULL, WTYPE "TOOLBAR",    0,            0,              1,           1,           0,            0,            0,          -1 },
	{ NULL,       NULL,       NULL, WTYPE "SPLASH",     0,            0,              1,           1,           0,            0,            0,          -1 },
};

static const MonitorRule monrules[] = {
	/* monitor  tag  layout  mfact  nmaster  showbar  topbar */
	{  1,       -1,  5,      -1,    -1,      -1,      -1     }, // use a different layout for the second monitor
	{  -1,      -1,  0,      -1,    -1,      -1,      -1     }, // default
};

/* layout(s) */
static const float mfact     = 0.5;  /* factor of master area size [0.05..0.95] */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int nmaster     = 1;    /* number of clients in master area */
static const int attachbelow = 1;    /* 1 means attach after the currently active window */

#define FORCE_VSPLIT 1  /* nrowgrid layout: force two clients to always split vertically */

#include <X11/XF86keysym.h>
#include "vanitygaps.c"

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "(@)",      spiral },
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[D]",      deck },
	{ "###",      nrowgrid },
	{ "TTT",      bstack },
	{ "|M|",      centeredmaster },
	{ "[M]",      monocle },
	{ "HHH",      gaplessgrid },
	{ NULL,       NULL },
};

/* key definitions */
#define M Mod4Mask
#define A Mod1Mask
#define S ShiftMask

#define C ControlMask
#define TAGKEYS(CHAIN,KEY,TAG) \
	{ A,       CHAIN,   KEY,   comboview,    {.ui = 1 << TAG} }, \
	{ C,       CHAIN,   KEY,   toggleview,   {.ui = 1 << TAG} }, \
	{ M,       CHAIN,   KEY,   toggletag,    {.ui = 1 << TAG} }, \
	{ A|S,     CHAIN,   KEY,   combotag,     {.ui = 1 << TAG} }, \
	{ A|C,     CHAIN,   KEY,   tagwith,      {.ui = 1 << TAG} }, \
	{ M|S,     CHAIN,   KEY,   swaptags,     {.ui = 1 << TAG} }, \
	{ A|M,     CHAIN,   KEY,   tagnextmon,   {.ui = 1 << TAG} }, \
	{ A|M|S,   CHAIN,   KEY,   tagprevmon,   {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }


static Key keys[] = {
	/* modifier     chain   key            function                argument */
    /* Spawing preograms */
	{ A,            -1,     XK_Return,     spawn,                  SHCMD("$TERMINAL") },
	{ A|S,          -1,     XK_c,          spawn,                  SHCMD("$TERMINAL htop") },
	{ A|S,          -1,     XK_z,          spawn,                  SHCMD("playerctl play-pause") },
	{ A,            -1,     XK_e,          spawn,                  SHCMD("$TERMINAL $EDITOR") },
	{ M,            -1,     XK_e,          spawn,                  SHCMD("emacsclient -c -a emacs") },
	{ M|S,          -1,     XK_c,          spawn,                  SHCMD("emacsclient -c -a e'macs' --eval '(ibuffer)'") },
	{ A,            -1,     XK_w,          spawn,                  SHCMD("xdo activate -N LibreWolf || librewolf") },
	{ A,            -1,     XK_o,          spawn,                  SHCMD("chromium") },
	{ A,            -1,     XK_Escape,     spawn,                  SHCMD("xkill") },
	{ C|A,          -1,     XK_d,          spawn,                  SHCMD("discord") },
	{ A|S,          -1,     XK_u,          spawn,                  SHCMD("import my-stuff/Pictures/snips/$(date +'%H:%M:%S').png") },
	{ A,            -1,     XK_p,          spawn,                  SHCMD("pcmanfm") },
	{ A,            -1,     XK_a,          spawn,                  SHCMD("$TERMINAL vifmrun") },
	{ C,            -1,     XK_m,          spawn,                  SHCMD("multimc") },
	{ M|C|A,        -1,     XK_l,          spawn,                  SHCMD("slock") },
	{ C|A,          -1,     XK_z,          spawn,                  SHCMD("playerctl play-pause") },
    /* Dmenu scripts */
	{ A|S,          -1,     XK_Return,     spawn,                  SHCMD("dmenu_run -l 5 -g 10 -p 'Run:'") },
	{ A,            -1,     XK_c,          spawn,                  SHCMD("volume-script") },
	{ A|C,          -1,     XK_Return,     spawn,                  SHCMD("Booky 'st nvim' '><' 'Cconfig'") },
	{ A|S,          -1,     XK_w,          spawn,                  SHCMD("Booky 'librewolf' ':' 'Bconfig'") },
	{ A,            -1,     XK_z,          spawn,                  SHCMD("music-changer cmus") },
	{ A|S,          XK_d,   XK_s,          spawn,                  SHCMD("switch") },
	{ A|S,          XK_d,   XK_e,          spawn,                  SHCMD("emoji-script") },
	{ A|S,          XK_d,   XK_c,          spawn,                  SHCMD("calc") },
	{ A|S,          XK_d,   XK_p,          spawn,                  SHCMD("passmenu2 -F -p 'Passwords:'") },
	{ A|S,          XK_d,   XK_v,          spawn,                  SHCMD("manview") },
	{ A|S,          XK_d,   XK_a,          spawn,                  SHCMD("allmenu") },
	{ A|S,          XK_d,   XK_q,          spawn,                  SHCMD("shut") },
    /* MultiMedia keys */
	{ 0,-1, XF86XK_AudioPrev,              spawn,                  SHCMD("playerctl --player cmus previous") },
	{ 0,-1, XF86XK_AudioNext,              spawn,                  SHCMD("playerctl --player cmus next") },
	{ 0,-1, XF86XK_AudioPlay,              spawn,                  SHCMD("playerctl --player cmus play-pause") },
	{ 0,-1, XF86XK_AudioLowerVolume,       spawn,                  SHCMD("pamixer --allow-boost -d 1 ; killall dwmStatus && dwmStatus &") },
	{ 0,-1, XF86XK_AudioRaiseVolume,       spawn,                  SHCMD("pamixer --allow-boost -i 1 ; killall dwmStatus && dwmStatus &") },
    /* DWM keybindings */
	{ A,            -1,     XK_q,          killclient,             {0} },
	{ A|C|S,        -1,     XK_x,          killpermanent,          {0} },
	{ A|S,          -1,     XK_q,          killunsel,              {0} },
	{ A,            -1,     XK_n,          togglebar,              {0} },
	{ A|S,          -1,     XK_h,          setmfact,               {.f = -0.05} },
	{ A|S,          -1,     XK_l,          setmfact,               {.f = +0.05} },
	{ A|S,          -1,     XK_j,          setcfact,               {.f = +0.25} },
	{ A|S,          -1,     XK_k,          setcfact,               {.f = -0.25} },
	{ A|C,          -1,     XK_u,          setcfact,               {0} },
	{ A,            -1,     XK_bracketleft,incnmaster,             {.i = +1 } },
	{ A,            -1,     XK_bracketright,incnmaster,            {.i = -1 } },
	{ M,            -1,     XK_space,      focusmaster,            {0} },
	{ A|C,          -1,     XK_space,      switchcol,              {0} },
	{ A,            -1,     XK_h,          focusdir,               {.i = 0 } }, // left
	{ A,            -1,     XK_l,          focusdir,               {.i = 1 } }, // right
	{ A,            -1,     XK_k,          focusdir,               {.i = 2 } }, // up
	{ A,            -1,     XK_j,          focusdir,               {.i = 3 } }, // down
	{ M|S,          -1,     XK_j,          focusstack,             {.i = +1 } },
	{ M|S,          -1,     XK_k,          focusstack,             {.i = -1 } },
	{ M|C,          -1,     XK_j,          inplacerotate,          {.i = +2 } },
	{ M|C,          -1,     XK_k,          inplacerotate,          {.i = -2 } },
    /* Layouts */
	{ A,            -1,     XK_t,          setlayout,              {.v = &layouts[0]} },
	{ A,            -1,     XK_v,          setlayout,              {.v = &layouts[1]} },
	{ A|S,          -1,     XK_f,          setlayout,              {.v = &layouts[2]} },
	{ A,            -1,     XK_d,          setlayout,              {.v = &layouts[3]} },
	{ A,            -1,     XK_g,          setlayout,              {.v = &layouts[4]} },
	{ A,            -1,     XK_b,          setlayout,              {.v = &layouts[5]} },
	{ A|S,          -1,     XK_m,          setlayout,              {.v = &layouts[6]} },
	{ A,            -1,     XK_m,          setlayout,              {.v = &layouts[7]} },
	{ A|S,          -1,     XK_g,          setlayout,              {.v = &layouts[8]} },
	{ A|S,          -1,     XK_t,          tabmode,                {-1} },
	{ A|C,          -1,     XK_i,          cyclelayout,            {.i = -1 } },
	{ A|C,          -1,     XK_p,          cyclelayout,            {.i = +1 } },
	{ A,            -1,     XK_0,          view,                   {.ui = ~0 } },
	{ A,            -1,     XK_Tab,        goback,                 {0} },
	{ A|S,          -1,     XK_n,          shiftviewclients,       { .i = +1 } },
	{ A|S,          -1,     XK_p,          shiftviewclients,       { .i = -1 } },
	{ A|S,          -1,     XK_a,          winview,                {0} },
    /* Window manipulation */
	{ A,            -1,     XK_semicolon,  zoom,                   {0} },
	{ A|S,          -1,     XK_v,          transfer,               {0} },
	{ M,            -1,     XK_j,          pushdown,               {0} },
	{ M,            -1,     XK_k,          pushup,                 {0} },
	{ A,            -1,     XK_space,      togglefloating,         {0} },
	{ A|S,          -1,     XK_space,      unfloatvisible,         {0} },
	{ M,            -1,     XK_s,          togglesticky,           {0} },
	{ A,            -1,     XK_f,          togglefullscr,          {0} },
	{ A|C,          -1,     XK_f,          togglefakefullscreen,   {0} },
	{ A,            -1,     XK_u,          togglescratch,          {.ui = 0 } },
    { A,            -1,     XK_i,          togglescratch,          {.ui = 1 } },
    { A,            -1,     XK_y,          togglescratch,          {.ui = 2 } },
    /* Monitors */
	{ A,            -1,     XK_comma,      focusmon,               {.i = -1 } },
	{ A,            -1,     XK_period,     focusmon,               {.i = +1 } },
	{ A|S,          -1,     XK_comma,      tagmon,                 {.i = -1 } },
	{ A|S,          -1,     XK_period,     tagmon,                 {.i = +1 } },
    /* Moveresize */
	{ A|C,          -1,     XK_j,          moveresize,             {.v = "0x 25y 0w 0h" } },
	{ A|C,          -1,     XK_k,          moveresize,             {.v = "0x -25y 0w 0h" } },
	{ A|C,          -1,     XK_l,          moveresize,             {.v = "25x 0y 0w 0h" } },
	{ A|C,          -1,     XK_h,          moveresize,             {.v = "-25x 0y 0w 0h" } },
	{ M|C,          -1,     XK_j,          moveresize,             {.v = "0x 0y 0w 25h" } },
	{ M|C,          -1,     XK_k,          moveresize,             {.v = "0x 0y 0w -25h" } },
	{ M|C,          -1,     XK_l,          moveresize,             {.v = "0x 0y 25w 0h" } },
	{ M|C,          -1,     XK_h,          moveresize,             {.v = "0x 0y -25w 0h" } },
    /* Gaps */
	{ A|S,          -1,     XK_equal,      incrgaps,               {.i = +1 } },
	{ A|S,          -1,     XK_minus,      incrgaps,               {.i = -1 } },
	{ A|S,          -1,     XK_0,          defaultgaps,            {0} },
	{ A|C,          -1,     XK_0,          togglegaps,             {0} },
    /* Borderpx */
	{ A|C,          -1,     XK_equal,      setborderpx,            {.i = +1 } },
	{ A|C,          -1,     XK_minus,      setborderpx,            {.i = -1 } },
	{ M,            -1,     XK_0,          setborderpx,            {.i = 0 } },
	TAGKEYS(        -1,     XK_1,                                  0)
	TAGKEYS(        -1,     XK_2,                                  1)
	TAGKEYS(        -1,     XK_3,                                  2)
	TAGKEYS(        -1,     XK_4,                                  3)
	TAGKEYS(        -1,     XK_5,                                  4)
	TAGKEYS(        -1,     XK_6,                                  5)
	TAGKEYS(        -1,     XK_7,                                  6)
	TAGKEYS(        -1,     XK_8,                                  7)
	TAGKEYS(        -1,     XK_9,                                  8)
	{ M|S,          -1,     XK_Escape,     quit,                   {0} }, 
	{ A|C|S,        -1,     XK_q,          quit,                   {1} }, 
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkClientWin,         A,              Button1,        movemouse,      {0} },
	{ ClkClientWin,         A,              Button2,        togglefloating, {0} },
	{ ClkClientWin,         A,              Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            A,              Button1,        tag,            {0} },
	{ ClkTagBar,            A,              Button3,        toggletag,      {0} },
	{ ClkTabBar,            0,              Button1,        focuswin,       {0} },
};
