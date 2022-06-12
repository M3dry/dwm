enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab                   = showtab_never; /* Default tab bar show mode */
static const int toptab                    = 0; /* False means bottom tab bar */
static       int showpreview               = 0; /* show tag preview */
static const int scalepreview              = 4; /* tag preview scaling */
static const int showbar                   = 1; /* 0 means no bar */
static const int topbar                    = 1; /* 0 means bottom bar */
static const int horizpadbar               = 0; /* horizontal padding for statusbar */
static const int vertpadbar                = 0; /* vertical padding for statusbar */
static const int vertpad                   = 0; /* vertical padding of bar */
static const int sidepad                   = 0; /* horizontal padding of bar */
static const int vertpadtoggle             = vertpad ? vertpad : 10; /* vertical padding of bar that's toggleable */
static const int sidepadtoggle             = sidepad ? sidepad : 10; /* horizontal padding of bar that's toggleable */
static const int user_bh                   = 24; /* 0 means that dwm will calculate bar height, >= 1 means dwm will user_bh as bar height */
static const unsigned int systraypinning   = 1;  /* 0: sloppy systray follows selected monitor, >0: pin systray to monitor X */
static const unsigned int systrayspacing   = 2;  /* systray spacing */
static const int systraypinningfailfirst   = 1;  /* 1: if pinning fails, display systray on the first monitor, False: display systray on the last monitor*/
static const int showsystray               = 1;  /* 0 means no systray */
static const unsigned int ulinepad         = 6;  /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke      = 2;  /* thickness / height of the underline */
static const unsigned int ulinevoffset     = 0;  /* how far above the bottom of the bar the line should appear */
static const unsigned int underlinetags    = 1;  /* 0 means no underline */
static const unsigned int underlinevacant  = 1;  /* 0 means no underline for vacant tags */
static const unsigned int gappih           = vertpad || sidepad ? vertpadtoggle : 0; /* horiz inner gap between windows */
static const unsigned int gappiv           = vertpad || sidepad ? vertpadtoggle : 0; /* vert inner gap between windows */
static const unsigned int gappoh           = vertpad            ? sidepadtoggle : 0; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov           = sidepad            ? sidepadtoggle : 0; /* vert outer gap between windows and screen edge */
static       unsigned int padding          = vertpad || sidepad ? 1 : 0;
static const int swallowfloating           = 1; /* 1 means swallow floating windows by default */
static unsigned int borderpx               = 1; /* border pixel of windows */
static unsigned int floatborderpx          = 2; /* border pixel of floating windows */
static const unsigned int snap             = 0; /* snap pixel */
static const int startontag                = 1; /* 0 means no tag active on start */
static const int decorhints                = 1; /* 1 means respect decoration hints */
static const int focusonwheel              = 0;
static const char *fonts[] = { "Operator Mono SSm Lig Book:size=12:antialias=true:autohint=true" };

static const char normfg[]                = "#4e5579";
static const char selfg[]                 = "#5fafff";
static const char normbg[]                = "#1e1c31";
static const char selbg[]                 = "#1e1c31";

static const char invfg[]                 = "#4e5579";
static const char invbg[]                 = "#1e1c31";

static const char normfloatwinborder[]    = "#0f111b";
static const char selfloatwinborder[]     = "#ff5370";
static const char normstickyborder[]      = "#0f111b";
static const char selstickyborder[]       = "#98be65";

static const char occupiedfg[]            = "#7986e7";
static const char occupiedbg[]            = "#1e1C31";

static const char statusfg[]              = "#7986e7";
static const char statusbg[]              = "#1e1c31";

static const char ltsymfg[]               = "#1e1c31";
static const char ltsymbg[]               = "#ff5370";

static const char invltsymfg[]            = "#4e5579";
static const char invltsymbg[]            = "#1e1c31";

static const char normtabfg[]             = "#4e5579";
static const char seltabfg[]              = "#7986e7";
static const char normtabbg[]             = "#1e1c31";
static const char seltabbg[]              = "#1e1c31";

static const char vacindfg[]              = "#4e5579";
static const char indfg[]                 = "#7986e7";

static const char numfg[]                 = "#7986e7";
static const char numbg[]                 = "#1e1c31";

static const char systraybg[]             = "#1e1c31";

static const char normtileborder[]        = "#1e1c31";
static const char normfibonacciborder[]   = "#1e1c31";
static const char normfloatborder[]       = "#1e1c31";
static const char normdeckborder[]        = "#1e1c31";
static const char normnrowgridborder[]    = "#1e1c31";
static const char normbstackborder[]      = "#1e1c31";
static const char normcenmasterborder[]   = "#1e1c31";
static const char normmonocleborder[]     = "#1e1c31";
static const char normgaplessgridborder[] = "#1e1c31";
static const char seltileborder[]         = "#5fafff";
static const char selfibonacciborder[]    = "#5fafff";
static const char selfloatborder[]        = "#5fafff";
static const char seldeckborder[]         = "#5fafff";
static const char selnrowgridborder[]     = "#5fafff";
static const char selbstackborder[]       = "#5fafff";
static const char selcenmasterborder[]    = "#5fafff";
static const char selmonocleborder[]      = "#5fafff";
static const char selgaplessgridborder[]  = "#5fafff";

static const char *colors[][10]  = {
    /* Tags/borders       fg            bg      float               sticky */
    [SchemeNorm]        = { normfg,     normbg, normfloatwinborder, normstickyborder },
    [SchemeSel]         = { selfg,      selbg,  selfloatwinborder,  selstickyborder },
    [SchemeInv]         = { invfg,      invbg },
    [SchemeOccupied]    = { occupiedfg, occupiedbg },
    [SchemeStatus]      = { statusfg,   statusbg },
    [SchemeLtsymbol]    = { ltsymfg,    ltsymbg },
    [SchemeTabNorm]     = { normtabfg,  normtabbg },
    [SchemeTabSel]      = { seltabfg,   seltabbg},
    [SchemeClientVac]   = { vacindfg },
    [SchemeClient]      = { indfg },
    [SchemeClientNum]   = { numfg,      numbg },
    [SchemeSystray]     = {             systraybg },
    [SchemeInvLtsymbol] = { invltsymfg, invltsymbg },
    /* Win borders          tile            fibonacci            float            deck            nrowgrid            bstack            centeredmaster       monocle            gaplessgrid */
    [SchemeNormLayout]  = { normtileborder, normfibonacciborder, normfloatborder, normdeckborder, normnrowgridborder, normbstackborder, normcenmasterborder, normmonocleborder, normgaplessgridborder },
    [SchemeSelLayout]   = { seltileborder,  selfibonacciborder,  selfloatborder,  seldeckborder,  selnrowgridborder,  selbstackborder,  selcenmasterborder,  selmonocleborder,  selgaplessgridborder },
};

static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
static const char ptagf[] = "[%s:%s]";
static const char etagf[] = "%s";
static const int lcaselbl = 0;

#define RULENMONS 2
static const TagRule tagrules[RULENMONS][9] = {
/* MON0 showbar topbar vacant layout gapih gapiv gapoh gapov smartgaps vpad spad tpad borderpx nmaster mfact */
    {{  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 }},
/* MON1 showbar topbar vacant layout gapih gapiv gapoh gapov smartgaps vpad spad tpad borderpx nmaster mfact */
    {{  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 },
     {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,   1,       1,      0.5 }},
};

static const char *scpclean[] = { "u", NULL };
static const char *scpcmus[]  = { "i", "st", "-c", "scpcmus",  "-t", "cmusSCP", "-e", "cmus", NULL };
static const char *scpcal[]   = { "y", "qalculate-gtk", "--title", "calSCP", NULL };

#define WTYPE "_NET_WM_WINDOW_TYPE_"

static const Rule rules[] = {
    /* class      instance    title          wintype tags mask switchtotag isfloating iscentered ispermanent isterminal noswallow monitor scratch key */
    /* Scratchpads */
    { "scpclean", NULL,       NULL,          NULL,   0,        0,          0,         0,         0,          0,         0,        -1, 'u' },
    { "scpcmus",  NULL,       NULL,          NULL,   0,        0,          0,         0,         1,          0,         0,        -1, 'i' },
    { NULL,       NULL,       "calSCP",      NULL,   0,        0,          1,         1,         0,          0,         0,        -1, 'y' },
    /* Swallow */
    { "St",       NULL,       NULL,          NULL,   0,        0,          0,         0,         0,          1,         0,        -1, 0 },
    { "Alacritty",NULL,       NULL,          NULL,   0,        0,          0,         0,         0,          1,         0,        -1, 0 },
    { "XTerm",    NULL,       NULL,          NULL,   0,        0,          0,         0,         0,          1,         0,        -1, 0 },
    { "Emacs",    NULL,       NULL,          NULL,   0,        0,          0,         0,         0,          1,         0,        -1, 0 },
    /* Noswallow */
    { NULL,       NULL,       "Event Tester",NULL,   0,        0,          0,         0,         0,          0,         1,        -1, 0 },
    { "Xephyr",   NULL,       NULL,          NULL,   0,        0,          1,         1,         0,          0,         1,        -1, 0 },
    { "Gimp",     NULL,       NULL,          NULL,   1 << 8,   3,          1,         1,         0,          0,         1,        -1, 0 },
    { NULL,       NULL,       "glxgears",    NULL,   0,        0,          1,         0,         0,          0,         1,        -1, 0 },
    /* General windows */
    { NULL,       "Navigator",NULL,          NULL,   1,        0,          0,         0,         1,          0,         1,         0, 0 },
    { NULL,       "chromium", NULL,          NULL,   1 << 3,   0,          0,         0,         1,          0,         1,         0, 0 },
    { NULL,       "discord",  NULL,          NULL,   1,        0,          0,         0,         0,          0,         0,         1, 0 },
    /* Wintype */
    { NULL,       NULL,       NULL, WTYPE "DIALOG",  0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "UTILITY", 0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "TOOLBAR", 0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "SPLASH",  0,        0,          1,         1,         0,          0,         0,        -1, 0 },
};

static const int resizehints = 0;
static const int nmaster     = 1;
static const int attachbelow = 1;

#define FORCE_VSPLIT 1
#include "vanitygaps.c"

static const Layout layouts[] = {
    /* symbol           arrange function */
    { "tile",                tile },
    { "spiral",              spiral },
    { "float",               NULL },
    { "deck",                deck },
    { "nrowgrid",            nrowgrid },
    { "grid",                gaplessgrid },
    { "bstack",              bstack },
    { "centeredmaster",      centeredmaster },
    { "centeredmasterfloat", centeredfloatingmaster },
    { "monocle",             monocle },
    { NULL,                  NULL },
};

#include <X11/XF86keysym.h>

#define M Mod4Mask
#define A Mod1Mask
#define S ShiftMask
#define C ControlMask
#define P KeyPress
#define R KeyRelease

#define TAGKEYS(KEY,TAG) \
    &((Keychord){1, {{P, A,     KEY}},                                  view,           { .ui = 1 << TAG } }), \
    &((Keychord){1, {{P, C,     KEY}},                                  toggleview,     { .ui = 1 << TAG } }), \
    &((Keychord){1, {{P, M,     KEY}},                                  toggletag,      { .ui = 1 << TAG } }), \
    &((Keychord){1, {{P, A|S,   KEY}},                                  tag,            { .ui = 1 << TAG } }), \
    &((Keychord){1, {{P, A|C,   KEY}},                                  tagwith,        { .ui = 1 << TAG } }), \
    &((Keychord){1, {{P, M|S,   KEY}},                                  swaptags,       { .ui = 1 << TAG } }), \
    &((Keychord){2, {{P, A|C,   XK_q},                    {P, 0, KEY}}, killontag,      { .ui = 1 << TAG } }), \
    &((Keychord){2, {{P, A|C,   XK_t},                    {P, 0, KEY}}, tagall,         { .ui  = TAG     } }), \
    &((Keychord){2, {{P, A|C,   XK_f},                    {P, 0, KEY}}, tagallfloat,    { .ui  = TAG     } }), \
    &((Keychord){3, {{P, A|C,   XK_comma},  {P, 0, XK_q}, {P, 0, KEY}}, killontagmonn,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C,   XK_period}, {P, 0, XK_q}, {P, 0, KEY}}, killontagmonp,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C,   XK_comma},  {P, 0, XK_f}, {P, 0, KEY}}, focusprevmon,   { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C,   XK_period}, {P, 0, XK_f}, {P, 0, KEY}}, focusnextmon,   { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C,   XK_comma},  {P, 0, XK_s}, {P, 0, KEY}}, showprevmon,    { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C,   XK_period}, {P, 0, XK_s}, {P, 0, KEY}}, shownextmon,    { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_comma},  {P, 0, XK_m}, {P, 0, KEY}}, tagmovprevmon,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_period}, {P, 0, XK_m}, {P, 0, KEY}}, tagmovnextmon,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_comma},  {P, 0, XK_f}, {P, 0, KEY}}, tagfolprevmon,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_period}, {P, 0, XK_f}, {P, 0, KEY}}, tagfolnextmon,  { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_comma},  {P, 0, XK_s}, {P, 0, KEY}}, tagshowprevmon, { .ui = 1 << TAG } }), \
    &((Keychord){3, {{P, A|C|S, XK_period}, {P, 0, XK_s}, {P, 0, KEY}}, tagshownextmon, { .ui = 1 << TAG } }),

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

static Keychord *keychords[] = {
&((Keychord){1, {{P, A,     XK_Return}},                  spawn,             SHCMD("$TERMINAL") }),
&((Keychord){1, {{P, A|S,   XK_c}},                       spawn,             SHCMD("$TERMINAL htop") }),
&((Keychord){1, {{P, A,     XK_e}},                       spawn,             SHCMD("emacsclient -c -a ''") }),
&((Keychord){2, {{P, A|S,   XK_e}, {P, 0, XK_v}},         spawn,             SHCMD("emacsclient -c -e '(+vterm/here nil)'") }),
&((Keychord){2, {{P, A|S,   XK_e}, {P, 0, XK_e}},         spawn,             SHCMD("emacsclient -c -e '(elfeed)'") }),
&((Keychord){1, {{P, A,     XK_w}},                       spawn,             SHCMD("xdo activate -N firefox || firefox") }),
&((Keychord){1, {{P, M,     XK_w}},                       spawn,             SHCMD("xdo activate -N Chromium || chromium") }),
&((Keychord){1, {{P, A|C,   XK_KP_Down}},                 spawn,             SHCMD("xkill") }),
&((Keychord){1, {{P, A|C,   XK_d}},                       spawn,             SHCMD("discord") }),
&((Keychord){1, {{P, A|C,   XK_s}},                       spawn,             SHCMD("spotify") }),
&((Keychord){1, {{P, A|C,   XK_u}},                       spawn,             SHCMD("import my-stuff/Pictures/snips/$(date +'%F-%T').png") }),
&((Keychord){1, {{P, A,     XK_p}},                       spawn,             SHCMD("pcmanfm") }),
&((Keychord){1, {{P, A|C,   XK_m}},                       spawn,             SHCMD("multimc") }),
&((Keychord){1, {{P, A|C,   XK_n}},                       spawn,             SHCMD("dunstctl close") }),
&((Keychord){1, {{P, A|M|C, XK_l}},                       spawn,             SHCMD("slock") }),
&((Keychord){1, {{P, M,     XK_g}},                       spawn,             SHCMD("xmenu.sh -p 0x0") }),
&((Keychord){1, {{P, A|S,   XK_Return}},                  spawn,             SHCMD("dmenu_run_history -F -l 5 -g 10 -p 'Run'") }),
&((Keychord){1, {{P, A|S,   XK_w}},                       spawn,             SHCMD("Booky 'firefox' '_' 'Bconfig'") }),
&((Keychord){1, {{P, A|S,   XK_z}},                       spawn,             SHCMD("audio-changer") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_z}},         spawn,             SHCMD("cmus-rem") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_v}},         spawn,             SHCMD("volume-script") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_c}},         spawn,             SHCMD("calc") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_p}},         spawn,             SHCMD("passmenu2 -F -p 'Passwords:'") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_m}},         spawn,             SHCMD("manview") }),
&((Keychord){2, {{P, A|S,   XK_d}, {P, 0, XK_q}},         spawn,             SHCMD("shut") }),
&((Keychord){1, {{P, 0,     XF86XK_AudioPrev}},           spawn,             SHCMD("playerctl --player cmus previous") }),
&((Keychord){1, {{P, 0,     XF86XK_AudioNext}},           spawn,             SHCMD("playerctl --player cmus next") }),
&((Keychord){1, {{P, 0,     XF86XK_AudioPlay}},           spawn,             SHCMD("playerctl --player cmus play-pause") }),
&((Keychord){1, {{P, 0,     XF86XK_AudioLowerVolume}},    spawn,             SHCMD("pamixer --allow-boost -d 1 ; sdwm -v") }),
&((Keychord){1, {{P, 0,     XF86XK_AudioRaiseVolume}},    spawn,             SHCMD("pamixer --allow-boost -i 1 ; sdwm -v") }),
&((Keychord){1, {{P, A,     XK_q}},                       killclient,        {0} }),
&((Keychord){1, {{P, M,     XK_q}},                       killpermanent,     {0} }),
&((Keychord){1, {{P, A|S,   XK_q}},                       killunsel,         {0} }),
&((Keychord){1, {{P, M,     XK_v}},                       togglevacant,      {0} }),
&((Keychord){1, {{P, A|C,   XK_v}},                       toggletopbar,      {0} }),
&((Keychord){1, {{P, M|S,   XK_v}},                       togglepadding,     {0} }),
&((Keychord){1, {{P, A|S,   XK_p}},                       togglepreview,     {0} }),
&((Keychord){1, {{P, A,     XK_n}},                       togglebar,         {0} }),
&((Keychord){1, {{P, A,     XK_bracketleft}},             incnmaster,        { .i = +1 } }),
&((Keychord){1, {{P, A,     XK_bracketright}},            incnmaster,        { .i = -1 } }),
&((Keychord){1, {{P, A,     XK_space}},                   focusmaster,       {0} }),
&((Keychord){1, {{P, A|C,   XK_space}},                   switchcol,         {0} }),
&((Keychord){1, {{P, A,     XK_h}},                       setmfact,          { .f = -0.05 } }),
&((Keychord){1, {{P, A,     XK_l}},                       setmfact,          { .f = +0.05 } }),
&((Keychord){1, {{P, A|S,   XK_h}},                       setcfact,          { .f = +0.25 } }),
&((Keychord){1, {{P, A|S,   XK_l}},                       setcfact,          { .f = -0.25 } }),
&((Keychord){2, {{P, A|S,   XK_o}, {P, 0, XK_m}},         setmfact,          {0} }),
&((Keychord){2, {{P, A|S,   XK_o}, {P, 0, XK_c}},         setcfact,          {0} }),
&((Keychord){2, {{P, A|S,   XK_o}, {P, 0, XK_n}},         resetnmaster,      {0} }),
&((Keychord){1, {{P, A|S,   XK_j}},                       pushdown,          {0} }),
&((Keychord){1, {{P, A|S,   XK_k}},                       pushup,            {0} }),
&((Keychord){1, {{P, A,     XK_k}},                       focusstack,        { .i = +1 } }),
&((Keychord){1, {{P, A,     XK_j}},                       focusstack,        { .i = -1 } }),
&((Keychord){1, {{P, A,     XK_t}},                       setlayout,         { .i = 0 } }),
&((Keychord){1, {{P, A,     XK_v}},                       setlayout,         { .i = 1 } }),
&((Keychord){1, {{P, A|S,   XK_f}},                       setlayout,         { .i = 2 } }),
&((Keychord){1, {{P, A,     XK_d}},                       setlayout,         { .i = 3 } }),
&((Keychord){2, {{P, A,     XK_g}, {P, 0, XK_n}},         setlayout,         { .i = 4 } }),
&((Keychord){2, {{P, A,     XK_g}, {P, 0, XK_g}},         setlayout,         { .i = 5 } }),
&((Keychord){1, {{P, A,     XK_b}},                       setlayout,         { .i = 6 } }),
&((Keychord){2, {{P, A,     XK_m}, {P, 0, XK_c}},         setlayout,         { .i = 7 } }),
&((Keychord){2, {{P, A,     XK_m}, {P, 0, XK_f}},         setlayout,         { .i = 8 } }),
&((Keychord){2, {{P, A,     XK_m}, {P, 0, XK_m}},         setlayout,         { .i = 9 } }),
&((Keychord){1, {{P, A|S,   XK_t}},                       tabmode,           {-1} }),
&((Keychord){1, {{P, A|C,   XK_i}},                       cyclelayout,       { .i = -1 } }),
&((Keychord){1, {{P, A|C,   XK_p}},                       cyclelayout,       { .i = +1 } }),
&((Keychord){1, {{P, A,     XK_Tab}},                     goback,            {0} }),
&((Keychord){1, {{P, A|S,   XK_bracketright}},            viewnextempty,     {0} }),
&((Keychord){1, {{P, A|S,   XK_bracketleft}},             viewprevempty,     {0} }),
&((Keychord){1, {{P, A|C,   XK_bracketright}},            viewnext,          {0} }),
&((Keychord){1, {{P, A|C,   XK_bracketleft}},             viewprev,          {0} }),
&((Keychord){1, {{P, A|S,   XK_a}},                       winview,           {0} }),
&((Keychord){1, {{P, A,     XK_semicolon}},               zoom,              {0} }),
&((Keychord){1, {{P, A|S,   XK_v}},                       transfer,          {0} }),
&((Keychord){2, {{P, A|S,   XK_space}, {P, 0, XK_f}},     togglefloating,    {0} }),
&((Keychord){2, {{P, A|S,   XK_space}, {P, 0, XK_space}}, unfloatvisible,    {0} }),
&((Keychord){1, {{P, A,     XK_s}},                       togglesticky,      {0} }),
&((Keychord){1, {{P, A,     XK_f}},                       togglefullscr,     {0} }),
&((Keychord){1, {{P, A,     XK_u}},                       togglescratch,     { .v = scpclean } }),
&((Keychord){1, {{P, A,     XK_i}},                       togglescratch,     { .v = scpcmus } }),
&((Keychord){1, {{P, A,     XK_y}},                       togglescratch,     { .v = scpcal } }),
&((Keychord){2, {{P, A|S,   XK_u}, {P, 0, XK_d}},         removescratch,     { .v = scpclean } }),
&((Keychord){2, {{P, A|S,   XK_i}, {P, 0, XK_d}},         removescratch,     { .v = scpcmus } }),
&((Keychord){2, {{P, A|S,   XK_y}, {P, 0, XK_d}},         removescratch,     { .v = scpcal } }),
&((Keychord){2, {{P, A|S,   XK_u}, {P, 0, XK_a}},         setscratch,        { .v = scpclean } }),
&((Keychord){2, {{P, A|S,   XK_i}, {P, 0, XK_a}},         setscratch,        { .v = scpcmus } }),
&((Keychord){2, {{P, A|S,   XK_y}, {P, 0, XK_a}},         setscratch,        { .v = scpcal } }),
&((Keychord){1, {{P, A,     XK_comma}},                   focusmon,          { .i = -1 } }),
&((Keychord){1, {{P, A,     XK_period}},                  focusmon,          { .i = +1 } }),
&((Keychord){1, {{P, A|S,   XK_comma}},                   tagmon,            { .i = -1 } }),
&((Keychord){1, {{P, A|S,   XK_period}},                  tagmon,            { .i = +1 } }),
&((Keychord){1, {{P, A|S,   XK_equal}},                   incrgaps,          { .i = +1 } }),
&((Keychord){1, {{P, A|S,   XK_minus}},                   incrgaps,          { .i = -1 } }),
&((Keychord){2, {{P, A|S,   XK_o}, {P, 0, XK_g}},         defaultgaps,       {0} }),
&((Keychord){1, {{P, A|S,   XK_0}},                       togglegaps,        {0} }),
&((Keychord){1, {{P, A|C,   XK_equal}},                   setborderpx,       { .i = +1 } }),
&((Keychord){1, {{P, A|C,   XK_minus}},                   setborderpx,       { .i = -1 } }),
&((Keychord){2, {{P, A|S,   XK_o}, {P, 0, XK_b}},         setborderpx,       { .i = 0 } }),
&((Keychord){1, {{P, M|S,   XK_Escape}},                  quit,              {0} }),
&((Keychord){1, {{P, M|S,   XK_q}},                       quit,              {1} }),
TAGKEYS(XK_1, 0)
TAGKEYS(XK_2, 1)
TAGKEYS(XK_3, 2)
TAGKEYS(XK_4, 3)
TAGKEYS(XK_5, 4)
TAGKEYS(XK_6, 5)
TAGKEYS(XK_7, 6)
TAGKEYS(XK_8, 7)
TAGKEYS(XK_9, 8)
&((Keychord){1, {{P, A,     XK_0}},                       view,              { .ui = ~0 } }),
};

static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      { .i = 0 } },
    { ClkLtSymbol,          0,              Button3,        setlayout,      { .i = 7 } },
    { ClkLtSymbol,          S,              Button1,        cyclelayout,    { .i = +1 } },
    { ClkLtSymbol,          S,              Button3,        cyclelayout,    { .i = -1 } },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            A,              Button1,        tag,            {0} },
    { ClkTagBar,            A,              Button3,        toggletag,      {0} },
    { ClkNumSymbol,         0,              Button1,        spawn,          SHCMD("xmenu.sh -p 0x0:0") },
    { ClkClientWin,         A,              Button1,        movemouse,      {0} },
    { ClkClientWin,         A,              Button2,        togglefloating, {0} },
    { ClkClientWin,         A,              Button3,        resizemouse,    {0} },
    { ClkClientWin,         A|S,            Button2,        movecenter,     {0} },
    { ClkTabBar,            0,              Button1,        focuswin,       {0} },
};

#include "dwmc.c"

static Signal signals[] = {
    /* signum           function */
    { "togglebar",      togglebar },
    { "togglevacant",   togglevacant },
    { "togglepadding",  togglepadding },
    { "focusmon",       focusmon },
    { "tagmon",         tagmon },
    { "quit",           quit },
    { "viewex",         viewex },
    { "toggleviewex",   toggleviewex },
    { "tagex",          tagex },
    { "tagwithex",      tagwithex },
    { "toggletagex",    toggletagex },
    { "setlayoutex",    setlayoutex },
};
