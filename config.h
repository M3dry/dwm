enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab                   = showtab_auto; /* Default tab bar show mode */
static const int toptab                    = 0;   /* False means bottom tab bar */

static const int showbar                   = 1;   /* 0 means no bar */

static const int topbar                    = 1;   /* 0 means bottom bar */

static const int horizpadbar               = 0;   /* horizontal padding for statusbar */
static const int vertpadbar                = 0;   /* vertical padding for statusbar */

static const int user_bh                   = 24;  /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */

static const unsigned int systraypinning   = 0;   /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing   = 2;   /* systray spacing */
static const int systraypinningfailfirst   = 1;   /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray               = 1;   /* 0 means no systray */

static       unsigned int vacanttags       = 1;   /* 0 means no vacant tags */

static const unsigned int ulinepad         = 2;   /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke      = 2;   /* thickness / height of the underline */
static const unsigned int ulinevoffset     = 0;   /* how far above the bottom of the bar the line should appear */

static const unsigned int underlinetags    = 0;   /* 0 means no underline */
static const unsigned int underlinevacant  = 0;   /* 0 means no underline for vacant tags */

static const unsigned int gappih           = 5;   /* horiz inner gap between windows */
static const unsigned int gappiv           = 5;   /* vert inner gap between windows */
static const unsigned int gappoh           = 0;   /* horiz outer gap between windows and screen edge */
static const unsigned int gappov           = 0;   /* vert outer gap between windows and screen edge */
static       int smartgaps                 = 1;   /* 1 means no outer gap when there is only one window */

static const int swallowfloating           = 1;   /* 1 means swallow floating windows by default */

static const unsigned int borderpx         = 2;   /* border pixel of windows */

static const unsigned int snap             = 0;   /* snap pixel */

static const int startontag                = 1;   /* 0 means no tag active on start */

static const int decorhints                = 1;   /* 1 means respect decoration hints */

static const int focusonwheel              = 0;

static const char *fonts[]                 = { "mononoki Nerd Font Mono:size=12:antialias=true:autohint=true" };

static const char normfg[]                = "#4E5579";
static const char selfg[]                 = "#ff5370";
static const char normbg[]                = "#1E1C31";
static const char selbg[]                 = "#1E1C31";

static const char invmonbg[]              = "#3071db";
static const char invmonfg[]              = "#ffffff";

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

static const char occupiedfg[]            = "#7986E7";
static const char occupiedbg[]            = "#1E1C31";
static const char ocinvfg[]               = "#000000";
static const char ocinvbg[]               = "#f0f0f0";

static const char statusfg[]              = "#7986E7";
static const char statusbg[]              = "#1E1C31";

static const char ltsymbolfg[]            = "#ff5370";
static const char ltsymbolbg[]            = "#1E1C31";

static const char normtabfg[]             = "#4E5579";
static const char seltabfg[]              = "#7986E7";
static const char normtabbg[]             = "#1E1C31";
static const char seltabbg[]              = "#1E1C31";

static const char selindfg[]              = "#ff5370";
static const char normindfg[]             = "#7986E7";
static const char incindfg[]              = "#7986E7";

static const char normtileborder[]        = "#1E1C31";
static const char normfibonacciborder[]   = "#1E1C31";
static const char normfloatborder[]       = "#1E1C31";
static const char normdeckborder[]        = "#1E1C31";
static const char normnrowgridborder[]    = "#1E1C31";
static const char normbstackborder[]      = "#1E1C31";
static const char normcenmasterborder[]   = "#1E1C31";
static const char normmonocleborder[]     = "#1E1C31";
static const char normgaplessgridborder[] = "#1E1C31";
static const char seltileborder[]         = "#ff5370";
static const char selfibonacciborder[]    = "#ff5370";
static const char selfloatborder[]        = "#16cc31";
static const char seldeckborder[]         = "#ff5370";
static const char selnrowgridborder[]     = "#ff5370";
static const char selbstackborder[]       = "#c678dd";
static const char selcenmasterborder[]    = "#ff5370";
static const char selmonocleborder[]      = "#ff5370";
static const char selgaplessgridborder[]  = "#ff5370";

static const char *colors[][10]  = {
    /* Tags/borders       fg            bg      float               sticky            sticky + float         fakefullscreen   fakefullscreen + float */
    [SchemeNorm]        = { normfg,     normbg, normfloatwinborder, normstickyborder, normstickyfloatborder, normfakefullscr, normfakefullscrfloat },
    [SchemeSel]         = { selfg,      selbg,  selfloatwinborder,  selstickyborder,  selstickyfloatborder,  selfakefullscr,  selfakefullscrfloat },
    [SchemeOccupied]    = { occupiedfg, occupiedbg },
    [SchemeOccupiedInv] = { ocinvfg,    ocinvbg },
    [SchemeStatus]      = { statusfg,   statusbg },
    [SchemeLtsymbol]    = { ltsymbolfg, ltsymbolbg },
    [SchemeTabNorm]     = { normtabfg,  normtabbg },
    [SchemeTabSel]      = { seltabfg,   seltabbg},
    [SchemeClientSel]   = { selindfg },
    [SchemeClientNorm]  = { normindfg },
    [SchemeClientInc]   = { incindfg },
    [SchemeInvMon]      = { invmonfg,    invmonbg },
    /* Win borders          tile            fibonacci            float            deck            nrowgrid            bstack            centeredmaster       monocle            gaplessgrid */
    [SchemeNormLayout]  = { normtileborder, normfibonacciborder, normfloatborder, normdeckborder, normnrowgridborder, normbstackborder, normcenmasterborder, normmonocleborder, normgaplessgridborder },
    [SchemeSelLayout]   = { seltileborder,  selfibonacciborder,  selfloatborder,  seldeckborder,  selnrowgridborder,  selbstackborder,  selcenmasterborder,  selmonocleborder,  selgaplessgridborder },
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

static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const char ptagf[] = "[%s:%s]"; /* format of a tag label */
static const char etagf[] = "%s";    /* format of an empty tag */
static const int lcaselbl = 0;         /* 1 means make tag label lowercase */

#define WTYPE "_NET_WM_WINDOW_TYPE_"
static const Rule rules[] = {
    /* xprop(1):
     *  WM_CLASS(STRING) = instance, class
     *  WM_NAME(STRING) = title
     *  _NET_WM_WINDOW_TYPE(ATOM) = wintype
     */
    /* class      instance    title          wintype    tags mask     switchtotag     isfloating   ispermanent   isterminal    noswallow   monitor  xkb layout*/
    /* Scratchpads */
    { "spte rm",  NULL,       NULL,          NULL,      SPTAG(0),     0,              1,           0,            0,            0,          -1,      0 }, /* St */
    { "spmus",    NULL,       NULL,          NULL,      SPTAG(1),     0,              1,           0,            0,            0,          -1,      0 }, /* cmus */
    { NULL,       NULL,       "spcal",       NULL,      SPTAG(2),     0,              1,           0,            0,            0,          -1,      0 }, /* qalculate-gtk */
    /* Terminals */
    { "St",       NULL,       NULL,          NULL,      0,            0,              0,           0,            1,            0,          -1,      0 },
    { "Alacritty",NULL,       NULL,          NULL,      0,            0,              0,           0,            1,            0,          -1,      0 },
    { "XTerm",    NULL,       NULL,          NULL,      0,            0,              0,           0,            1,            0,          -1,      0 },
    /* Noswallow */
    { NULL,       "Navigator",NULL,          NULL,      1,            1,              0,           1,            0,            1,          -1,      0 }, /* firefox */
    { NULL,       "chromium", NULL,          NULL,      1 << 3,       1,              0,           1,            0,            1,          -1,      1 }, /* chromium */
    { NULL,       NULL,       "Event Tester",NULL,      0,            0,              0,           0,            0,            1,          -1,      0 }, /* xev */
    { "Xephyr",   NULL,       NULL,          NULL,      0,            0,              1,           0,            0,            1,          -1,      0 }, /* xephyr */
    { "Gimp",     NULL,       NULL,          NULL,      1 << 8,       3,              1,           0,            0,            1,          -1,      0 }, /* gimp */
    { NULL,       NULL,       "glxgears",    NULL,      0,            0,              1,           0,            0,            1,          -1,      0 },
    /* Wintype */
    { NULL,       NULL,       NULL, WTYPE "DIALOG",     0,            0,              1,           0,            0,            0,          -1,      0 },
    { NULL,       NULL,       NULL, WTYPE "UTILITY",    0,            0,              1,           0,            0,            0,          -1,      0 },
    { NULL,       NULL,       NULL, WTYPE "TOOLBAR",    0,            0,              1,           0,            0,            0,          -1,      0 },
    { NULL,       NULL,       NULL, WTYPE "SPLASH",     0,            0,              1,           0,            0,            0,          -1,      0 },
};

static const MonitorRule monrules[] = {
   /* monitor  tag  layout  mfact  nmaster  showbar  topbar */
   {  1,       -1,  5,      -1,    -1,      -1,      -1     }, // use a different layout for the second monitor
   {  -1,      -1,  0,      -1,    -1,      -1,      -1     }, // default
};

static const char *xkb_layouts [] = {
    "en",
    "cz",
};

static const float mfact     = 0.5;
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int nmaster     = 1;
static const int attachbelow = 1;
#define FORCE_VSPLIT 1
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

#include <X11/XF86keysym.h>

#define M Mod4Mask
#define A Mod1Mask
#define S ShiftMask
#define C ControlMask

#define TAGKEYS(KEY,TAG) \
    { A,       -1,   KEY,   comboview,    {.ui = 1 << TAG} }, \
    { C,       -1,   KEY,   toggleview,   {.ui = 1 << TAG} }, \
    { M,       -1,   KEY,   toggletag,    {.ui = 1 << TAG} }, \
    { A|S,     -1,   KEY,   combotag,     {.ui = 1 << TAG} }, \
    { A|C,     -1,   KEY,   tagwith,      {.ui = 1 << TAG} }, \
    { M|S,     -1,   KEY,   swaptags,     {.ui = 1 << TAG} }, \
    { C|M,     XK_l, KEY,   focusnextmon, {.ui = 1 << TAG} }, \
    { C|M,     XK_h, KEY,   focusprevmon, {.ui = 1 << TAG} }, \
    { A|M,     XK_l, KEY,   tagnextmon,   {.ui = 1 << TAG} }, \
    { A|M,     XK_h, KEY,   tagprevmon,   {.ui = 1 << TAG} },

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

static Key keys[] = {
    { A,            -1,     XK_Return,     spawn,                  SHCMD("$TERMINAL") },
    { A|S,          -1,     XK_c,          spawn,                  SHCMD("$TERMINAL htop") },
    { A|S,          -1,     XK_z,          spawn,                  SHCMD("playerctl play-pause") },
    { A|S,          -1,     XK_e,          spawn,                  SHCMD("$TERMINAL $EDITOR") },
    { A,            XK_e,   XK_e,          spawn,                  SHCMD("emacsclient -c -a emacs") },
    { A,            XK_e,   XK_c,          spawn,                  SHCMD("emacsclient -c -e '(ibuffer)'") },
    { A,            XK_e,   XK_d,          spawn,                  SHCMD("emacsclient -c -e '(dired nil)'") },
    { A,            XK_e,   XK_f,          spawn,                  SHCMD("emacsclient -c -e '(elfeed)'") },
    { A,            -1,     XK_w,          spawn,                  SHCMD("xdo activate -N LibreWolf || librewolf") },
    { A,            -1,     XK_o,          spawn,                  SHCMD("xdo activate -N Chromium || chromium") },
    { A|C,          -1,     XK_KP_Down,    spawn,                  SHCMD("xkill") },
    { C|A,          -1,     XK_d,          spawn,                  SHCMD("discord") },
    { A|S,          -1,     XK_u,          spawn,                  SHCMD("import my-stuff/Pictures/snips/$(date +'%H:%M:%S').png") },
    { A,            -1,     XK_p,          spawn,                  SHCMD("pcmanfm") },
    { A,            -1,     XK_a,          spawn,                  SHCMD("$TERMINAL vifmrun") },
    { C,            -1,     XK_m,          spawn,                  SHCMD("multimc") },
    { M|C|A,        -1,     XK_l,          spawn,                  SHCMD("slock") },
    { C|A,          -1,     XK_z,          spawn,                  SHCMD("playerctl play-pause") },

    { A|S,          -1,     XK_Return,     spawn,                  SHCMD("dmenu_run -l 5 -g 10 -p 'Run:'") },
    { A,            -1,     XK_c,          spawn,                  SHCMD("volume-script") },
    { A|C,          -1,     XK_Return,     spawn,                  SHCMD("Booky 'emacsclient -c -a emacs' '><' 'Cconfig'") },
    { A|S,          -1,     XK_w,          spawn,                  SHCMD("Booky 'librewolf' ':' 'Bconfig'") },
    { A,            -1,     XK_z,          spawn,                  SHCMD("music-changer cmus") },
    { A|S,          XK_d,   XK_s,          spawn,                  SHCMD("switch") },
    { A|S,          XK_d,   XK_e,          spawn,                  SHCMD("emoji-script") },
    { A|S,          XK_d,   XK_c,          spawn,                  SHCMD("calc") },
    { A|S,          XK_d,   XK_p,          spawn,                  SHCMD("passmenu2 -F -p 'Passwords:'") },
    { A|S,          XK_d,   XK_v,          spawn,                  SHCMD("manview") },
    { A|S,          XK_d,   XK_a,          spawn,                  SHCMD("allmenu") },
    { A|S,          XK_d,   XK_q,          spawn,                  SHCMD("shut") },

    { 0,-1, XF86XK_AudioPrev,              spawn,                  SHCMD("playerctl --player cmus previous") },
    { 0,-1, XF86XK_AudioNext,              spawn,                  SHCMD("playerctl --player cmus next") },
    { 0,-1, XF86XK_AudioPlay,              spawn,                  SHCMD("playerctl --player cmus play-pause") },
    { 0,-1, XF86XK_AudioLowerVolume,       spawn,                  SHCMD("pamixer --allow-boost -d 1 ; killall dwmStatus && dwmStatus &") },
    { 0,-1, XF86XK_AudioRaiseVolume,       spawn,                  SHCMD("pamixer --allow-boost -i 1 ; killall dwmStatus && dwmStatus &") },

    { A,            -1,     XK_q,          killclient,             {0} },
    { A|C|S,        -1,     XK_x,          killpermanent,          {0} },
    { A|S,          -1,     XK_q,          killunsel,              {0} },
    { M|S,          -1,     XK_v,          togglevacant,           {0} },
    { A,            -1,     XK_n,          togglebar,              {0} },
    { A,            -1,     XK_r,          reorganizetags,         {0} },
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

    { A,            -1,     XK_comma,      focusmon,               {.i = -1 } },
    { A,            -1,     XK_period,     focusmon,               {.i = +1 } },
    { A|S,          -1,     XK_comma,      tagmon,                 {.i = -1 } },
    { A|S,          -1,     XK_period,     tagmon,                 {.i = +1 } },

    { A|C,          -1,     XK_j,          moveresize,             {.v = "0x 25y 0w 0h" } },
    { A|C,          -1,     XK_k,          moveresize,             {.v = "0x -25y 0w 0h" } },
    { A|C,          -1,     XK_l,          moveresize,             {.v = "25x 0y 0w 0h" } },
    { A|C,          -1,     XK_h,          moveresize,             {.v = "-25x 0y 0w 0h" } },
    { M|C,          -1,     XK_j,          moveresize,             {.v = "0x 0y 0w 25h" } },
    { M|C,          -1,     XK_k,          moveresize,             {.v = "0x 0y 0w -25h" } },
    { M|C,          -1,     XK_l,          moveresize,             {.v = "0x 0y 25w 0h" } },
    { M|C,          -1,     XK_h,          moveresize,             {.v = "0x 0y -25w 0h" } },

    { A|S,          -1,     XK_equal,      incrgaps,               {.i = +1 } },
    { A|S,          -1,     XK_minus,      incrgaps,               {.i = -1 } },
    { A|S,          -1,     XK_0,          defaultgaps,            {0} },
    { A|C,          -1,     XK_0,          togglegaps,             {0} },

    { A|C,          -1,     XK_equal,      setborderpx,            {.i = +1 } },
    { A|C,          -1,     XK_minus,      setborderpx,            {.i = -1 } },
    { M,            -1,     XK_0,          setborderpx,            {.i = 0 } },

    TAGKEYS(                XK_1,                                  0)
    TAGKEYS(                XK_2,                                  1)
    TAGKEYS(                XK_3,                                  2)
    TAGKEYS(                XK_4,                                  3)
    TAGKEYS(                XK_5,                                  4)
    TAGKEYS(                XK_6,                                  5)
    TAGKEYS(                XK_7,                                  6)
    TAGKEYS(                XK_8,                                  7)
    TAGKEYS(                XK_9,                                  8)

    { M|S,          -1,     XK_Escape,     quit,                   {0} },
    { A|C|S,        -1,     XK_q,          quit,                   {1} },
};

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
