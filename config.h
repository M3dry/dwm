enum showtab_modes { showtab_never, showtab_auto, showtab_nmodes, showtab_always};
static const int showtab                   = showtab_never; /* Default tab bar show mode */
static const int toptab                    = 0; /* False means bottom tab bar */
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
static       unsigned int vacantonstart    = 1;  /* 0 means no vacant tags */
static const unsigned int ulinepad         = 6;  /* horizontal padding between the underline and tag */
static const unsigned int ulinestroke      = 2;  /* thickness / height of the underline */
static const unsigned int ulinevoffset     = 0;  /* how far above the bottom of the bar the line should appear */
static const unsigned int underlinetags    = 1;  /* 0 means no underline */
static const unsigned int underlinevacant  = 1;  /* 0 means no underline for vacant tags */
static const unsigned int gappih           = vertpad || sidepad ? vertpadtoggle : 0; /* horiz inner gap between windows */
static const unsigned int gappiv           = vertpad || sidepad ? vertpadtoggle : 0; /* vert inner gap between windows */
static const unsigned int gappoh           = vertpad            ? sidepadtoggle : 0; /* horiz outer gap between windows and screen edge */
static const unsigned int gappov           = sidepad            ? sidepadtoggle : 0; /* vert outer gap between windows and screen edge */
static       unsigned int smartgaps        = vertpad || sidepad ? 0 : 1; /* 1 means no outer gap when there is only one window one window */
static       unsigned int padding          = vertpad || sidepad ? 1 : 0;
static const int swallowfloating           = 1; /* 1 means swallow floating windows by default */
static unsigned int borderpx               = 1; /* border pixel of windows */
static const unsigned int snap             = 0; /* snap pixel */
static const int startontag                = 1; /* 0 means no tag active on start */
static const int decorhints                = 1; /* 1 means respect decoration hints */
static const int focusonwheel              = 0;
static const char *fonts[] = { "Operator Mono SSm Lig Nerd Font:size=12:antialias=true:autohint=true" };

static const char normfg[]                = "#4E5579";
static const char selfg[]                 = "#5fafff";
static const char normbg[]                = "#1E1C31";
static const char selbg[]                 = "#1E1C31";

static const char invnormbg[]             = "#f0f0f0";
static const char invnormfg[]             = "#000000";
static const char invselfg[]              = "#5fafff";
static const char invselbg[]              = "#f0f0f0";

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

static const char ocinvfg[]               = "#7986E7";
static const char ocinvbg[]               = "#f0f0f0";

static const char statusfg[]              = "#7986E7";
static const char statusbg[]              = "#1e1c31";

static const char invstatusbg[]           = "#f0f0f0";

static const char ltsymbolfg[]            = "#1e1c31";
static const char ltsymbolbg[]            = "#ff5370";

static const char normtabfg[]             = "#4E5579";
static const char seltabfg[]              = "#7986E7";
static const char normtabbg[]             = "#1E1C31";
static const char seltabbg[]              = "#1E1C31";

static const char vacindfg[]              = "#4e5579";
static const char indfg[]                 = "#7986E7";

static const char numfg[]                 = "#7986e7";
static const char numbg[]                 = "#1e1c31";

static const char systraybg[]             = "#1E1C31";

static const char normtileborder[]        = "#1E1C31";
static const char normfibonacciborder[]   = "#1E1C31";
static const char normfloatborder[]       = "#1E1C31";
static const char normdeckborder[]        = "#1E1C31";
static const char normnrowgridborder[]    = "#1E1C31";
static const char normbstackborder[]      = "#1E1C31";
static const char normcenmasterborder[]   = "#1E1C31";
static const char normmonocleborder[]     = "#1E1C31";
static const char normgaplessgridborder[] = "#1E1C31";
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
    /* Tags/borders       fg            bg      float               sticky            sticky + float         fakefullscreen   fakefullscreen + float */
    [SchemeNorm]        = { normfg,     normbg, normfloatwinborder, normstickyborder, normstickyfloatborder, normfakefullscr, normfakefullscrfloat },
    [SchemeSel]         = { selfg,      selbg,  selfloatwinborder,  selstickyborder,  selstickyfloatborder,  selfakefullscr,  selfakefullscrfloat },
    [SchemeOccupied]    = { occupiedfg, occupiedbg },
    [SchemeOccupiedInv] = { ocinvfg,    ocinvbg },
    [SchemeStatus]      = { statusfg,   statusbg, invstatusbg },
    [SchemeLtsymbol]    = { ltsymbolfg, ltsymbolbg },
    [SchemeTabNorm]     = { normtabfg,  normtabbg },
    [SchemeTabSel]      = { seltabfg,   seltabbg},
    [SchemeClientVac]   = { vacindfg },
    [SchemeClient]      = { indfg },
    [SchemeClientNum]   = { numfg,      numbg },
    [SchemeSystray]     = {             systraybg },
    [SchemeInvMon]      = { invnormfg,  invnormbg },
    [SchemeInvMonSel]   = { invselfg,   invselbg },
    /* Win borders          tile            fibonacci            float            deck            nrowgrid            bstack            centeredmaster       monocle            gaplessgrid */
    [SchemeNormLayout]  = { normtileborder, normfibonacciborder, normfloatborder, normdeckborder, normnrowgridborder, normbstackborder, normcenmasterborder, normmonocleborder, normgaplessgridborder },
    [SchemeSelLayout]   = { seltileborder,  selfibonacciborder,  selfloatborder,  seldeckborder,  selnrowgridborder,  selbstackborder,  selcenmasterborder,  selmonocleborder,  selgaplessgridborder },
};

static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };

static const char ptagf[] = "[%s:%s]";
static const char etagf[] = "%s";
static const int lcaselbl = 0;

static const char *defaulttagapps[] = { "firefox", NULL, NULL, "chromium", NULL, NULL, NULL, "discord", "gimp" };

static const char *scpclean[] = {"u", NULL};
static const char *scpcmus[]  = {"i", "st", "-c", "scpcmus",  "-t", "cmusSCP", "-e", "cmus", NULL};
static const char *scpcal[]   = {"y", "qalculate-gtk", "--title", "calSCP", NULL};

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
    { NULL,       "discord",  NULL,          NULL,   1 << 7,   0,          0,         0,         0,          0,         0,        -1, 0 },
    { NULL,       "Navigator",NULL,          NULL,   1,        0,          0,         0,         1,          0,         1,        -1, 0 },
    { NULL,       "nyxt",     NULL,          NULL,   1,        0,          0,         0,         1,          0,         1,        -1, 0 },
    { NULL,       "chromium", NULL,          NULL,   1 << 3,   0,          0,         0,         1,          0,         1,        -1, 0 },
    /* Wintype */
    { NULL,       NULL,       NULL, WTYPE "DIALOG",  0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "UTILITY", 0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "TOOLBAR", 0,        0,          1,         1,         0,          0,         0,        -1, 0 },
    { NULL,       NULL,       NULL, WTYPE "SPLASH",  0,        0,          1,         1,         0,          0,         0,        -1, 0 },
};

static const TagRule tagrules[LENGTH(tags) + 1] = {
    /* showbar topbar vacant layout gapih gapiv gapoh gapov smartgaps vpad spad borderpx nmaster mfact */
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  1,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   1,       1,      0.5 },
    {  0,      1,     1,     0,     0,    0,    0,    0,    1,        0,   0,   0,       1,      0.5 },
};

static const float mfact     = 0.5;
static const int resizehints = 0;
static const int nmaster     = 1;
static const int attachbelow = 1;

#define FORCE_VSPLIT 1
#include "vanitygaps.c"

static const Layout layouts[] = {
    /* symbol           arrange function */
    { "tile",           tile },
    { "spiral",         spiral },
    { "float",          NULL },
    { "deck",           deck },
    { "nrowgrid",       nrowgrid },
    { "bstack",         bstack },
    { "centeredmaster", centeredmaster },
    { "monocle",        monocle },
    { "grid",           gaplessgrid },
    { NULL,             NULL },
};

#include <X11/XF86keysym.h>

#define M Mod4Mask
#define A Mod1Mask
#define S ShiftMask
#define C ControlMask

#define TAGKEYS(KEY,TAG) \
    { A,       -1,        KEY,   view,         {.ui = 1 << TAG} }, \
    { C,       -1,        KEY,   toggleview,   {.ui = 1 << TAG} }, \
    { M,       -1,        KEY,   toggletag,    {.ui = 1 << TAG} }, \
    { A|S,     -1,        KEY,   combotag,     {.ui = 1 << TAG} }, \
    { A|C,     -1,        KEY,   tagwith,      {.ui = 1 << TAG} }, \
    { M|S,     -1,        KEY,   swaptags,     {.ui = 1 << TAG} }, \
    { A|C,     XK_comma,  KEY,   focusnextmon, {.ui = 1 << TAG} }, \
    { A|C,     XK_period, KEY,   focusprevmon, {.ui = 1 << TAG} }, \
    { A|C|S,   XK_comma,  KEY,   tagnextmon,   {.ui = 1 << TAG} }, \
    { A|C|S,   XK_period, KEY,   tagprevmon,   {.ui = 1 << TAG} }, \
    { A|C,     XK_q,      KEY,   killontag,    {.ui = 1 << TAG} },

#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

static Key keys[] = {
{A,      -1,    XK_Return,               spawn,                SHCMD("$TERMINAL") },
{A|S,    -1,    XK_c,                    spawn,                SHCMD("$TERMINAL htop") },
{A|S,    -1,    XK_z,                    spawn,                SHCMD("playerctl play-pause") },
{A,      -1,    XK_e,                    spawn,                SHCMD("$TERMINAL nvim") },
{A|S,    XK_e,  XK_e,                    spawn,                SHCMD("emacsclient -c") },
{A|S,    XK_e,  XK_c,                    spawn,                SHCMD("emacsclient -c -e '(ibuffer)'") },
{A|S,    XK_e,  XK_d,                    spawn,                SHCMD("emacsclient -c -e '(dired nil)'") },
{A|S,    XK_e,  XK_f,                    spawn,                SHCMD("emacsclient -c -e '(elfeed)'") },
{A,      -1,    XK_s,                    spawn,                SHCMD("~/.emacs.d/bin/doom everywhere") },
{A,      -1,    XK_w,                    spawn,                SHCMD("xdo activate -N firefox || firefox") },
{M,      -1,    XK_w,                    spawn,                SHCMD("xdo activate -N Chromium || chromium") },
{A|C,    -1,    XK_KP_Down,              spawn,                SHCMD("xkill") },
{A|C,    -1,    XK_d,                    spawn,                SHCMD("discord") },
{A|C,    -1,    XK_u,                    spawn,                SHCMD("import my-stuff/Pictures/snips/$(date +'%F-%T').png") },
{A,      -1,    XK_p,                    spawn,                SHCMD("pcmanfm") },
{A|C,    -1,    XK_m,                    spawn,                SHCMD("multimc") },
{A|M|C,  -1,    XK_l,                    spawn,                SHCMD("slock") },
{M,      -1,    XK_g,                    spawn,                SHCMD("xmenu.sh -p 0x0") },
{A,      -1,    XK_r,                    spawndefault,         {0} },
{A|S,    -1,    XK_Return,               spawn,                SHCMD("dmenu_run_history -l 5 -g 10 -p 'Run'") },
{A,      -1,    XK_c,                    spawn,                SHCMD("volume-script") },
{A|C,    -1,    XK_Return,               spawn,                SHCMD("Booky 'st -e nvim' '><' 'Cconfig'") },
{A|S,    -1,    XK_w,                    spawn,                SHCMD("Booky 'firefox' '_' 'Bconfig'") },
{A,      -1,    XK_z,                    spawn,                SHCMD("music-changer cmus") },
{A|S,    XK_d,  XK_s,                    spawn,                SHCMD("switch") },
{A|S,    XK_d,  XK_c,                    spawn,                SHCMD("calc") },
{A|S,    XK_d,  XK_p,                    spawn,                SHCMD("passmenu2 -F -p 'Passwords'") },
{A|S,    XK_d,  XK_v,                    spawn,                SHCMD("manview") },
{A|S,    XK_d,  XK_q,                    spawn,                SHCMD("shut") },
{0,      -1,    XF86XK_AudioPrev,        spawn,                SHCMD("playerctl --player cmus previous") },
{0,      -1,    XF86XK_AudioNext,        spawn,                SHCMD("playerctl --player cmus next") },
{0,      -1,    XF86XK_AudioPlay,        spawn,                SHCMD("playerctl --player cmus play-pause") },
{0,      -1,    XF86XK_AudioLowerVolume, spawn,                SHCMD("pamixer --allow-boost -d 1 ; killall dwmStatus ; dwmStatus &") },
{0,      -1,    XF86XK_AudioRaiseVolume, spawn,                SHCMD("pamixer --allow-boost -i 1 ; killall dwmStatus ; dwmStatus &") },
{A,      -1,    XK_q,                    killclient,           {0} },
{A|C|S,  -1,    XK_x,                    killpermanent,        {0} },
{A|S,    -1,    XK_q,                    killunsel,            {0} },
{M,      -1,    XK_v,                    togglevacant,         {0} },
{A|C,    -1,    XK_v,                    toggletopbar,         {0} },
{M|S,    -1,    XK_v,                    togglepadding,        {0} },
{A,      -1,    XK_n,                    togglebar,            {0} },
{A|S,    -1,    XK_h,                    setmfact,             { .f = -0.05 } },
{A|S,    -1,    XK_l,                    setmfact,             { .f = +0.05 } },
{M|C,    -1,    XK_u,                    setmfact,             { .f = mfact + 1 } },
{A|S,    -1,    XK_j,                    setcfact,             { .f = +0.25 } },
{A|S,    -1,    XK_k,                    setcfact,             { .f = -0.25 } },
{M|C|S,  -1,    XK_u,                    setcfact,             {0} },
{A,      -1,    XK_bracketleft,          incnmaster,           { .i = +1 } },
{A,      -1,    XK_bracketright,         incnmaster,           { .i = -1 } },
{M,      -1,    XK_space,                focusmaster,          {0} },
{A|C,    -1,    XK_space,                switchcol,            {0} },
{A,      -1,    XK_h,                    focusdir,             { .i = 0 } },
{A,      -1,    XK_l,                    focusdir,             { .i = 1 } },
{A,      -1,    XK_k,                    focusdir,             { .i = 2 } },
{A,      -1,    XK_j,                    focusdir,             { .i = 3 } },
{M|S,    -1,    XK_j,                    focusstack,           { .i = +1 } },
{M|S,    -1,    XK_k,                    focusstack,           { .i = -1 } },
{M|A,    -1,    XK_h,                    inplacerotate,        { .i = +2 } },
{M|A,    -1,    XK_l,                    inplacerotate,        { .i = -2 } },
{A,      -1,    XK_t,                    setlayout,            { .v = &layouts[0] }},
{A,      -1,    XK_v,                    setlayout,            { .v = &layouts[1] }},
{A|S,    -1,    XK_f,                    setlayout,            { .v = &layouts[2] }},
{A,      -1,    XK_d,                    setlayout,            { .v = &layouts[3] }},
{A,      -1,    XK_g,                    setlayout,            { .v = &layouts[4] }},
{A,      -1,    XK_b,                    setlayout,            { .v = &layouts[5] }},
{A|S,    -1,    XK_m,                    setlayout,            { .v = &layouts[6] }},
{A,      -1,    XK_m,                    setlayout,            { .v = &layouts[7] }},
{A|S,    -1,    XK_g,                    setlayout,            { .v = &layouts[8] }},
{A|S,    -1,    XK_t,                    tabmode,              {-1} },
{A|C,    -1,    XK_i,                    cyclelayout,          { .i = -1 } },
{A|C,    -1,    XK_p,                    cyclelayout,          { .i = +1 } },
{A,      -1,    XK_Tab,                  goback,               {0} },
{A|S,    -1,    XK_n,                    shiftviewclients,     { .i = +1 } },
{A|S,    -1,    XK_p,                    shiftviewclients,     { .i = -1 } },
{A|S,    -1,    XK_a,                    winview,              {0} },
{A,      -1,    XK_semicolon,            zoom,                 {0} },
{A|S,    -1,    XK_v,                    transfer,             {0} },
{M|C,    -1,    XK_j,                    pushdown,             {0} },
{M|C,    -1,    XK_k,                    pushup,               {0} },
{A|C,    -1,    XK_r,                    togglefloating,       {0} },
{A|S,    -1,    XK_space,                unfloatvisible,       {0} },
{A|S,    -1,    XK_s,                    togglesticky,         {0} },
{A,      -1,    XK_f,                    togglefullscr,        {0} },
{A|C,    -1,    XK_f,                    togglefakefullscreen, {0} },
{A,      -1,    XK_u,                    togglescratch,        { .v = scpclean } },
{A,      -1,    XK_i,                    togglescratch,        { .v = scpcmus } },
{A,      -1,    XK_y,                    togglescratch,        { .v = scpcal } },
{A|M,    -1,    XK_u,                    removescratch,        { .v = scpclean } },
{A|M,    -1,    XK_i,                    removescratch,        { .v = scpcmus } },
{A|M,    -1,    XK_y,                    removescratch,        { .v = scpcal } },
{A|S,    -1,    XK_u,                    setscratch,           { .v = scpclean } },
{A|S,    -1,    XK_i,                    setscratch,           { .v = scpcmus } },
{A|S,    -1,    XK_y,                    setscratch,           { .v = scpcal } },
{A,      -1,    XK_comma,                focusmon,             { .i = -1 } },
{A,      -1,    XK_period,               focusmon,             { .i = +1 } },
{A|S,    -1,    XK_comma,                tagmon,               { .i = -1 } },
{A|S,    -1,    XK_period,               tagmon,               { .i = +1 } },
{A|S,    -1,    XK_equal,                incrgaps,             { .i = +1 } },
{A|S,    -1,    XK_minus,                incrgaps,             { .i = -1 } },
{A|S,    -1,    XK_0,                    defaultgaps,          {0} },
{A|C,    -1,    XK_0,                    togglegaps,           {0} },
{A|C,    -1,    XK_equal,                setborderpx,          { .i = +1 } },
{A|C,    -1,    XK_minus,                setborderpx,          { .i = -1 } },
{M,      -1,    XK_0,                    setborderpx,          { .i = 0 } },
{M|S,    -1,    XK_Escape,               quit,                 {0} },
{A|C|S,  -1,    XK_q,                    quit,                 {1} },
TAGKEYS( XK_1, 0)
TAGKEYS( XK_2, 1)
TAGKEYS( XK_3, 2)
TAGKEYS( XK_4, 3)
TAGKEYS( XK_5, 4)
TAGKEYS( XK_6, 5)
TAGKEYS( XK_7, 6)
TAGKEYS( XK_8, 7)
TAGKEYS( XK_9, 8)};

static Button buttons[] = {
    /* click                event mask      button          function        argument */
    { ClkLtSymbol,          0,              Button1,        setlayout,      { .v = &layouts[0] } },
    { ClkLtSymbol,          0,              Button3,        setlayout,      { .v = &layouts[6] } },
    { ClkLtSymbol,          S,              Button1,        cyclelayout,    { .i = +1 } },
    { ClkLtSymbol,          S,              Button3,        cyclelayout,    { .i = -1 } },
    { ClkTagBar,            0,              Button1,        view,           {0} },
    { ClkTagBar,            0,              Button3,        toggleview,     {0} },
    { ClkTagBar,            A,              Button1,        tag,            {0} },
    { ClkTagBar,            A,              Button3,        toggletag,      {0} },
    { ClkNumSymbol,         0,              Button1,        spawn,          SHCMD("xmenu.sh -p 0x0") },
    { ClkClientWin,         A,              Button1,        movemouse,      {0} },
    { ClkClientWin,         A,              Button2,        togglefloating, {0} },
    { ClkClientWin,         A,              Button3,        resizemouse,    {0} },
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
