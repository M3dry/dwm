/* See LICENSE file for copyright and license details.
 *
 * dynamic window manager is designed like any other X client as well. It is
 * allowed to select for this event mask.
 *
 * The event handlers of dwm are organized in an array which is accessed
 * whenever a new event has been fetched. This allows event dispatching
 * in O(1) time.
 *
 * Each child of the root window is called a client, except windows which have
 * set the override_redirect flag. Clients are organized in a linked client
 * list on each monitor, the focus history is remembered through a stack list
 * on each monitor. Each client contains a bit array to indicate the tags of a
 * client.
 *
 * Keys and tagging rules are organized as arrays and defined in config.h.
 *
 * To understand everything else, start reading main().
 */
#include <X11/X.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <locale.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/cursorfont.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xproto.h>
#include <X11/Xutil.h>
#ifdef XINERAMA
#include <X11/extensions/Xinerama.h>
#endif /* XINERAMA */
#include <X11/Xft/Xft.h>
#include <Imlib2.h>
#include <X11/Xlib-xcb.h>
#include <xcb/res.h>
#ifdef __OpenBSD__
#include <sys/sysctl.h>
#include <kvm.h>
#endif /* __OpenBSD */

#include "drw.h"
#include "util.h"

/* macros */
#define BUTTONMASK              (ButtonPressMask|ButtonReleaseMask)
#define CLEANMASK(mask)         (mask & ~(numlockmask|LockMask) & (ShiftMask|ControlMask|Mod1Mask|Mod2Mask|Mod3Mask|Mod4Mask|Mod5Mask))
#define INTERSECT(x,y,w,h,m)    (MAX(0, MIN((x)+(w),(m)->wx+(m)->ww) - MAX((x),(m)->wx)) \
                               * MAX(0, MIN((y)+(h),(m)->wy+(m)->wh) - MAX((y),(m)->wy)))
#define ISVISIBLE(C)            ((C->tags & C->mon->tagset[C->mon->seltags]) || C->issticky)
#define LENGTH(X)               (sizeof X / sizeof X[0])
#define MOUSEMASK               (BUTTONMASK|PointerMotionMask)
#define WIDTH(X)                ((X)->w + 2 * (X)->bw)
#define HEIGHT(X)               ((X)->h + 2 * (X)->bw)
#define TAGMASK                 ((1 << LENGTH(tags)) - 1)
#define TEXTW(X)                (drw_fontset_getwidth(drw, (X)) + lrpad)

#define MWM_HINTS_FLAGS_FIELD       0
#define MWM_HINTS_DECORATIONS_FIELD 2
#define MWM_HINTS_DECORATIONS       (1 << 1)
#define MWM_DECOR_ALL               (1 << 0)
#define MWM_DECOR_BORDER            (1 << 1)
#define MWM_DECOR_TITLE             (1 << 3)
#define SYSTEM_TRAY_REQUEST_DOCK    0

/* XEMBED messages */
#define XEMBED_EMBEDDED_NOTIFY      0
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_FOCUS_IN             4
#define XEMBED_MODALITY_ON         10

#define XEMBED_MAPPED              (1 << 0)
#define XEMBED_WINDOW_ACTIVATE      1
#define XEMBED_WINDOW_DEACTIVATE    2

#define VERSION_MAJOR               0
#define VERSION_MINOR               0
#define XEMBED_EMBEDDED_VERSION (VERSION_MAJOR << 16) | VERSION_MINOR

/* enums */
enum { CurNormal, CurResize, CurMove, CurLast }; /* cursor */
enum { SchemeNorm, SchemeSel, SchemeInv, SchemeOccupied, SchemeStatus, SchemeLtsymbol,
       SchemeTabNorm, SchemeTabSel, SchemeClientVac, SchemeClient, SchemeClientNum, SchemeInvLtsymbol,
       SchemeSystray, SchemeNormLayout, SchemeSelLayout }; /* color schemes */
enum { NetSupported, NetWMName, NetWMState, NetWMCheck,
       NetSystemTray, NetSystemTrayOP, NetSystemTrayOrientation, NetSystemTrayOrientationHorz,
       NetWMFullscreen, NetActiveWindow, NetWMWindowType, NetWMWindowTypeDock, NetWMWindowTypeDialog,
       NetClientList, NetDesktopNames, NetDesktopViewport, NetNumberOfDesktops, NetCurrentDesktop, NetLast }; /* EWMH atoms */
enum { Manager, Xembed, XembedInfo, XLast }; /* Xembed atoms */
enum { WMProtocols, WMDelete, WMState, WMTakeFocus, WMLast }; /* default atoms */
enum { IsFloating, IsSticky, IsFullscreen, Tag, Cfact, Scratchkey, ClientLast }; /* dusk client atoms */
enum { ClkTagBar, ClkTabBar, ClkLtSymbol, ClkNumSymbol, ClkStatusText, ClkClientWin,
       ClkRootWin, ClkLast }; /* clicks */

typedef union {
    int i;
    unsigned int ui;
    float f;
    const void *v;
} Arg;

typedef struct {
    unsigned int click;
    unsigned int mask;
    unsigned int button;
    void (*func)(const Arg *arg);
    const Arg arg;
} Button;

typedef struct Monitor Monitor;
typedef struct Client Client;
struct Client {
    char name[256];
    float mina, maxa;
    float cfact;
    int x, y, w, h;
    int sfx, sfy, sfw, sfh; /* stored float geometry, used on mode revert */
    int oldx, oldy, oldw, oldh;
    int basew, baseh, incw, inch, maxw, maxh, minw, minh;
    int bw, oldbw;
    unsigned int tags;
    unsigned int switchtag;
    int isfixed, isfloating, isurgent, neverfocus, oldstate, isfullscreen, needresize, issticky, isterminal, noswallow, issteam, ispermanent, iscentered;
    char scratchkey;
    pid_t pid;
    Client *next;
    Client *snext;
    Client *swallowing;
    Monitor *mon;
    Window win;
};

typedef struct {
    int type;
    unsigned int mod;
    KeySym keysym;
} Key;

typedef struct {
    unsigned int n;
    const Key keys[5];
    void (*func)(const Arg *);
    const Arg arg;
} Keychord;

typedef struct {
    const char * sig;
    void (*func)(const Arg *);
} Signal;

typedef struct {
    const char *symbol;
    void (*arrange)(Monitor *);
} Layout;

#define MAXTABS 50

typedef struct Pertag Pertag;
struct Monitor {
    char ltsymbol[16];
    float mfact;
    int nmaster;
    int num;
    int by;               /* bar geometry */
    int ty;               /* tab bar geometry */
    int mx, my, mw, mh;   /* screen size */
    int wx, wy, ww, wh;   /* window area  */
    int gappih;           /* horizontal gap between windows */
    int gappiv;           /* vertical gap between windows */
    int gappoh;           /* horizontal outer gaps */
    int gappov;           /* vertical outer gaps */
    int smartgaps;
    int vp;           /* vertical outer gaps */
    int sp;           /* vertical outer gaps */
    int vactag;
    int restart;
    unsigned int borderpx;
    unsigned int seltags;
    unsigned int sellt;
    unsigned int tagset[2];
    int previewshow;
    int showbar;
    int showtab;
    int topbar;
    int toptab;
    Client *clients;
    Client *sel;
    Client *stack;
    Monitor *next;
    Window barwin;
    Window tabwin;
    Window tagwin;
    int ntabs;
    int tab_widths[MAXTABS];
    Pixmap tagmap[9];
    const Layout *lt[2];
    Pertag *pertag;
};

typedef struct {
    const char *class;
    const char *instance;
    const char *title;
    const char *wintype;
    unsigned int tags;
    int switchtag;
    int isfloating;
    int iscentered;
    int ispermanent;
    int isterminal;
    int noswallow;
    int monitor;
    const char scratchkey;
} Rule;

typedef struct {
    int showbar;
    int topbar;
    int vacant;
    int layout;
    int gapih;
    int gapiv;
    int gapoh;
    int gapov;
    int smartgaps;
    int vpad;
    int spad;
    int tpad;
    int borderpx;
    int nmaster;
    float mfact;
} TagRule;

typedef struct Systray   Systray;
struct Systray {
    Window win;
    Client *icons;
};

/* function declarations */
static void applyrules(Client *c);
static int applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact);
static void arrange(Monitor *m);
static void arrangemon(Monitor *m);
static void attach(Client *c);
static void attachBelow(Client *c);
static void attachstack(Client *c);
static int fake_signal(void);
static void buttonpress(XEvent *e);
static void checkotherwm(void);
static void cleanup(void);
static void cleanupmon(Monitor *mon);
static void clientmessage(XEvent *e);
static void configure(Client *c);
static void configurenotify(XEvent *e);
static void configurerequest(XEvent *e);
static Monitor *createmon(void);
static void cyclelayout(const Arg *arg);
static void destroynotify(XEvent *e);
static void detach(Client *c);
static void detachstack(Client *c);
static Monitor *dirtomon(int dir);
static void drawbar(Monitor *m);
static void drawbars(void);
static int drawstatusbar(Monitor *m, int bh, char* text);
static void drawtab(Monitor *m);
static void drawtabs(void);
static void expose(XEvent *e);
static Client *findbefore(Client *c);
static void focus(Client *c);
static void focusin(XEvent *e);
static void focusmon(const Arg *arg);
static void focusstack(const Arg *arg);
static void focuswin(const Arg* arg);
static Atom getatomprop(Client *c, Atom prop, Atom req);
static Client *getclientundermouse(void);
static int getrootptr(int *x, int *y);
static long getstate(Window w);
static unsigned int getsystraywidth();
static int gettextprop(Window w, Atom atom, char *text, unsigned int size);
static void goback(const Arg *arg);
static void grabbuttons(Client *c, int focused);
static void grabkeys(void);
static void incnmaster(const Arg *arg);
static void keypress(XEvent *e);
static void killclient(const Arg *arg);
static void killpermanent(const Arg *arg);
static void killunsel(const Arg *arg);
static void manage(Window w, XWindowAttributes *wa);
static void mappingnotify(XEvent *e);
static void maprequest(XEvent *e);
static void monocle(Monitor *m);
static void motionnotify(XEvent *e);
static void movemouse(const Arg *arg);
static void movecenter(const Arg *arg);
static unsigned int nexttag(int prev, int empty);
static Client *nexttiled(Client *c);
static void pop(Client *);
static Client *prevtiled(Client *c);
static void propertynotify(XEvent *e);
static void pushdown(const Arg *arg);
static void pushup(const Arg *arg);
static void quit(const Arg *arg);
static void savedata(const Monitor *m);
static void savemondata(unsigned long data[1], char *string);
static unsigned long gettagdata(const Monitor *m, char *string, int i);
static Monitor *recttomon(int x, int y, int w, int h);
static void removescratch(const Arg *arg);
static void removesystrayicon(Client *i);
static void resetnmaster(const Arg *arg);
static void resize(Client *c, int x, int y, int w, int h, int interact);
static void resizebarwin(Monitor *m);
static void resizeclient(Client *c, int x, int y, int w, int h);
static void resizemouse(const Arg *arg);
static void resizerequest(XEvent *e);
static void restack(Monitor *m);
static void run(void);
static void scan(void);
static int sendevent(Window w, Atom proto, int m, long d0, long d1, long d2, long d3, long d4);
static void sendmon(Client *c, Monitor *m);
static void setborderpx(const Arg *arg);
static void setclientstate(Client *c, long state);
static void setcurrentdesktop(void);
static void setdesktopnames(void);
static void setfocus(Client *c);
static void setfullscreen(Client *c, int fullscreen);
static void setlayout(const Arg *arg);
static void setcfact(const Arg *arg);
static void setmfact(const Arg *arg);
static void setscratch(const Arg *arg);
static void setnumdesktops(void);
static void setup(void);
static void setviewport(void);
static void seturgent(Client *c, int urg);
static void showhide(Client *c);
static void showtagpreview(int tag);
static void sigchld(int unused);
static void sighup(int unused);
static void sigterm(int unused);
static void spawn(const Arg *arg);
static void switchtag(void);
static void spawnscratch(const Arg *arg);
static void switchcol(const Arg *arg);
static void swaptags(const Arg *arg);
static Monitor *systraytomon(Monitor *m);
static void tabmode(const Arg *arg);
static void tag(const Arg *arg);
static void tagall(const Arg *arg);
static void tagallfloat(const Arg *arg);
static void tagwith(const Arg *arg);
static void tagmon(const Arg *arg);
static void killontag(const Arg *arg);
static void killontagmonn(const Arg *arg);
static void killontagmonp(const Arg *arg);
static void focusnextmon(const Arg *arg);
static void focusprevmon(const Arg *arg);
static void shownextmon(const Arg *arg);
static void showprevmon(const Arg *arg);
static void focusothermon(const Arg *arg, int dir, int show);
static void focusothermonview(const Arg *arg, Monitor *m);
static void tagmovnextmon(const Arg *arg);
static void tagmovprevmon(const Arg *arg);
static void tagfolnextmon(const Arg *arg);
static void tagfolprevmon(const Arg *arg);
static void tagshownextmon(const Arg *arg);
static void tagshowprevmon(const Arg *arg);
static void tagothermon(const Arg *arg, int dir, int show, int follow);
static void togglebar(const Arg *arg);
static void togglefloating(const Arg *arg);
static void togglescratch(const Arg *arg);
static void unfloatvisible(const Arg *arg);
static void togglefullscr(const Arg *arg);
static void togglesticky(const Arg *arg);
static void toggletag(const Arg *arg);
static void toggleview(const Arg *arg);
static void togglevacant(const Arg *arg);
static void toggletopbar(const Arg *arg);
static void togglepadding(const Arg *arg);
static void togglepreview(const Arg *arg);
static void transfer(const Arg *arg);
static void unfocus(Client *c, int setfocus);
static void unmanage(Client *c, int destroyed);
static void unmapnotify(XEvent *e);
static void updatecurrentdesktop(void);
static void updatebarpos(Monitor *m);
static void updatebars(void);
static void updateclientlist(void);
static int updategeom(void);
static void updatemotifhints(Client *c);
static void updatenumlockmask(void);
static void updatesizehints(Client *c);
static void updatestatus(void);
static void updatesystray(void);
static void updatesystrayicongeom(Client *i, int w, int h);
static void updatesystrayiconstate(Client *i, XPropertyEvent *ev);
static void updatetitle(Client *c);
static void updatepreview(void);
static void updatewindowtype(Client *c);
static void updatewmhints(Client *c);
static void view(const Arg *arg);
static void warp(const Client *c);
static void viewnextempty(const Arg *arg);
static void viewprevempty(const Arg *arg);
static void viewnext(const Arg *arg);
static void viewprev(const Arg *arg);
static Client *wintoclient(Window w);
static Monitor *wintomon(Window w);
static void winview(const Arg* arg);
static Client *wintosystrayicon(Window w);
static int xerror(Display *dpy, XErrorEvent *ee);
static int xerrordummy(Display *dpy, XErrorEvent *ee);
static int xerrorstart(Display *dpy, XErrorEvent *ee);
static void zoom(const Arg *arg);
static void focusmaster(const Arg *arg);

static pid_t getparentprocess(pid_t p);
static int isdescprocess(pid_t p, pid_t c);
static Client *swallowingclient(Window w);
static Client *termforwin(const Client *c);
static pid_t winpid(Window w);

/* variables */
static Systray *systray = NULL;
static Client *prevzoom = NULL;
static const char broken[] = "broken";
static char stext[1024];
static int screen;
static int sw, sh;           /* X display screen geometry width, height */
static int bh, blw = 0;      /* bar geometry */
static int th = 0;           /* tab bar geometry */
static int lrpad;            /* sum of left and right padding for text */
static int (*xerrorxlib)(Display *, XErrorEvent *);
static unsigned int numlockmask = 0;
static void (*handler[LASTEvent]) (XEvent *) = {
    [ButtonPress] = buttonpress,
    [ClientMessage] = clientmessage,
    [ConfigureRequest] = configurerequest,
    [ConfigureNotify] = configurenotify,
    [DestroyNotify] = destroynotify,
    [Expose] = expose,
    [FocusIn] = focusin,
    [KeyPress] = keypress,
    [KeyRelease] = keypress,
    [MappingNotify] = mappingnotify,
    [MapRequest] = maprequest,
	[MotionNotify] = motionnotify,
    [PropertyNotify] = propertynotify,
    [ResizeRequest] = resizerequest,
    [UnmapNotify] = unmapnotify
};
static Atom wmatom[WMLast], netatom[NetLast], motifatom, xatom[XLast], clientatom[ClientLast];
static int restart = 0;
static int running = 1;
static Cur *cursor[CurLast];
static Clr **scheme;
static Display *dpy;
static Drw *drw;
static Monitor *mons, *selmon, *prevmon;
static Window root, wmcheckwin;
unsigned int currentkey = 0;

static xcb_connection_t *xcon;

/* configuration, allows nested code to access above variables */
#include "config.h"

unsigned int tagw[LENGTH(tags)];

/* compile-time check if all tags fit into an unsigned int bit array. */
struct NumTags { char limitexceeded[LENGTH(tags) > 31 ? -1 : 1]; };

void
applyrules(Client *c)
{
    const char *class, *instance;
    Atom wintype;
    unsigned int i, newtagset;
    const Rule *r;
    Monitor *m;
    XClassHint ch = { NULL, NULL };

    /* rule matching */
    c->iscentered = 0;
    c->isfloating = 0;
    c->tags = 0;
    c->scratchkey = 0;
    XGetClassHint(dpy, c->win, &ch);
    class    = ch.res_class ? ch.res_class : broken;
    instance = ch.res_name  ? ch.res_name  : broken;
    wintype  = getatomprop(c, netatom[NetWMWindowType], XA_ATOM);

    if (strstr(class, "Steam") || strstr(class, "steam_app_"))
        c->issteam = 1;

    for (i = 0; i < LENGTH(rules); i++) {
        r = &rules[i];
        if ((!r->title || strstr(c->name, r->title))
        && (!r->class || strstr(class, r->class))
        && (!r->instance || strstr(instance, r->instance))
        && (!r->wintype || wintype == XInternAtom(dpy, r->wintype, False)))
        {
            c->isterminal = r->isterminal;
            c->noswallow  = r->noswallow;
            c->isfloating = r->isfloating;
            c->iscentered = r->iscentered;
            c->ispermanent = r->ispermanent;
            c->tags |= r->tags;
            c->scratchkey = r->scratchkey;

            if (!c->scratchkey && selmon->restart)
                c->scratchkey = getatomprop(c, clientatom[Scratchkey], AnyPropertyType);

            for (m = mons; m && m->num != r->monitor; m = m->next);
            if (m)
                c->mon = m;

            if (r->switchtag) {
                selmon = c->mon;
                if (r->switchtag == 2 || r->switchtag == 4)
                    newtagset = c->mon->tagset[c->mon->seltags] ^ c->tags;
                else
                    newtagset = c->tags;

                if (newtagset && !(c->tags & c->mon->tagset[c->mon->seltags])) {
                    if (r->switchtag == 3 || r->switchtag == 4)
                        c->switchtag = c->mon->tagset[c->mon->seltags];
                    if (r->switchtag == 1 || r->switchtag == 3)
                        view(&((Arg) { .ui = newtagset }));
                    else {
                        c->mon->tagset[c->mon->seltags] = newtagset;
                        arrange(c->mon);
                    }
                }
            }
        }
    }
    if (ch.res_class)
        XFree(ch.res_class);
    if (ch.res_name)
        XFree(ch.res_name);

    if (getatomprop(c, clientatom[Tag], AnyPropertyType))
        c->tags = getatomprop(c, clientatom[Tag], AnyPropertyType);
    // else if (c->scratchkey)
    //     c->tags = 0;
    else if (c->tags & TAGMASK)
        c->tags = c->tags & TAGMASK;
    else if (c->mon->tagset[c->mon->seltags])
        c->tags = c->mon->tagset[c->mon->seltags];
    else
        c->tags = 1;
}

int
applysizehints(Client *c, int *x, int *y, int *w, int *h, int interact)
{
    int baseismin;
    Monitor *m = c->mon;

    /* set minimum possible */
    *w = MAX(1, *w);
    *h = MAX(1, *h);
    if (interact) {
        if (*x > sw)
            *x = sw - WIDTH(c);
        if (*y > sh)
            *y = sh - HEIGHT(c);
        if (*x + *w + 2 * c->bw < 0)
            *x = 0;
        if (*y + *h + 2 * c->bw < 0)
            *y = 0;
    } else {
        if (*x >= m->wx + m->ww)
            *x = m->wx + m->ww - WIDTH(c);
        if (*y >= m->wy + m->wh)
            *y = m->wy + m->wh - HEIGHT(c);
        if (*x + *w + 2 * c->bw <= m->wx)
            *x = m->wx;
        if (*y + *h + 2 * c->bw <= m->wy)
            *y = m->wy;
    }
    if (*h < bh)
        *h = bh;
    if (*w < bh)
        *w = bh;
    if (resizehints || c->isfloating || !c->mon->lt[c->mon->sellt]->arrange) {
        /* see last two sentences in ICCCM 4.1.2.3 */
        baseismin = c->basew == c->minw && c->baseh == c->minh;
        if (!baseismin) { /* temporarily remove base dimensions */
            *w -= c->basew;
            *h -= c->baseh;
        }
        /* adjust for aspect limits */
        if (c->mina > 0 && c->maxa > 0) {
            if (c->maxa < (float)*w / *h)
                *w = *h * c->maxa + 0.5;
            else if (c->mina < (float)*h / *w)
                *h = *w * c->mina + 0.5;
        }
        if (baseismin) { /* increment calculation requires this */
            *w -= c->basew;
            *h -= c->baseh;
        }
        /* adjust for increment value */
        if (c->incw)
            *w -= *w % c->incw;
        if (c->inch)
            *h -= *h % c->inch;
        /* restore base dimensions */
        *w = MAX(*w + c->basew, c->minw);
        *h = MAX(*h + c->baseh, c->minh);
        if (c->maxw)
            *w = MIN(*w, c->maxw);
        if (c->maxh)
            *h = MIN(*h, c->maxh);
    }
    return *x != c->x || *y != c->y || *w != c->w || *h != c->h;
}

void
arrange(Monitor *m)
{
    if (m)
        showhide(m->stack);
    else for (m = mons; m; m = m->next)
        showhide(m->stack);
    if (m) {
        arrangemon(m);
        restack(m);
    } else for (m = mons; m; m = m->next)
        arrangemon(m);
}

void
arrangemon(Monitor *m) {
    updatebarpos(m);
    XMoveResizeWindow(dpy, m->tabwin, m->wx, m->ty, m->ww, th);
    strncpy(m->ltsymbol, m->lt[m->sellt]->symbol, sizeof m->ltsymbol);
    if (m->lt[m->sellt]->arrange)
        m->lt[m->sellt]->arrange(m);
}

void
attach(Client *c)
{
    c->next = c->mon->clients;
    c->mon->clients = c;
}
void
attachBelow(Client *c)
{
    //If there is nothing on the monitor or the selected client is floating, attach as normal
    if (c->mon->sel == NULL || c->mon->sel == c || c->mon->sel->isfloating) {
        attach(c);
        return;
    }

    //Set the new client's next property to the same as the currently selected clients next
    c->next = c->mon->sel->next;
    //Set the currently selected clients next property to the new client
    c->mon->sel->next = c;

}

void
attachstack(Client *c)
{
    c->snext = c->mon->stack;
    c->mon->stack = c;
}

void
swallow(Client *p, Client *c)
{

    if (c->noswallow || c->isterminal)
        return;
    if (c->noswallow && !swallowfloating && c->isfloating)
        return;

    detach(c);
    detachstack(c);

    setclientstate(c, WithdrawnState);
    XUnmapWindow(dpy, p->win);

    p->swallowing = c;
    c->mon = p->mon;

    Window w = p->win;
    p->win = c->win;
    c->win = w;
    updatetitle(p);
    XMoveResizeWindow(dpy, p->win, p->x, p->y, p->w, p->h);
    arrange(p->mon);
    configure(p);
    updateclientlist();
}

void
unswallow(Client *c)
{
    c->win = c->swallowing->win;

    free(c->swallowing);
    c->swallowing = NULL;

    /* unfullscreen the client */
    setfullscreen(c, 0);
    updatetitle(c);
    arrange(c->mon);
    XMapWindow(dpy, c->win);
    XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
    setclientstate(c, NormalState);
    focus(NULL);
    arrange(c->mon);
}

void
buttonpress(XEvent *e)
{
    unsigned int i, x, click, occ = 0, n = 0;
    Arg arg = {0};
    Client *c;
    Monitor *m;
    XButtonPressedEvent *ev = &e->xbutton;

    click = ClkRootWin;

    /* focus monitor if necessary */
    if ((m = wintomon(ev->window)) && m != selmon
        && (focusonwheel || (ev->button != Button4 && ev->button != Button5))) {
        unfocus(selmon->sel, 1);
        if (m != selmon)
            prevmon = selmon;
        selmon = m;
        focus(NULL);
    }
    for (c = selmon->clients; c; c = c->next)
        if (ISVISIBLE(c))
            n++;

    if (ev->window == selmon->barwin) {
        if (selmon->previewshow) {
            XUnmapWindow(dpy, selmon->tagwin);
            selmon->previewshow = 0;
        }
        i = 0;
        x = LENGTH(selmon->ltsymbol) + blw - 15;

        if (selmon->vactag) {
            for (c = selmon->clients; c; c = c->next)
                occ |= c->tags == 255 ? 0 : c->tags;
            do {
                /* do not reserve space for vacant tags */
                if (!(occ & 1 << i || selmon->tagset[selmon->seltags] & 1 << i))
                    continue;
                x += tagw[i];
            } while (ev->x >= x && ++i < LENGTH(tags));

            if (ev->x < LENGTH(selmon->ltsymbol) + blw) {
                click = ClkLtSymbol;
            } else if (i < LENGTH(tags)) {
                click = ClkTagBar;
                arg.ui = 1 << i;
            } else if (ev->x < x - 20 + blw && n > 1) {
                click = ClkNumSymbol;
            } else
                click = ClkStatusText;
        } else {
            do
                x += tagw[i];
            while (ev->x >= x && ++i < LENGTH(tags));

            if (ev->x < LENGTH(selmon->ltsymbol) + blw) {
                click = ClkLtSymbol;
            } else if (i < LENGTH(tags)) {
                click = ClkTagBar;
                arg.ui = 1 << i;
            } else if (ev->x < x - 5 + blw && n > 1) {
                click = ClkNumSymbol;
            } else
                click = ClkStatusText;
        }
    }
    if (ev->window == selmon->tabwin) {
        i = 0; x = 0;
        for(c = selmon->clients; c; c = c->next){
            if (!ISVISIBLE(c)) continue;
            x += selmon->tab_widths[i];
            if (ev->x > x)
                ++i;
            else
                break;
            if (i >= selmon->ntabs) break;
        }
        if (c) {
            click = ClkTabBar;
            arg.ui = i;
        }
    }
    else if ((c = wintoclient(ev->window))) {
        if (focusonwheel || (ev->button != Button4 && ev->button != Button5))
            focus(c);
        XAllowEvents(dpy, ReplayPointer, CurrentTime);
        click = ClkClientWin;
    }
    for (i = 0; i < LENGTH(buttons); i++)
        if (click == buttons[i].click && buttons[i].func && buttons[i].button == ev->button
        && CLEANMASK(buttons[i].mask) == CLEANMASK(ev->state)){
            buttons[i].func(((click == ClkTagBar || click == ClkTabBar) && buttons[i].arg.i == 0) ? &arg : &buttons[i].arg);
        }
}

void
checkotherwm(void)
{
    xerrorxlib = XSetErrorHandler(xerrorstart);
    /* this causes an error if some other window manager is running */
    XSelectInput(dpy, DefaultRootWindow(dpy), SubstructureRedirectMask);
    XSync(dpy, False);
    XSetErrorHandler(xerror);
    XSync(dpy, False);
}

void
cleanup(void)
{
    Arg a = {.ui = ~0};
    Layout foo = { "", NULL };
    Monitor *m;
    size_t i;

    view(&a);
    selmon->lt[selmon->sellt] = &foo;
    for (m = mons; m; m = m->next)
        while (m->stack)
            unmanage(m->stack, 0);
    XUngrabKey(dpy, AnyKey, AnyModifier, root);
    while (mons)
        cleanupmon(mons);
    if (showsystray) {
        XUnmapWindow(dpy, systray->win);
        XDestroyWindow(dpy, systray->win);
        free(systray);
    }
    for (i = 0; i < CurLast; i++)
        drw_cur_free(drw, cursor[i]);
    for (i = 0; i < LENGTH(colors) + 1; i++)
        free(scheme[i]);
     free(scheme);
    XDestroyWindow(dpy, wmcheckwin);
    drw_free(drw);
    XSync(dpy, False);
    XSetInputFocus(dpy, PointerRoot, RevertToPointerRoot, CurrentTime);
    XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
}

void
cleanupmon(Monitor *mon)
{
    Monitor *m;
    size_t i;

    if (mon == mons)
        mons = mons->next;
    else {
        for (m = mons; m && m->next != mon; m = m->next);
        m->next = mon->next;
    }
    for (i = 0; i < LENGTH(tags); i++)
        if (mon->tagmap[i])
            XFreePixmap(dpy, mon->tagmap[i]);
    XUnmapWindow(dpy, mon->barwin);
    XDestroyWindow(dpy, mon->barwin);
    XUnmapWindow(dpy, mon->tabwin);
    XDestroyWindow(dpy, mon->tabwin);
    XUnmapWindow(dpy, mon->tagwin);
    XDestroyWindow(dpy, mon->tagwin);
    free(mon->pertag);
    free(mon);
}

void
clientmessage(XEvent *e)
{
    XWindowAttributes wa;
    XSetWindowAttributes swa;
    XClientMessageEvent *cme = &e->xclient;
    Client *c = wintoclient(cme->window);
    unsigned int i;

    if (showsystray && cme->window == systray->win && cme->message_type == netatom[NetSystemTrayOP]) {
        /* add systray icons */
        if (cme->data.l[1] == SYSTEM_TRAY_REQUEST_DOCK) {
            if (!(c = (Client *)calloc(1, sizeof(Client))))
                die("fatal: could not malloc() %u bytes\n", sizeof(Client));
            if (!(c->win = cme->data.l[2])) {
                free(c);
                return;
            }
            c->mon = selmon;
            c->next = systray->icons;
            systray->icons = c;
            if (!XGetWindowAttributes(dpy, c->win, &wa)) {
                /* use sane defaults */
                wa.width = bh;
                wa.height = bh;
                wa.border_width = 0;
            }
            c->x = c->oldx = c->y = c->oldy = 0;
            c->w = c->oldw = wa.width;
            c->h = c->oldh = wa.height;
            c->oldbw = wa.border_width;
            c->bw = 0;
            c->isfloating = True;
            /* reuse tags field as mapped status */
            c->tags = 1;
            updatesizehints(c);
            updatesystrayicongeom(c, wa.width, wa.height);
            XAddToSaveSet(dpy, c->win);
            XSelectInput(dpy, c->win, StructureNotifyMask | PropertyChangeMask | ResizeRedirectMask);
            XReparentWindow(dpy, c->win, systray->win, 0, 0);
            /* use parents background color */
            swa.background_pixel  = scheme[SchemeNorm][ColBg].pixel;
            XChangeWindowAttributes(dpy, c->win, CWBackPixel, &swa);
            sendevent(c->win, netatom[Xembed], StructureNotifyMask, CurrentTime, XEMBED_EMBEDDED_NOTIFY, 0 , systray->win, XEMBED_EMBEDDED_VERSION);
            /* FIXME not sure if I have to send these events, too */
            sendevent(c->win, netatom[Xembed], StructureNotifyMask, CurrentTime, XEMBED_FOCUS_IN, 0 , systray->win, XEMBED_EMBEDDED_VERSION);
            sendevent(c->win, netatom[Xembed], StructureNotifyMask, CurrentTime, XEMBED_WINDOW_ACTIVATE, 0 , systray->win, XEMBED_EMBEDDED_VERSION);
            sendevent(c->win, netatom[Xembed], StructureNotifyMask, CurrentTime, XEMBED_MODALITY_ON, 0 , systray->win, XEMBED_EMBEDDED_VERSION);
            XSync(dpy, False);
            resizebarwin(selmon);
            updatesystray();
            setclientstate(c, NormalState);
        }
        return;
    }

    if (!c)
        return;
    if (cme->message_type == netatom[NetWMState]) {
        if (cme->data.l[1] == netatom[NetWMFullscreen]
        || cme->data.l[2] == netatom[NetWMFullscreen])
            setfullscreen(c, (cme->data.l[0] == 1 /* _NET_WM_STATE_ADD    */
                || (cme->data.l[0] == 2 /* _NET_WM_STATE_TOGGLE */ && !c->isfullscreen)));
    } else if (cme->message_type == netatom[NetActiveWindow]) {
        if (c->tags & c->mon->tagset[c->mon->seltags]) {
            focus(c);
            warp(c);
        } else {
            for (i = 0; i < LENGTH(tags) && !((1 << i) & c->tags); i++);
            if (i < LENGTH(tags)) {
                if (c != selmon->sel)
                    unfocus(selmon->sel, 0);
                selmon = c->mon;
                if (((1 << i) & TAGMASK) != selmon->tagset[selmon->seltags])
                    view(&((Arg) { .ui = 1 << i }));
                focus(c);
                restack(selmon);
            }
        }
    }
}

void
configure(Client *c)
{
    XConfigureEvent ce;

    ce.type = ConfigureNotify;
    ce.display = dpy;
    ce.event = c->win;
    ce.window = c->win;
    ce.x = c->x;
    ce.y = c->y;
    ce.width = c->w;
    ce.height = c->h;
    ce.border_width = c->bw;
    ce.above = None;
    ce.override_redirect = False;
    XSendEvent(dpy, c->win, False, StructureNotifyMask, (XEvent *)&ce);
}

void
configurenotify(XEvent *e)
{
    Monitor *m;
    Client *c;
    XConfigureEvent *ev = &e->xconfigure;
    int dirty;

    /* TODO: updategeom handling sucks, needs to be simplified */
    if (ev->window == root) {
        dirty = (sw != ev->width || sh != ev->height);
        sw = ev->width;
        sh = ev->height;
        if (updategeom() || dirty) {
            drw_resize(drw, sw, bh);
            updatebars();
            for (m = mons; m; m = m->next) {
                for (c = m->clients; c; c = c->next)
                    if (c->isfullscreen)
                        resizeclient(c, m->mx, m->my, m->mw, m->mh);
                resizebarwin(m);
            }
            focus(NULL);
            arrange(NULL);
        }
    }
}

void
configurerequest(XEvent *e)
{
    Client *c;
    Monitor *m;
    XConfigureRequestEvent *ev = &e->xconfigurerequest;
    XWindowChanges wc;

    if ((c = wintoclient(ev->window))) {
        if (ev->value_mask & CWBorderWidth)
            c->bw = ev->border_width;
        else if (c->isfloating || !selmon->lt[selmon->sellt]->arrange) {
            m = c->mon;
            if (!c->issteam) {
                if (ev->value_mask & CWX) {
                    c->oldx = c->x;
                    c->x = m->mx + ev->x;
                }
                if (ev->value_mask & CWY) {
                    c->oldy = c->y;
                    c->y = m->my + ev->y;
                }
            }
            if (ev->value_mask & CWWidth) {
                c->oldw = c->w;
                c->w = ev->width;
            }
            if (ev->value_mask & CWHeight) {
                c->oldh = c->h;
                c->h = ev->height;
            }
            if ((c->x + c->w) > m->mx + m->mw && c->isfloating)
                c->x = m->mx + (m->mw / 2 - WIDTH(c) / 2); /* center in x direction */
            if ((c->y + c->h) > m->my + m->mh && c->isfloating)
                c->y = m->my + (m->mh / 2 - HEIGHT(c) / 2); /* center in y direction */
            if ((ev->value_mask & (CWX|CWY)) && !(ev->value_mask & (CWWidth|CWHeight)))
                configure(c);
            if (ISVISIBLE(c))
                XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
            else
                c->needresize = 1;
        } else
            configure(c);
    } else {
        wc.x = ev->x;
        wc.y = ev->y;
        wc.width = ev->width;
        wc.height = ev->height;
        wc.border_width = ev->border_width;
        wc.sibling = ev->above;
        wc.stack_mode = ev->detail;
        XConfigureWindow(dpy, ev->window, ev->value_mask, &wc);
    }
    XSync(dpy, False);
}

Monitor *
createmon(void)
{
    Monitor *m, *mon;
    int j;
    unsigned long tmp;

    m = ecalloc(1, sizeof(Monitor));
    m->tagset[0] = m->tagset[1] = startontag ? 1 : 0;
    m->mfact = tagrules[0][0].mfact;
    m->nmaster = tagrules[0][0].nmaster;
    m->showbar = tagrules[0][0].showbar;
    m->showtab = showtab;
    m->topbar = tagrules[0][0].topbar;
    m->toptab = toptab;
    m->borderpx = tagrules[0][0].borderpx;
    m->ntabs = 0;
    m->gappih = tagrules[0][0].gapih;
    m->gappiv = tagrules[0][0].gapiv;
    m->gappoh = tagrules[0][0].gapoh;
    m->gappov = tagrules[0][0].gapov;
    m->smartgaps = tagrules[0][0].smartgaps;
    m->vactag = tagrules[0][0].vacant;
    m->vp = tagrules[0][0].vpad;
    m->sp = tagrules[0][0].spad;
    m->lt[0] = &layouts[0];
    m->lt[1] = &layouts[1 % LENGTH(layouts)];

    strncpy(m->ltsymbol, layouts[0].symbol, sizeof m->ltsymbol);
    if (!(m->pertag = (Pertag *)calloc(1, sizeof(Pertag))))
        die("fatal: could not malloc() %u bytes\n", sizeof(Pertag));
    m->pertag->curtag = m->pertag->prevtag = 1;

    for (j = 0, mon = mons; mon; mon = mon->next, j++);
    if (j >= RULENMONS)
        j = 0;

    m->num = j;
    tmp = gettagdata(m, "_DWN_MONITOR_TAGS_%u", -1);
    if (tmp != ULONG_MAX) {
        m->tagset[m->seltags] = tmp;
        m->pertag->curtag = m->tagset[m->seltags];
    }

    for (int i = 0; i < LENGTH(tags); i++) {
        tmp = gettagdata(m, "_DWN_MONITOR_PERTAG_%u_%u_1", i);
        m->restart = !((tmp >> 27) & 0x1);

        m->pertag->nmasters[i] = m->restart ? tmp & 0xF : tagrules[j][i].nmaster;
        m->pertag->mfacts[i] = m->restart ? (float)((tmp >> 4) & 0x7F) / 100 : tagrules[j][i].mfact;
        m->pertag->ltidx[i] = m->restart ? (tmp >> 23) & 0xF : tagrules[j][i].layout;
        m->pertag->ltidxs[i][0] = &layouts[m->pertag->ltidx[i]];
        m->pertag->ltidxs[i][1] = &layouts[m->pertag->ltidx[i]];
        m->pertag->showbars[i] = m->restart ? (tmp >> 11) & 0x1 : tagrules[j][i].showbar;
        m->pertag->topbar[i] = m->restart ? (tmp >> 12) & 0x1 : tagrules[j][i].topbar;
        m->pertag->enablegaps[i] = m->restart ? (tmp >> 13) & 0x1: 1;
        m->pertag->smartgaps[i] = m->restart ? (tmp >> 14) & 0x1: tagrules[j][i].smartgaps;
        m->pertag->tpadding[i] = m->restart ? (tmp >> 15) & 0x1: tagrules[j][i].tpad;
        m->pertag->vactags[i] = m->restart ? (tmp >> 16) & 0x1: tagrules[j][i].vacant;
        m->pertag->borderpx[i] = m->restart ? (tmp >> 17) & 0x3F: tagrules[j][i].borderpx;

        if (m->restart && m->pertag->curtag & 1 << i) {
            m->nmaster = m->pertag->nmasters[i];
            m->mfact = m->pertag->mfacts[i];
            m->lt[0] = m->pertag->ltidxs[i][0];
            m->lt[1] = m->pertag->ltidxs[i][1];
            m->showbar = m->pertag->showbars[i];
            m->topbar = m->pertag->topbar[i];
            m->smartgaps = m->pertag->smartgaps[i];
            m->vactag = m->pertag->vactags[i];
            m->borderpx = m->pertag->borderpx[i];
        }

        tmp = gettagdata(m, "_DWN_MONITOR_PERTAG_%u_%u_2", i);

        m->pertag->vertpd[i] = m->restart ? (tmp & 0x7F): tagrules[j][i].vpad;
        m->pertag->sidepd[i] = m->restart ? (tmp >> 7) & 0x7F: tagrules[j][i].spad;

        if (m->restart && m->pertag->curtag & 1 << i) {
            m->vp = m->pertag->vertpd[i];
            m->sp = m->pertag->sidepd[i];
        }

        tmp = gettagdata(m, "_DWN_MONITOR_PERTAG_%u_%u_GAPS", i);

        if (m->restart) {
            m->pertag->gaps[i] = ((tmp & 0x7F) << 0) | (((tmp >> 7) & 0x7F) << 8) | (((tmp >> 14) & 0x7F) << 16) | (((tmp >> 21) & 0x7F) << 24);

            if (m->pertag->curtag & 1 << i) {
                m->gappoh = (m->pertag->gaps[i] & 0xff) >> 0;
                m->gappov = (m->pertag->gaps[i] & 0xff00) >> 8;
                m->gappih = (m->pertag->gaps[i] & 0xff0000) >> 16;
                m->gappiv = (m->pertag->gaps[i] & 0xff000000) >> 24;
            }
        } else {
            m->pertag->gaps[i] = ((tagrules[j][i].gapoh & 0xFF) << 0) | ((tagrules[0][i].gapov & 0xFF) << 8) | ((tagrules[0][i].gapih & 0xFF) << 16) | ((tagrules[0][i].gapiv & 0xFF) << 24);
        }
    }

    return m;
}

void
cyclelayout(const Arg *arg) {
    Layout *l;
    for(l = (Layout *)layouts; l != selmon->lt[selmon->sellt]; l++);
    if (arg->i > 0) {
        if (l->symbol && (l + 1)->symbol)
            setlayout(&((Arg) { .v = (l + 1) }));
        else
            setlayout(&((Arg) { .v = layouts }));
    } else {
        if (l != layouts && (l - 1)->symbol)
            setlayout(&((Arg) { .v = (l - 1) }));
        else
            setlayout(&((Arg) { .v = &layouts[LENGTH(layouts) - 2] }));
    }
}

void
destroynotify(XEvent *e)
{
    Client *c;
    XDestroyWindowEvent *ev = &e->xdestroywindow;

    if ((c = wintoclient(ev->window)))
        unmanage(c, 1);
    else if ((c = wintosystrayicon(ev->window))) {
        removesystrayicon(c);
        resizebarwin(selmon);
        updatesystray();
    } else if ((c = swallowingclient(ev->window)))
        unmanage(c->swallowing, 1);
}

void
detach(Client *c)
{
    Client **tc;

    for (tc = &c->mon->clients; *tc && *tc != c; tc = &(*tc)->next);
    *tc = c->next;
}

void
detachstack(Client *c)
{
    Client **tc, *t;

    for (tc = &c->mon->stack; *tc && *tc != c; tc = &(*tc)->snext);
    *tc = c->snext;

    if (c == c->mon->sel) {
        for (t = c->mon->stack; t && !ISVISIBLE(t); t = t->snext);
        c->mon->sel = t;
    }
}

Monitor *
dirtomon(int dir)
{
    Monitor *m = NULL;

    if (dir > 0) {
        if (!(m = selmon->next))
            m = mons;
    } else if (selmon == mons)
        for (m = mons; m->next; m = m->next);
    else
        for (m = mons; m->next != selmon; m = m->next);
    return m;
}

int
drawstatusbar(Monitor *m, int bh, char* stext) {
    int ret, i, w, x, len;
    short isCode = 0;
    char *text;
    char *p;

    len = strlen(stext) + 1 ;
    if (!(text = (char*) malloc(sizeof(char)*len)))
        die("malloc");
    p = text;
    memcpy(text, stext, len);

    /* compute width of the status text */
    w = 0;
    i = -1;
    while (text[++i]) {
        if (text[i] == '^') {
            if (!isCode) {
                isCode = 1;
                text[i] = '\0';
                w += TEXTW(text) - lrpad;
                text[i] = '^';
                if (text[++i] == 'f')
                    w += atoi(text + ++i);
            } else {
                isCode = 0;
                text = text + i + 1;
                i = -1;
            }
        }
    }

    if (!isCode)
        w += TEXTW(text) - lrpad;
    else
        isCode = 0;
    text = p;

    w += 2; /* 1px padding on both sides */
    ret = m->ww - w;
    x = ret - (m == systraytomon(m) ? getsystraywidth() : 0);

    drw_setscheme(drw, scheme[LENGTH(colors)]);
    drw->scheme[ColFg] = scheme[SchemeStatus][ColFg];
    drw->scheme[ColBg] = scheme[SchemeStatus][1];
    drw_rect(drw, x, 0, w, bh, 1, 1);
    x++;

    /* process status text */
    i = -1;
    while (text[++i]) {
        if (text[i] == '^' && !isCode) {
            isCode = 1;

            text[i] = '\0';
            w = TEXTW(text) - lrpad;
            drw_text(drw, x - 2 * m->sp, 0, w, bh, 0, text, 0);
            x += w;

            /* process code */
            while (text[++i] != '^') {
                if (text[i] == 'c') {
                    char buf[8];
                    memcpy(buf, (char*)text+i+1, 7);
                    buf[7] = '\0';
                    drw_clr_create(drw, &drw->scheme[ColFg], buf);
                    i += 7;
                } else if (text[i] == 'b') {
                    char buf[8];
                    memcpy(buf, (char*)text+i+1, 7);
                    buf[7] = '\0';
                    drw_clr_create(drw, &drw->scheme[1], buf);
                    i += 7;
                } else if (text[i] == 'd') {
                    drw->scheme[ColFg] = scheme[SchemeStatus][ColFg];
                    drw->scheme[ColBg] = scheme[SchemeStatus][1];
                } else if (text[i] == 'r') {
                    int rx = atoi(text + ++i);
                    while (text[++i] != ',');
                    int ry = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rw = atoi(text + ++i);
                    while (text[++i] != ',');
                    int rh = atoi(text + ++i);

                    drw_rect(drw, rx + x, ry, rw, rh, 1, 0);
                } else if (text[i] == 'f') {
                    x += atoi(text + ++i);
                }
            }

            text = text + i + 1;
            i=-1;
            isCode = 0;
        }
    }

    if (!isCode) {
        w = TEXTW(text) - lrpad; /* 2px right padding */
        drw_text(drw, x - 2 * m->sp, 0, w, bh, lrpad / 2, text, 0);
    }

    drw_setscheme(drw, scheme[SchemeNorm]);
    free(p);

    return ret;
}

void
drawbar(Monitor *m)
{
    int indn, cnum = 0;
    int x, w, stw = 0;
    unsigned int i, occ = 0, urg = 0, n = 0;
    Client *c;
    char tagdisp[24];
    char *masterclientontag[LENGTH(tags)];
    char ntext[16];

    if (showsystray && m == systraytomon(m))
        stw = getsystraywidth();

    resizebarwin(m);

    if (!m->showbar)
        return;

    for (i = 0; i < LENGTH(tags); i++)
        masterclientontag[i] = NULL;

    for (c = m->clients; c; c = c->next) {
        if (m->vactag)
            occ |= c->tags == 255 ? 0 : c->tags;
        else 
            occ |= c->tags;
        if (c->isurgent)
            urg |= c->tags;
        if (ISVISIBLE(c))
            n++;
        for (i = 0; i < LENGTH(tags); i++)
            if (!masterclientontag[i] && c->tags & (1<<i)) {
                XClassHint ch = { NULL, NULL };
                XGetClassHint(dpy, c->win, &ch);
                masterclientontag[i] = ch.res_class;
                if (lcaselbl)
                    masterclientontag[i][0] = tolower(masterclientontag[i][0]);
            }
    }

    w = blw = TEXTW(m->ltsymbol);
    drw_setscheme(drw, scheme[m == selmon ? SchemeLtsymbol : SchemeInvLtsymbol]);
    x = drw_text(drw, 0, 0, w, bh, lrpad / 2, m->ltsymbol, 0);

    for (i = 0; i < LENGTH(tags); i++) {
        indn = 0;
        cnum = 0;
        if (m->vactag)
            if (!(occ & 1 << i || m->tagset[m->seltags] & 1 << i))
                continue;

        if (masterclientontag[i])
            snprintf(tagdisp, 24, ptagf, tags[i], masterclientontag[i]);
        else
            snprintf(tagdisp, 24, etagf, tags[i]);
        masterclientontag[i] = tagdisp;
        tagw[i] = w = TEXTW(masterclientontag[i]);
        if (m->vactag) {
            if (occ & 1 << i)
                drw_setscheme(drw, scheme[m == selmon ? m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeNorm : SchemeInv]);
            else
                drw_setscheme(drw, scheme[m == selmon ? m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeNorm : SchemeInv]);
        } else
            if (occ & 1 << i)
                drw_setscheme(drw, scheme[m == selmon ? m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeOccupied : SchemeInv]);
            else
                drw_setscheme(drw, scheme[m == selmon ? m->tagset[m->seltags] & 1 << i ? SchemeSel : SchemeNorm : SchemeInv]);

        drw_text(drw, x, 0, w + 2 * m->sp, bh, lrpad / 2, masterclientontag[i], urg & 1 << i);

        if (underlinetags && ((underlinevacant) ? (m->vactag && m->tagset[m->seltags] & 1 << i) : (!m->vactag && (occ & 1 << i || m->tagset[m->seltags] & 1 << i))))
            drw_rect(drw, x + ulinepad, bh - ulinestroke - ulinevoffset, w - (ulinepad * 2), ulinestroke, 1, 0);

        for (c = m->clients; c; c = c->next)
            if (c->tags & (1 << i))
                cnum++;

        for (c = m->clients; c && cnum > 1; c = c->next)
            if (c->tags & (1 << i) && !(m->tagset[m->seltags] & 1 << i)) {
                drw_setscheme(drw, scheme[m->vactag ? SchemeClientVac : SchemeClient]);
                drw_rect(drw, x, 1 + (indn * 2), 4, 1, 3, urg & 1 << i);
                indn++;
            }

        x += w;
    }

    if (n > 1) {
        drw_setscheme(drw, scheme[SchemeClientNum]);
        snprintf(ntext, sizeof ntext, " {%u}", n);
        w = TEXTW(ntext);
        x = drw_text(drw, x, 0, w, bh, lrpad / 2, ntext, 0);
    }

    drw_setscheme(drw, scheme[SchemeStatus]);
    drw_rect(drw, x, 0, m->ww - x, bh, 1, 1);
    drawstatusbar(m, bh, stext);
    drw_map(drw, m->barwin, 0, 0, m->ww - stw, bh);
}

void
drawbars(void)
{
    Monitor *m;

    for (m = mons; m; m = m->next)
        drawbar(m);
}

void
drawtabs(void) {
    Monitor *m;

    for(m = mons; m; m = m->next)
        drawtab(m);
}

static int
cmpint(const void *p1, const void *p2) {
  /* The actual arguments to this function are "pointers to
     pointers to char", but strcmp(3) arguments are "pointers
     to char", hence the following cast plus dereference */
  return *((int*) p1) > * (int*) p2;
}


void
drawtab(Monitor *m) {
    Client *c;
    int i;
    int itag = -1;
    char view_info[50];
    int view_info_w = 0;
    int sorted_label_widths[MAXTABS];
    int tot_width;
    int maxsize = bh;
    int x = 0;
    int w = 0;

    //view_info: indicate the tag which is displayed in the view
    for(i = 0; i < LENGTH(tags); ++i){
      if ((selmon->tagset[selmon->seltags] >> i) & 1) {
        if (itag >= 0){ //more than one tag selected
          itag = -1;
          break;
        }
        itag = i;
      }
    }

    if (0 <= itag && itag < LENGTH(tags)){
      snprintf(view_info, sizeof view_info, "[%s]", tags[itag]);
    } else {
      strncpy(view_info, "[...]", sizeof view_info);
    }
    view_info[sizeof(view_info) - 1 ] = 0;
    view_info_w = TEXTW(view_info);
    tot_width = view_info_w;

    /* Calculates number of labels and their width */
    m->ntabs = 0;
    for (c = m->clients; c; c = c->next){
      if (!ISVISIBLE(c)) continue;
      m->tab_widths[m->ntabs] = TEXTW(c->name);
      tot_width += m->tab_widths[m->ntabs];
      ++m->ntabs;
      if (m->ntabs >= MAXTABS) break;
    }

    if (tot_width > m->ww){ //not enough space to display the labels, they need to be truncated
      memcpy(sorted_label_widths, m->tab_widths, sizeof(int) * m->ntabs);
      qsort(sorted_label_widths, m->ntabs, sizeof(int), cmpint);
      tot_width = view_info_w;
      for (i = 0; i < m->ntabs; ++i){
        if (tot_width + (m->ntabs - i) * sorted_label_widths[i] > m->ww)
          break;
        tot_width += sorted_label_widths[i];
      }
      maxsize = (m->ww - tot_width) / (m->ntabs - i);
    } else{
      maxsize = m->ww;
    }
    i = 0;
    for (c = m->clients; c; c = c->next){
      if (!ISVISIBLE(c)) continue;
      if (i >= m->ntabs) break;
      if (m->tab_widths[i] >  maxsize) m->tab_widths[i] = maxsize;
      w = m->tab_widths[i];
      drw_setscheme(drw, scheme[(c == m->sel) ? SchemeTabSel : SchemeTabNorm]);
      drw_text(drw, x, 0, w, th, 0, c->name, 0);
      x += w;
      ++i;
    }

    drw_setscheme(drw, scheme[SchemeTabNorm]);

    /* cleans interspace between window names and current viewed tag label */
    w = m->ww - view_info_w - x;
    drw_text(drw, x - 5, 0, w, th, 0, "", 0);

    /* view info */
    x += w;
    w = view_info_w;
    drw_text(drw, x, 0, w, th, 0, view_info, 0);

    drw_map(drw, m->tabwin, 0, 0, m->ww, th);
}

void
expose(XEvent *e)
{
    Monitor *m;
    XExposeEvent *ev = &e->xexpose;

    if (ev->count == 0 && (m = wintomon(ev->window))) {
        drawbar(m);
        drawtab(m);
        if (m == selmon)
            updatesystray();
    }
}

Client *
findbefore(Client *c)
{
    Client *tmp;
    if (c == selmon->clients)
        return NULL;
    for (tmp = selmon->clients; tmp && tmp->next != c; tmp = tmp->next);
    return tmp;
}

void
focus(Client *c)
{
    if (!c || !ISVISIBLE(c))
        for (c = selmon->stack; c && !ISVISIBLE(c); c = c->snext);
    if (selmon->sel && selmon->sel != c)
        unfocus(selmon->sel, 0);
    if (c) {
        if (c->mon != selmon) {
            prevmon = selmon;
            selmon = c->mon;
        }
        if (c->isurgent)
            seturgent(c, 0);
        detachstack(c);
        attachstack(c);
        grabbuttons(c, 1);
        if (c->issticky)
            XSetWindowBorder(dpy, c->win, scheme[SchemeSel][ColSticky].pixel);
        else if (c->isfloating)
            XSetWindowBorder(dpy, c->win, scheme[SchemeSel][ColFloat].pixel);
        else {
            for (unsigned int i = 0; i < 9; i++) {
                if (&layouts[i] == selmon->lt[selmon->sellt]) {
                    XSetWindowBorder(dpy, c->win, scheme[SchemeSelLayout][i].pixel);
                    break;
                }
            }
        }
        setfocus(c);
    } else {
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
    }
    selmon->sel = c;
    drawbars();
    drawtabs();
}

/* there are some broken focus acquiring clients needing extra handling */
void
focusin(XEvent *e)
{
    XFocusChangeEvent *ev = &e->xfocus;

    if (selmon->sel && ev->window != selmon->sel->win && wintoclient(ev->window))
        setfocus(selmon->sel);
}

void
focusmon(const Arg *arg)
{
    Monitor *m;

    if (!mons->next)
        return;
    if ((m = dirtomon(arg->i)) == selmon)
        return;
    unfocus(selmon->sel, 0);
    XWarpPointer(dpy, None, m->barwin, 0, 0, 0, 0, m->mw / 2, m->mh / 2);
    prevmon = selmon;
    selmon = m;
    focus(NULL);
}

void
focusstack(const Arg *arg)
{
    Client *c = NULL, *i;

    if (!selmon->sel || selmon->sel->isfullscreen)
        return;

    if (arg->i > 0) {
        for (c = selmon->sel->next; c && !ISVISIBLE(c); c = c->next);
        if (!c)
            for (c = selmon->clients; c && !ISVISIBLE(c); c = c->next);
    } else {
        for (i = selmon->clients; i != selmon->sel; i = i->next)
            if (ISVISIBLE(i))
                c = i;

        if (!c)
            for (; i; i = i->next)
                if (ISVISIBLE(i))
                    c = i;
    }

    if (c) {
        focus(c);
        restack(selmon);
    }
}

void
focuswin(const Arg* arg) {
    int iwin = arg->i;
    Client* c = NULL;

    for(c = selmon->clients; c && (iwin || !ISVISIBLE(c)) ; c = c->next) {
        if (ISVISIBLE(c)) --iwin;
    }

    if (c) {
        focus(c);
        restack(selmon);
    }
}

Atom
getatomprop(Client *c, Atom prop, Atom req)
{
    int di;
    unsigned long dl;
    unsigned char *p = NULL;
    Atom da, atom = None;
    /* FIXME getatomprop should return the number of items and a pointer to
     * the stored data instead of this workaround */
	if (XGetWindowProperty(dpy, c->win, prop, 0L, sizeof atom, False, req,
		&da, &di, &dl, &dl, &p) == Success && p) {
		atom = *(Atom *)p;
		if (da == xatom[XembedInfo] && dl == 2)
			atom = ((Atom *)p)[1];
		XFree(p);
	}
	return atom;
}

Client *
getclientundermouse(void)
{
	int ret, di;
	unsigned int dui;
	Window child, dummy;

	ret = XQueryPointer(dpy, root, &dummy, &child, &di, &di, &di, &di, &dui);
	if (!ret)
		return NULL;

	return wintoclient(child);
}

int
getrootptr(int *x, int *y)
{
    int di;
    unsigned int dui;
    Window dummy;

    return XQueryPointer(dpy, root, &dummy, &dummy, x, y, &di, &di, &dui);
}

long
getstate(Window w)
{
    int format;
    long result = -1;
    unsigned char *p = NULL;
    unsigned long n, extra;
    Atom real;

    if (XGetWindowProperty(dpy, w, wmatom[WMState], 0L, 2L, False, wmatom[WMState],
        &real, &format, &n, &extra, (unsigned char **)&p) != Success)
        return -1;
    if (n != 0)
        result = *p;
    XFree(p);
    return result;
}

unsigned int
getsystraywidth()
{
    unsigned int w = 0;
    Client *i;
    if (showsystray)
        for(i = systray->icons; i; w += i->w + systrayspacing, i = i->next) ;
    return w ? w + systrayspacing : 1;
}

int
gettextprop(Window w, Atom atom, char *text, unsigned int size)
{
    char **list = NULL;
    int n;
    XTextProperty name;

    if (!text || size == 0)
        return 0;
    text[0] = '\0';
    if (!XGetTextProperty(dpy, w, &name, atom) || !name.nitems)
        return 0;
    if (name.encoding == XA_STRING)
        strncpy(text, (char *)name.value, size - 1);
    else {
        if (XmbTextPropertyToTextList(dpy, &name, &list, &n) >= Success && n > 0 && *list) {
            strncpy(text, *list, size - 1);
            XFreeStringList(list);
        }
    }
    text[size - 1] = '\0';
    XFree(name.value);
    return 1;
}

void
goback(const Arg *arg)
{
    if (prevmon == NULL) {
        Arg a = {0};
        view(&a);
    } else if (prevmon != selmon) {
        unfocus(selmon->sel, 0);
        Monitor *p = selmon;
        selmon = prevmon;
        focus(NULL);
        prevmon = p;
    }
}

void
grabbuttons(Client *c, int focused)
{
    updatenumlockmask();
    {
        unsigned int i, j;
        unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
        XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
        if (!focused)
            XGrabButton(dpy, AnyButton, AnyModifier, c->win, False,
                BUTTONMASK, GrabModeSync, GrabModeSync, None, None);
        for (i = 0; i < LENGTH(buttons); i++)
            if (buttons[i].click == ClkClientWin)
                for (j = 0; j < LENGTH(modifiers); j++)
                    XGrabButton(dpy, buttons[i].button,
                        buttons[i].mask | modifiers[j],
                        c->win, False, BUTTONMASK,
                        GrabModeAsync, GrabModeSync, None, None);
    }
}

void
grabkeys(void)
{
    updatenumlockmask();
    {
        unsigned int i, k;
        unsigned int modifiers[] = { 0, LockMask, numlockmask, numlockmask|LockMask };
        KeyCode code;

        XUngrabKey(dpy, AnyKey, AnyModifier, root);
        for (i = 0; i < LENGTH(keychords); i++)
            if ((code = XKeysymToKeycode(dpy, keychords[i]->keys[currentkey].keysym)))
                for (k = 0; k < LENGTH(modifiers); k++)
                    XGrabKey(dpy, code, keychords[i]->keys[currentkey].mod | modifiers[k], root,
                             True, GrabModeAsync, GrabModeAsync);
        if (currentkey > 0)
            XGrabKey(dpy, XKeysymToKeycode(dpy, XK_Escape), AnyModifier, root, True, GrabModeAsync, GrabModeAsync);
    }
}

void
incnmaster(const Arg *arg)
{
    selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag] = MAX(selmon->nmaster + arg->i, 0);
    arrange(selmon);
}

#ifdef XINERAMA
static int
isuniquegeom(XineramaScreenInfo *unique, size_t n, XineramaScreenInfo *info)
{
    while (n--)
        if (unique[n].x_org == info->x_org && unique[n].y_org == info->y_org
        && unique[n].width == info->width && unique[n].height == info->height)
            return 0;
    return 1;
}
#endif /* XINERAMA */

void
keypress(XEvent *e)
{
    XEvent event = *e;
    unsigned int ran = 0;
    KeySym keysym;
    XKeyEvent *ev;

    Keychord *arr1[sizeof(keychords) / sizeof(Keychord*)];
    Keychord *arr2[sizeof(keychords) / sizeof(Keychord*)];
    memcpy(arr1, keychords, sizeof(keychords));
    Keychord **rpointer = arr1;
    Keychord **wpointer = arr2;
    
    size_t r = sizeof(keychords)/ sizeof(Keychord*);
    
    while (1) {
        ev = &event.xkey;
        keysym = XKeycodeToKeysym(dpy, (KeyCode)ev->keycode, 0);
        size_t w = 0;

        for (int i = 0; i < r; i++){
            if(keysym == (*(rpointer + i))->keys[currentkey].keysym
               && ev->type == (*(rpointer + 1))->keys[currentkey].type
               && CLEANMASK((*(rpointer + i))->keys[currentkey].mod) == CLEANMASK(ev->state)
               && (*(rpointer + i))->func) {
                if((*(rpointer + i))->n == currentkey +1){
                    (*(rpointer + i))->func(&((*(rpointer + i))->arg));
                    ran = 1;
            	}else{
                    *(wpointer + w) = *(rpointer + i);
                    w++;
                }
            }
        }

        currentkey++;

        if(w == 0 || ran == 1)
            break;

        grabkeys();

        while (running && !XNextEvent(dpy, &event) && !ran)
            if(event.type == KeyPress)
                break;

        r = w;
        Keychord **holder = rpointer;
        rpointer = wpointer;
        wpointer = holder;
    }

    currentkey = 0;
    grabkeys();
}

int
fake_signal(void)
{
    char fsignal[256];
    char indicator[9] = "fsignal:";
    char str_sig[50];
    char param[16];
    int i, len_str_sig, n, paramn;
    size_t len_fsignal, len_indicator = strlen(indicator);
    Arg arg;

    // Get root name property
    if (gettextprop(root, XA_WM_NAME, fsignal, sizeof(fsignal))) {
        len_fsignal = strlen(fsignal);

        // Check if this is indeed a fake signal
        if (len_indicator > len_fsignal ? 0 : strncmp(indicator, fsignal, len_indicator) == 0) {
            paramn = sscanf(fsignal+len_indicator, "%s%n%s%n", str_sig, &len_str_sig, param, &n);

            if (paramn == 1) arg = (Arg) {0};
            else if (paramn > 2) return 1;
            else if (strncmp(param, "i", n - len_str_sig) == 0)
                sscanf(fsignal + len_indicator + n, "%i", &(arg.i));
            else if (strncmp(param, "ui", n - len_str_sig) == 0)
                sscanf(fsignal + len_indicator + n, "%u", &(arg.ui));
            else if (strncmp(param, "f", n - len_str_sig) == 0)
                sscanf(fsignal + len_indicator + n, "%f", &(arg.f));
            else return 1;

            // Check if a signal was found, and if so handle it
            for (i = 0; i < LENGTH(signals); i++)
                if (strncmp(str_sig, signals[i].sig, len_str_sig) == 0 && signals[i].func)
                    signals[i].func(&(arg));

            // A fake signal was sent
            return 1;
        }
    }

    // No fake signal was sent, so proceed with update
    return 0;
}

void
killclient(const Arg *arg)
{
    if (!selmon->sel || selmon->sel->ispermanent)
        return;

    if (!sendevent(selmon->sel->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
        XGrabServer(dpy);
        XSetErrorHandler(xerrordummy);
        XSetCloseDownMode(dpy, DestroyAll);
        XKillClient(dpy, selmon->sel->win);
        XSync(dpy, False);
        XSetErrorHandler(xerror);
        XUngrabServer(dpy);
    }
}

void
killpermanent(const Arg *arg)
{
    if (!selmon->sel)
        return;
    if (!sendevent(selmon->sel->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
        XGrabServer(dpy);
        XSetErrorHandler(xerrordummy);
        XSetCloseDownMode(dpy, DestroyAll);
        XKillClient(dpy, selmon->sel->win);
        XSync(dpy, False);
        XSetErrorHandler(xerror);
        XUngrabServer(dpy);
    }
}

void
killunsel(const Arg *arg)
{
    Client *c = NULL;

    if (!selmon->sel)
        return;

    for (c = selmon->clients; c; c = c->next) {
        if (ISVISIBLE(c) && c != selmon->sel && !c->ispermanent &&
            !sendevent(c->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
            XGrabServer(dpy);
            XSetErrorHandler(xerrordummy);
            XSetCloseDownMode(dpy, DestroyAll);
            XKillClient(dpy, c->win);
            XSync(dpy, False);
            XSetErrorHandler(xerror);
            XUngrabServer(dpy);
        }
    }
}

void
manage(Window w, XWindowAttributes *wa)
{
    Client *c, *t = NULL, *term = NULL;
    Window trans = None;
    XWindowChanges wc;

    c = ecalloc(1, sizeof(Client));
    c->win = w;
    c->pid = winpid(w);
    /* geometry */
    c->x = c->oldx = wa->x;
    c->y = c->oldy = wa->y;
    c->w = c->oldw = wa->width;
    c->h = c->oldh = wa->height;
    c->oldbw = wa->border_width;
    c->cfact = 1.0;

    {
        int cfact = getatomprop(c, clientatom[Cfact], AnyPropertyType);

        if (cfact < 401 && cfact > 24)
            c->cfact = (float)cfact / 100;
    }

    {
        int sticky = getatomprop(c, clientatom[IsSticky], AnyPropertyType);

        c->issticky = sticky;
    }

    updatetitle(c);

    if (XGetTransientForHint(dpy, w, &trans) && (t = wintoclient(trans))) {
        c->mon = t->mon;
        c->tags = t->tags;
    } else {
        c->mon = selmon;
        applyrules(c);
        term = termforwin(c);
    }

    if (c->x + WIDTH(c) > c->mon->mx + c->mon->mw)
        c->x = c->mon->mx + c->mon->mw - WIDTH(c);
    if (c->y + HEIGHT(c) > c->mon->my + c->mon->mh)
        c->y = c->mon->my + c->mon->mh - HEIGHT(c);
    c->x = MAX(c->x, c->mon->mx);
    /* only fix client y-offset, if the client center might cover the bar */
    c->y = MAX(c->y, ((c->mon->by == c->mon->my) && (c->x + (c->w / 2) >= c->mon->wx)
        && (c->x + (c->w / 2) < c->mon->wx + c->mon->ww)) ? bh : c->mon->my);
    c->bw = c->mon->borderpx;

    wc.border_width = c->bw;
    XConfigureWindow(dpy, w, CWBorderWidth, &wc);
    if (c->issticky)
        XSetWindowBorder(dpy, w, scheme[SchemeSel][ColSticky].pixel);
    else if (c->isfloating)
        XSetWindowBorder(dpy, w, scheme[SchemeSel][ColFloat].pixel);
    else {
        for (unsigned int i = 0; i < 9; i++) {
            if (&layouts[i] == selmon->lt[selmon->sellt]) {
                XSetWindowBorder(dpy, w, scheme[SchemeNormLayout][i].pixel);
                break;
            }
        }
    }
    configure(c); /* propagates border_width, if size doesn't change */
	updatewindowtype(c);
    if (getatomprop(c, netatom[NetWMState], XA_ATOM) == netatom[NetWMFullscreen])
        setfullscreen(c, 1);
    updatesizehints(c);
    updatewmhints(c);
    c->sfx = c->x;
    c->sfy = c->y;
    c->sfw = c->w;
    c->sfh = c->h;
    if (c->iscentered) {
        c->x = c->mon->mx + (c->mon->mw - WIDTH(c)) / 2;
        c->y = c->mon->my + (c->mon->mh - HEIGHT(c)) / 2;
    }
    updatemotifhints(c);
    XSelectInput(dpy, w, EnterWindowMask|FocusChangeMask|PropertyChangeMask|StructureNotifyMask);
    grabbuttons(c, 0);
    if (!c->isfloating)
        c->isfloating = c->oldstate = trans != None || c->isfixed;
    if (getatomprop(c, clientatom[IsFloating], AnyPropertyType))
        c->isfloating = 1;
    if (c->isfloating) {
        XRaiseWindow(dpy, c->win);
        XSetWindowBorder(dpy, w, scheme[SchemeNorm][ColFloat].pixel);
    }
    if (attachbelow)
        attachBelow(c);
    else
        attach(c);
    attachstack(c);
    XChangeProperty(dpy, root, netatom[NetClientList], XA_WINDOW, 32, PropModeAppend,
        (unsigned char *) &(c->win), 1);
    XMoveResizeWindow(dpy, c->win, c->x + 2 * sw, c->y, c->w, c->h); /* some windows require this */
    setclientstate(c, NormalState);
    if (c->mon == selmon)
        unfocus(selmon->sel, 0);
    c->mon->sel = c;
    arrange(c->mon);
    XMapWindow(dpy, c->win);
    if (term)
        swallow(term, c);
    focus(NULL);
    if (getatomprop(c, clientatom[IsFullscreen], AnyPropertyType)) {
        setfullscreen(c, 0);
    }
}

void
mappingnotify(XEvent *e)
{
    XMappingEvent *ev = &e->xmapping;

    XRefreshKeyboardMapping(ev);
    if (ev->request == MappingKeyboard)
        grabkeys();
}

void
maprequest(XEvent *e)
{
    static XWindowAttributes wa;
    XMapRequestEvent *ev = &e->xmaprequest;
    Client *i;
    if ((i = wintosystrayicon(ev->window))) {
        sendevent(i->win, netatom[Xembed], StructureNotifyMask, CurrentTime, XEMBED_WINDOW_ACTIVATE, 0, systray->win, XEMBED_EMBEDDED_VERSION);
        resizebarwin(selmon);
        updatesystray();
    }

    if (!XGetWindowAttributes(dpy, ev->window, &wa))
        return;
    if (wa.override_redirect)
        return;
    if (!wintoclient(ev->window))
        manage(ev->window, &wa);
}

void
monocle(Monitor *m)
{
    Client *c;

    for (c = nexttiled(m->clients); c; c = nexttiled(c->next))
        resize(c, m->wx, m->wy, m->ww - 2 * c->bw, m->wh - 2 * c->bw, 0);
}

void
motionnotify(XEvent *e)
{
    static Monitor *mon = NULL;
    Monitor *m;
    Client *c;
    XMotionEvent *ev = &e->xmotion;
    unsigned int i, x, occ = 0;

    if (showpreview) {
        if (ev->window == selmon->barwin) {
            i = 0;
            x = LENGTH(selmon->ltsymbol) + blw - 15;

            if (selmon->vactag) {
                for (c = selmon->clients; c; c = c->next)
                    occ |= c->tags == 255 ? 0 : c->tags;
                do {
                    /* do not reserve space for vacant tags */
                    if (!(occ & 1 << i || selmon->tagset[selmon->seltags] & 1 << i))
                    continue;
                        x += tagw[i];
                } while (ev->x >= x && ++i < LENGTH(tags));

                if (ev->x < LENGTH(m->ltsymbol) + blw) {
                } else if (i < LENGTH(tags)) {
                    if ((i + 1) != selmon->previewshow && !(selmon->tagset[selmon->seltags] & 1 << i)) {
                        selmon->previewshow = i + 1;
                        showtagpreview(i);
                    } else if (selmon->tagset[selmon->seltags] & 1 << i) {
                        selmon->previewshow = 0;
                        showtagpreview(0);
                    }
                } else if (selmon->previewshow != 0) {
                    selmon->previewshow = 0;
                    showtagpreview(0);
                }
            } else {
                do
                    x += tagw[i];
                while (ev->x >= x && ++i < LENGTH(tags));

                if (ev->x < LENGTH(m->ltsymbol) + blw) {
                } else if (i < LENGTH(tags)) {
                    if ((i + 1) != selmon->previewshow && !(selmon->tagset[selmon->seltags] & 1 << i)) {
                        selmon->previewshow = i + 1;
                        showtagpreview(i);
                    } else if (selmon->tagset[selmon->seltags] & 1 << i) {
                        selmon->previewshow = 0;
                        showtagpreview(0);
                    }
                } else if (selmon->previewshow != 0) {
                    selmon->previewshow = 0;
                    showtagpreview(0);
                }
            }
        } else if (selmon->previewshow != 0) {
            selmon->previewshow = 0;
            showtagpreview(0);
        }
    }

    if (ev->window != root)
        return;
    if ((m = recttomon(ev->x_root, ev->y_root, 1, 1)) != mon && mon) {
        unfocus(selmon->sel, 1);
        selmon = m;
        focus(NULL);
    }
    mon = m;
}

void
movemouse(const Arg *arg)
{
    int x, y, ocx, ocy, nx, ny;
    Client *c;
    Monitor *m;
    XEvent ev;
    Time lasttime = 0;

    if (!(c = selmon->sel))
        return;
    if (c->isfullscreen) /* no support moving fullscreen windows by mouse */
        return;
    restack(selmon);
    ocx = c->x;
    ocy = c->y;
    if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
        None, cursor[CurMove]->cursor, CurrentTime) != GrabSuccess)
        return;
    if (!getrootptr(&x, &y))
        return;
    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type) {
        case ConfigureRequest:
        case Expose:
        case MapRequest:
            handler[ev.type](&ev);
            break;
        case MotionNotify:
            if ((ev.xmotion.time - lasttime) <= (1000 / 60))
                continue;
            lasttime = ev.xmotion.time;

            nx = ocx + (ev.xmotion.x - x);
            ny = ocy + (ev.xmotion.y - y);
            if (abs(selmon->wx - nx) < snap)
                nx = selmon->wx;
            else if (abs((selmon->wx + selmon->ww) - (nx + WIDTH(c))) < snap)
                nx = selmon->wx + selmon->ww - WIDTH(c);
            if (abs(selmon->wy - ny) < snap)
                ny = selmon->wy;
            else if (abs((selmon->wy + selmon->wh) - (ny + HEIGHT(c))) < snap)
                ny = selmon->wy + selmon->wh - HEIGHT(c);
            if (!c->isfloating && selmon->lt[selmon->sellt]->arrange
            && (abs(nx - c->x) > snap || abs(ny - c->y) > snap))
                togglefloating(NULL);
            if (!selmon->lt[selmon->sellt]->arrange || c->isfloating)
                resize(c, nx, ny, c->w, c->h, 1);
            break;
        }
    } while (ev.type != ButtonRelease);
    XUngrabPointer(dpy, CurrentTime);
    if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
        sendmon(c, m);
        prevmon = selmon;
        selmon = m;
        focus(NULL);
    }
}

static void
movecenter(const Arg *arg)
{
    if (!selmon->sel->isfloating)
        togglefloating(NULL);

    selmon->sel->x = selmon->sel->mon->mx + (selmon->sel->mon->mw - WIDTH(selmon->sel)) / 2;
    selmon->sel->y = selmon->sel->mon->my + (selmon->sel->mon->mh - HEIGHT(selmon->sel)) / 2;
    arrange(selmon);
}

unsigned int
nexttag(int prev, int empty)
{
    unsigned int seltag = selmon->tagset[selmon->seltags];
    unsigned int usedtags = 0;
    Client *c = selmon->clients;

    if (!c)
        return seltag;

    /* skip vacant tags */
    do {
        usedtags |= c->tags;
        c = c->next;
    } while (c);

    do {
        if (prev)
            seltag = seltag == 1 ? (1 << (LENGTH(tags) - 1)) : seltag >> 1;
        else
            seltag = seltag == (1 << (LENGTH(tags) - 1)) ? 1 : seltag << 1;
    } while ((!empty && !(seltag & usedtags)) || (empty && (seltag & usedtags)));

    return seltag;
}

Client *
nexttiled(Client *c)
{
    for (; c && (c->isfloating || !ISVISIBLE(c)); c = c->next);
    return c;
}

void
pop(Client *c)
{
    detach(c);
    attach(c);
    focus(c);
    arrange(c->mon);
}

Client *
prevtiled(Client *c) {
    Client *p, *r;

    for(p = selmon->clients, r = NULL; p && p != c; p = p->next)
        if (!p->isfloating && ISVISIBLE(p))
            r = p;
    return r;
}

void
propertynotify(XEvent *e)
{
    Client *c;
    Window trans;
    XPropertyEvent *ev = &e->xproperty;

    if ((c = wintosystrayicon(ev->window))) {
        if (ev->atom == XA_WM_NORMAL_HINTS) {
            updatesizehints(c);
            updatesystrayicongeom(c, c->w, c->h);
        }
        else
            updatesystrayiconstate(c, ev);
        resizebarwin(selmon);
        updatesystray();
    }
    if ((ev->window == root) && (ev->atom == XA_WM_NAME)) {
        if (!fake_signal())
            updatestatus();
    }
    else if (ev->state == PropertyDelete)
        return; /* ignore */
    else if ((c = wintoclient(ev->window))) {
        switch(ev->atom) {
        default: break;
        case XA_WM_TRANSIENT_FOR:
            if (!c->isfloating && (XGetTransientForHint(dpy, c->win, &trans)) &&
                (c->isfloating = (wintoclient(trans)) != NULL))
                arrange(c->mon);
            break;
        case XA_WM_NORMAL_HINTS:
            updatesizehints(c);
        case XA_WM_HINTS:
            updatewmhints(c);
            drawbars();
            drawtabs();
            break;
        }
        if (ev->atom == XA_WM_NAME || ev->atom == netatom[NetWMName])
            updatetitle(c);
        drawtab(c->mon);
        if (ev->atom == motifatom)
            updatemotifhints(c);
        if (ev->atom == netatom[NetWMWindowType])
            updatewindowtype(c);
    }
}

void
pushdown(const Arg *arg) {
    Client *sel = selmon->sel, *c;

    if (!sel || sel->isfloating || sel == nexttiled(selmon->clients))
        return;
    if ((c = nexttiled(sel->next))) {
        detach(sel);
        sel->next = c->next;
        c->next = sel;
    }
    focus(sel);
    arrange(selmon);
}

void
pushup(const Arg *arg) {
    Client *sel = selmon->sel, *c;

    if (!sel || sel->isfloating)
        return;
    if ((c = prevtiled(sel)) && c != nexttiled(selmon->clients)) {
        detach(sel);
        sel->next = c;
        for(c = selmon->clients; c->next != sel->next; c = c->next);
        c->next = sel;
    }
    focus(sel);
    arrange(selmon);
}

void
quit(const Arg *arg)
{
    if (arg->i) {
        for (Monitor *m = mons; m; m = m->next) {
            savedata(m);
        }

        restart = 1;
    }

    running = 0;
}

void
savedata(const Monitor *m)
{
    Client *c;
    char string[32];
    unsigned long data[1];

    data[0] = m->tagset[m->seltags];
    sprintf(string, "_DWN_MONITOR_TAGS_%u", m->num);
    savemondata(data, string);

    for (int i = 0; i < LENGTH(tags); i++) {
        data[0] = 
            (m->pertag->nmasters[i]            & 0xF)         |
            ((int)(m->pertag->mfacts[i] * 100) & 0x7F) << 4   |
            (m->pertag->showbars[i]            & 0x1)  << 11  |
            (m->pertag->topbar[i]              & 0x1)  << 12  |
            (m->pertag->enablegaps[i]          & 0x1)  << 13  |
            (m->pertag->smartgaps[i]           & 0x1)  << 14  |
            (m->pertag->tpadding[i]            & 0x1)  << 15  |
            (m->pertag->vactags[i]             & 0x1)  << 16  |
            (m->pertag->borderpx[i]            & 0x3F) << 17  |
            (m->pertag->ltidx[i]               & 0xF)  << 23  |
            (0                                 & 0x1)  << 27;
        if (m->pertag->curtag & 1 << i)
            data[0] = 
                (m->nmaster               & 0xF)         |
                ((int)(m->mfact * 100)    & 0x7F) << 4   |
                (m->showbar               & 0x1)  << 11  |
                (m->topbar                & 0x1)  << 12  |
                (m->pertag->enablegaps[i] & 0x1)  << 13  |
                (m->smartgaps             & 0x1)  << 14  |
                (m->pertag->tpadding[i]   & 0x1)  << 15  |
                (m->vactag                & 0x1)  << 16  |
                (m->borderpx              & 0x3F) << 17  |
                (m->pertag->ltidx[i]      & 0xF)  << 23  |
                (0                        & 0x1)  << 27;
        sprintf(string, "_DWN_MONITOR_PERTAG_%u_%u_1", i, m->num);
        savemondata(data, string);

        data[0] = 
            (m->pertag->vertpd[i] & 0x7F) |
            (m->pertag->sidepd[i] & 0x7F) << 7;
        if (m->pertag->curtag & 1 << i)
            data[0] = 
                (m->vp & 0x7F) |
                (m->sp & 0x7F) << 7;
        sprintf(string, "_DWN_MONITOR_PERTAG_%u_%u_2", i, m->num);
        savemondata(data, string);

        data[0] =
            ((m->pertag->gaps[i] & 0xff)       >> 0  & 0x7F)       |
            ((m->pertag->gaps[i] & 0xff00)     >> 8  & 0x7F) << 7  |
            ((m->pertag->gaps[i] & 0xff0000)   >> 16 & 0x7F) << 14 |
            ((m->pertag->gaps[i] & 0xff000000) >> 24 & 0x7F) << 21;
        if (m->pertag->curtag & 1 << i)
            data[0] = 
                (m->gappoh & 0x7F)       |
                (m->gappov & 0x7F) << 7  |
                (m->gappih & 0x7F) << 14 |
                (m->gappiv & 0x7F) << 21;
        sprintf(string, "_DWN_MONITOR_PERTAG_%u_%u_GAPS", i, m->num);
        savemondata(data, string);
    }

    for (c = m->clients; c; c = c->next) {
        data[0] = c->isfloating;
        XChangeProperty(dpy, c->win, clientatom[IsFloating], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
        data[0] = c->issticky;
        XChangeProperty(dpy, c->win, clientatom[IsSticky], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
        data[0] = c->isfullscreen;
        XChangeProperty(dpy, c->win, clientatom[IsFullscreen], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
        data[0] = c->tags;
        XChangeProperty(dpy, c->win, clientatom[Tag], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
        data[0] = c->cfact * 100;
        XChangeProperty(dpy, c->win, clientatom[Cfact], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
        data[0] = c->scratchkey;
        XChangeProperty(dpy, c->win, clientatom[Scratchkey], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
    }

    XSync(dpy, False);
}

void
savemondata(unsigned long data[1], char *string)
{
    XChangeProperty(dpy, root, XInternAtom(dpy, string, False), XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

unsigned long
gettagdata(const Monitor *m, char *string, int i)
{
    char atom[32];
    int di;
    unsigned long dl;
    unsigned char *p = NULL;
    Atom da;

    if (i == -1)
        sprintf(atom, string, m->num);
    else
        sprintf(atom, string, i, m->num);

    Atom monitortags = XInternAtom(dpy, atom, True);

    if (!monitortags)
        return -1;

    if (XGetWindowProperty(dpy, root, monitortags, 0L, sizeof(tags), False, AnyPropertyType,
        &da, &di, &dl, &dl, &p) == Success && p) {
        return *(Atom *)p;
        XFree(p);
    }

    return -1;
}

Monitor *
recttomon(int x, int y, int w, int h)
{
    Monitor *m, *r = selmon;
    int a, area = 0;

    for (m = mons; m; m = m->next)
        if ((a = INTERSECT(x, y, w, h, m)) > area) {
            area = a;
            r = m;
        }
    return r;
}

void
removesystrayicon(Client *i)
{
    Client **ii;

    if (!showsystray || !i)
        return;
    for (ii = &systray->icons; *ii && *ii != i; ii = &(*ii)->next);
    if (ii)
        *ii = i->next;
    free(i);
}

void
removescratch(const Arg *arg)
{
     Client *c = selmon->sel;
     if (!c)
          return;
     c->scratchkey = 0;
}
void
resetnmaster(const Arg *arg)
{
	selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag] = 1;
	arrange(selmon);
}

void
resize(Client *c, int x, int y, int w, int h, int interact)
{
    if (applysizehints(c, &x, &y, &w, &h, interact))
        resizeclient(c, x, y, w, h);
}

void
resizebarwin(Monitor *m) {
    unsigned int w = m->ww;
    if (showsystray && m == systraytomon(m))
        w -= getsystraywidth();
                XMoveResizeWindow(dpy, m->barwin, m->wx + m->sp, m->by + m->vp, m->ww -  2 * m->sp, bh);
}

void
resizeclient(Client *c, int x, int y, int w, int h)
{
    XWindowChanges wc;
    c->oldx = c->x; c->x = wc.x = x;
    c->oldy = c->y; c->y = wc.y = y;
    c->oldw = c->w; c->w = wc.width = w;
    c->oldh = c->h; c->h = wc.height = h;
    wc.border_width = c->isfloating ? floatborderpx : c->bw;
    if (((nexttiled(c->mon->clients) == c && !nexttiled(c->next))
        || &monocle == c->mon->lt[c->mon->sellt]->arrange)
        && !c->isfullscreen && !c->isfloating
        && NULL != c->mon->lt[c->mon->sellt]->arrange) {
            c->w = wc.width += c->bw * 2;
            c->h = wc.height += c->bw * 2;
            wc.border_width = 0;
    }

    XConfigureWindow(dpy, c->win, CWX|CWY|CWWidth|CWHeight|CWBorderWidth, &wc);
    configure(c);
    XSync(dpy, False);
}

void
resizemouse(const Arg *arg)
{
    int ocx, ocy, nw, nh;
    int ocx2, ocy2, nx, ny;
    Client *c;
    Monitor *m;
    XEvent ev;
    int horizcorner, vertcorner;
    int di;
    unsigned int dui;
    Window dummy;
    Time lasttime = 0;

    if (!(c = selmon->sel))
        return;
    if (c->isfullscreen) /* no support resizing fullscreen windows by mouse */
        return;
    restack(selmon);
    ocx = c->x;
    ocy = c->y;
    ocx2 = c->x + c->w;
    ocy2 = c->y + c->h;
    if (XGrabPointer(dpy, root, False, MOUSEMASK, GrabModeAsync, GrabModeAsync,
        None, cursor[CurResize]->cursor, CurrentTime) != GrabSuccess)
        return;
    if (!XQueryPointer (dpy, c->win, &dummy, &dummy, &di, &di, &nx, &ny, &dui))
        return;
    horizcorner = nx < c->w / 2;
    vertcorner  = ny < c->h / 2;
    XWarpPointer (dpy, None, c->win, 0, 0, 0, 0,
            horizcorner ? (-c->bw) : (c->w + c->bw -1),
            vertcorner  ? (-c->bw) : (c->h + c->bw -1));
    do {
        XMaskEvent(dpy, MOUSEMASK|ExposureMask|SubstructureRedirectMask, &ev);
        switch(ev.type) {
        case ConfigureRequest:
        case Expose:
        case MapRequest:
            handler[ev.type](&ev);
            break;
        case MotionNotify:
            if ((ev.xmotion.time - lasttime) <= (1000 / 60))
                continue;
            lasttime = ev.xmotion.time;

            nx = horizcorner ? ev.xmotion.x : c->x;
            ny = vertcorner ? ev.xmotion.y : c->y;
            nw = MAX(horizcorner ? (ocx2 - nx) : (ev.xmotion.x - ocx - 2 * c->bw + 1), 1);
            nh = MAX(vertcorner ? (ocy2 - ny) : (ev.xmotion.y - ocy - 2 * c->bw + 1), 1);

            if (c->mon->wx + nw >= selmon->wx && c->mon->wx + nw <= selmon->wx + selmon->ww
            && c->mon->wy + nh >= selmon->wy && c->mon->wy + nh <= selmon->wy + selmon->wh)
            {
                if (!c->isfloating && selmon->lt[selmon->sellt]->arrange
                && (abs(nw - c->w) > snap || abs(nh - c->h) > snap))
                    togglefloating(NULL);
            }
            if (!selmon->lt[selmon->sellt]->arrange || c->isfloating)
                resize(c, nx, ny, nw, nh, 1);
            break;
        }
    } while (ev.type != ButtonRelease);
    XWarpPointer(dpy, None, c->win, 0, 0, 0, 0,
              horizcorner ? (-c->bw) : (c->w + c->bw - 1),
              vertcorner ? (-c->bw) : (c->h + c->bw - 1));
    XUngrabPointer(dpy, CurrentTime);
    while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
    if ((m = recttomon(c->x, c->y, c->w, c->h)) != selmon) {
        sendmon(c, m);
        prevmon = selmon;
        selmon = m;
        focus(NULL);
    }
}

void
resizerequest(XEvent *e)
{
    XResizeRequestEvent *ev = &e->xresizerequest;
    Client *i;

    if ((i = wintosystrayicon(ev->window))) {
        updatesystrayicongeom(i, ev->width, ev->height);
        resizebarwin(selmon);
        updatesystray();
    }
}

void
restack(Monitor *m)
{
    Client *c;
    XEvent ev;
    XWindowChanges wc;

    drawbar(m);
    drawtab(m);
    if (!m->sel)
        return;
    if (m->sel->isfloating || !m->lt[m->sellt]->arrange)
        XRaiseWindow(dpy, m->sel->win);
    if (m->lt[m->sellt]->arrange) {
        wc.stack_mode = Below;
        wc.sibling = m->barwin;
        for (c = m->stack; c; c = c->snext)
            if (!c->isfloating && ISVISIBLE(c)) {
                XConfigureWindow(dpy, c->win, CWSibling|CWStackMode, &wc);
                wc.sibling = c->win;
            }
    }
    XSync(dpy, False);
    while (XCheckMaskEvent(dpy, EnterWindowMask, &ev));
    if (m == selmon && (m->tagset[m->seltags] & m->sel->tags) && selmon->lt[selmon->sellt] != &layouts[2])
        warp(m->sel);
}

void
run(void)
{
    XEvent ev;
    /* main event loop */
    XSync(dpy, False);
    while (running && !XNextEvent(dpy, &ev))
    {
        if (handler[ev.type])
            handler[ev.type](&ev); /* call handler */
    }
}

void
scan(void)
{
    unsigned int i, num;
    Window d1, d2, *wins = NULL;
    XWindowAttributes wa;

    if (XQueryTree(dpy, root, &d1, &d2, &wins, &num)) {
        for (i = 0; i < num; i++) {
            if (!XGetWindowAttributes(dpy, wins[i], &wa)
            || wa.override_redirect || XGetTransientForHint(dpy, wins[i], &d1))
                continue;
            if (wa.map_state == IsViewable || getstate(wins[i]) == IconicState)
                manage(wins[i], &wa);
        }
        for (i = 0; i < num; i++) { /* now the transients */
            if (!XGetWindowAttributes(dpy, wins[i], &wa))
                continue;
            if (XGetTransientForHint(dpy, wins[i], &d1)
            && (wa.map_state == IsViewable || getstate(wins[i]) == IconicState))
                manage(wins[i], &wa);
        }
        if (wins)
            XFree(wins);
    }
}

void
killontag(const Arg *arg)
{
    Client *c = NULL;

    for (c = selmon->clients; c; c = c->next) {
        if (c->tags & arg->ui && !sendevent(c->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
            XGrabServer(dpy);
            XSetErrorHandler(xerrordummy);
            XSetCloseDownMode(dpy, DestroyAll);
            XKillClient(dpy, c->win);
            XSync(dpy, False);
            XSetErrorHandler(xerror);
            XUngrabServer(dpy);
        }
    }
}

void
killontagmonn(const Arg *arg)
{
    if (!selmon->sel || !mons->next)
        return;

    Monitor *m = dirtomon(1);

    for (Client *c = m->clients; c; c = c->next) {
        if (c->tags & arg->ui && !sendevent(c->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
            XGrabServer(dpy);
            XSetErrorHandler(xerrordummy);
            XSetCloseDownMode(dpy, DestroyAll);
            XKillClient(dpy, c->win);
            XSync(dpy, False);
            XSetErrorHandler(xerror);
            XUngrabServer(dpy);
        }
    }
}

void
killontagmonp(const Arg *arg)
{
    if (!selmon->sel || !mons->next)
        return;

    Monitor *m = dirtomon(-1);

    for (Client *c = m->clients; c; c = c->next) {
        if (c->tags & arg->ui && !sendevent(c->win, wmatom[WMDelete], NoEventMask, wmatom[WMDelete], CurrentTime, 0 , 0, 0)) {
            XGrabServer(dpy);
            XSetErrorHandler(xerrordummy);
            XSetCloseDownMode(dpy, DestroyAll);
            XKillClient(dpy, c->win);
            XSync(dpy, False);
            XSetErrorHandler(xerror);
            XUngrabServer(dpy);
        }
    }
}

void
focusnextmon(const Arg *arg)
{
    focusothermon(arg, 1, 0);
}

void
focusprevmon(const Arg *arg)
{
    focusothermon(arg, -1, 0);
}

void
shownextmon(const Arg *arg)
{
    focusothermon(arg, 1, 1);
}

void
showprevmon(const Arg *arg)
{
    focusothermon(arg, -1, 1);
}

void
focusothermon(const Arg *arg, int dir, int show)
{
    if (!mons->next)
        return;

    focusothermonview(arg, dirtomon(dir));
    if (!show) {
        Arg tmp;

        tmp.i = dir;
        focusmon(&tmp);
    }
}

void
focusothermonview(const Arg *arg, Monitor *m)
{
    int i;
    unsigned int tmptag;

    prevmon = NULL;
    switchtag();
    m->seltags ^= 1; /* toggle sel tagset */
    if (arg->ui & TAGMASK) {
        m->pertag->prevtag = m->pertag->curtag;
         m->tagset[m->seltags] = arg->ui & TAGMASK;
        if (arg->ui == ~0)
            m->pertag->curtag = 0;
        else {
            for (i=0; !(arg->ui & 1 << i); i++) ;
            m->pertag->curtag = i;
        }
    } else {
        tmptag = m->pertag->prevtag;
        m->pertag->prevtag = m->pertag->curtag;
        m->pertag->curtag = tmptag;
    }

    m->nmaster = m->pertag->nmasters[m->pertag->curtag];
    m->mfact = m->pertag->mfacts[m->pertag->curtag];
    m->sellt = m->pertag->sellts[m->pertag->curtag];
    m->lt[m->sellt] = m->pertag->ltidxs[m->pertag->curtag][m->sellt];
    m->lt[m->sellt^1] = m->pertag->ltidxs[m->pertag->curtag][m->sellt^1];

    m->gappoh = (m->pertag->gaps[m->pertag->curtag] & 0xff) >> 0;
    m->gappov = (m->pertag->gaps[m->pertag->curtag] & 0xff00) >> 8;
    m->gappih = (m->pertag->gaps[m->pertag->curtag] & 0xff0000) >> 16;
    m->gappiv = (m->pertag->gaps[m->pertag->curtag] & 0xff000000) >> 24;

    m->vp = m->pertag->vertpd[m->pertag->curtag];
    m->sp = m->pertag->sidepd[m->pertag->curtag];
    m->smartgaps = m->pertag->smartgaps[m->pertag->curtag];
    m->vactag = m->pertag->vactags[m->pertag->curtag];
    m->borderpx = m->pertag->borderpx[m->pertag->curtag];
    m->topbar = m->pertag->topbar[m->pertag->curtag];
    m->showbar = m->pertag->showbars[m->pertag->curtag];

    focus(NULL);
    arrange(m);
    updatecurrentdesktop();
}

void
tagmovnextmon(const Arg *arg)
{
    tagothermon(arg, 1, 0, 0);
}

void
tagmovprevmon(const Arg *arg)
{
    tagothermon(arg, -1, 0, 0);
}

void
tagfolnextmon(const Arg *arg)
{
    tagothermon(arg, 1, 0, 1);
}

void
tagfolprevmon(const Arg *arg)
{
    tagothermon(arg, -1, 0, 1);
}

void
tagshownextmon(const Arg *arg)
{
    tagothermon(arg, 1, 1, 0);
}

void
tagshowprevmon(const Arg *arg)
{
    tagothermon(arg, -1, 1, 0);
}

void
tagothermon(const Arg *arg, int dir, int show, int follow)
{
    Client *sel;
    Monitor *newmon;

    if (!selmon->sel || !mons->next)
        return;
    sel = selmon->sel;
    newmon = dirtomon(dir);
    sendmon(sel, newmon);
    if (arg->ui & TAGMASK) {
        sel->tags = arg->ui & TAGMASK;
        focus(NULL);
        arrange(newmon);
        if (follow)
            focusothermon(arg, dir, 0);
        else if (show)
            focusothermon(arg, dir, 1);
    }
}

void
sendmon(Client *c, Monitor *m)
{
    if (c->mon == m)
        return;
    unfocus(c, 1);
    detach(c);
    detachstack(c);
    c->mon = m;
    c->tags = (m->tagset[m->seltags] ? m->tagset[m->seltags] : 1);
    if (attachbelow)
        attachBelow(c);
    else
        attach(c);
    attachstack(c);
    focus(NULL);
    arrange(NULL);
    if (c->switchtag)
        c->switchtag = 0;
}

void
setborderpx(const Arg *arg)
{
    Client *c;
    int prev_borderpx = selmon->borderpx;

    if (arg->i == 0)
        selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag] = borderpx;
    else if (selmon->borderpx + arg->i < 0)
        selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag] = 0;
    else
        selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag] += arg->i;

    for (c = selmon->clients; c; c = c->next)
    {
        if (c->bw + arg->i < 0)
            c->bw = selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag] = 0;
        else
            c->bw = selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag];
        if (c->isfloating || !selmon->lt[selmon->sellt]->arrange)
        {
            if (arg->i != 0 && prev_borderpx + arg->i >= 0)
                resize(c, c->x, c->y, c->w-(arg->i*2), c->h-(arg->i*2), 0);
            else if (arg->i != 0)
                resizeclient(c, c->x, c->y, c->w, c->h);
            else if (prev_borderpx > borderpx)
                resize(c, c->x, c->y, c->w + 2*(prev_borderpx - borderpx), c->h + 2*(prev_borderpx - borderpx), 0);
            else if (prev_borderpx < borderpx)
                resize(c, c->x, c->y, c->w-2*(borderpx - prev_borderpx), c->h-2*(borderpx - prev_borderpx), 0);
        }
    }
    arrange(selmon);
}

void
setclientstate(Client *c, long state)
{
    long data[] = { state, None };

    XChangeProperty(dpy, c->win, wmatom[WMState], wmatom[WMState], 32,
        PropModeReplace, (unsigned char *)data, 2);
}
void
setcurrentdesktop(void){
    long data[] = { 0 };
    XChangeProperty(dpy, root, netatom[NetCurrentDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}
void setdesktopnames(void){
    XTextProperty text;
    Xutf8TextListToTextProperty(dpy, tags, LENGTH(tags), XUTF8StringStyle, &text);
    XSetTextProperty(dpy, root, &text, netatom[NetDesktopNames]);
}

int
sendevent(Window w, Atom proto, int mask, long d0, long d1, long d2, long d3, long d4)
{
    int n;
    Atom *protocols, mt;
    int exists = 0;
    XEvent ev;

    if (proto == wmatom[WMTakeFocus] || proto == wmatom[WMDelete]) {
        mt = wmatom[WMProtocols];
        if (XGetWMProtocols(dpy, w, &protocols, &n)) {
            while (!exists && n--)
                exists = protocols[n] == proto;
            XFree(protocols);
        }
    }
    else {
        exists = True;
        mt = proto;
    }
    if (exists) {
        ev.type = ClientMessage;
        ev.xclient.window = w;
        ev.xclient.message_type = mt;
        ev.xclient.format = 32;
        ev.xclient.data.l[0] = d0;
        ev.xclient.data.l[1] = d1;
        ev.xclient.data.l[2] = d2;
        ev.xclient.data.l[3] = d3;
        ev.xclient.data.l[4] = d4;
        XSendEvent(dpy, w, False, mask, &ev);
    }
    return exists;
}

void
setnumdesktops(void){
    long data[] = { LENGTH(tags) };
    XChangeProperty(dpy, root, netatom[NetNumberOfDesktops], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

void
setfocus(Client *c)
{
    if (!c->neverfocus) {
        XSetInputFocus(dpy, c->win, RevertToPointerRoot, CurrentTime);
        XChangeProperty(dpy, root, netatom[NetActiveWindow],
            XA_WINDOW, 32, PropModeReplace,
            (unsigned char *) &(c->win), 1);
    }
    sendevent(c->win, wmatom[WMTakeFocus], NoEventMask, wmatom[WMTakeFocus], CurrentTime, 0, 0, 0);
}

void
setfullscreen(Client *c, int fullscreen)
{
    if (fullscreen && !c->isfullscreen) {
        XChangeProperty(dpy, c->win, netatom[NetWMState], XA_ATOM, 32,
            PropModeReplace, (unsigned char*)&netatom[NetWMFullscreen], 1);
        c->isfullscreen = 1;
        c->oldbw = c->bw;
        c->oldstate = c->isfloating;
        c->bw = 0;
        c->isfloating = 0;
        resizeclient(c, c->mon->mx, c->mon->my, c->mon->mw, c->mon->mh);
        XRaiseWindow(dpy, c->win);
    } else if (!fullscreen && c->isfullscreen){
        XChangeProperty(dpy, c->win, netatom[NetWMState], XA_ATOM, 32,
            PropModeReplace, (unsigned char*)0, 0);
        c->isfullscreen = 0;
        c->bw = c->oldbw;
        c->isfloating = c->oldstate;
        c->x = c->oldx;
        c->y = c->oldy;
        c->w = c->oldw;
        c->h = c->oldh;
        resizeclient(c, c->x, c->y, c->w, c->h);
        arrange(c->mon);
    }
}

void
setlayout(const Arg *arg)
{
    if (!arg || !&layouts[arg->i] || &layouts[arg->i] != selmon->lt[selmon->sellt]) {
        selmon->pertag->sellts[selmon->pertag->curtag] ^= 1;
        selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
    }
    if (arg && &layouts[arg->i]) {
        selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt] = (Layout *)&layouts[arg->i];
    }
    selmon->pertag->ltidx[selmon->pertag->curtag] = arg->i;
    selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
    strncpy(selmon->ltsymbol, selmon->lt[selmon->sellt]->symbol, sizeof selmon->ltsymbol);
    if (selmon->sel)
        arrange(selmon);
    else
        drawbar(selmon);
    if (selmon->sel) {
        for (unsigned int i = 0; i < 9; i++) {
            if (&layouts[i] == selmon->lt[selmon->sellt]) {
                XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSelLayout][i].pixel);
                break;
            }
        }
    }
}

void setcfact(const Arg *arg) {
    float f;
    Client *c;

    c = selmon->sel;

    if (!arg || !c || !selmon->lt[selmon->sellt]->arrange)
        return;
    f = arg->f + c->cfact;
    if (arg->f == 0.0)
        f = 1.0;
    else if (f < 0.25 || f > 4.0)
        return;
    c->cfact = f;
    arrange(selmon);
}

void
setmfact(const Arg *arg)
{
    float f;

    if (!arg || !selmon->lt[selmon->sellt]->arrange)
        return;
    f = arg->f < 1.0 ? arg->f + selmon->mfact : arg->f - 1.0;
    if (arg->f == 0.0)
        f = tagrules[0][0].mfact;
    else if (f < 0.05 || f > 0.95)
        return;
    selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag] = f;
    arrange(selmon);
}

void
setscratch(const Arg *arg)
{
     Client *c = selmon->sel;
     if (!c)
          return;

     c->scratchkey = ((char**)arg->v)[0][0];
}

void
setup(void)
{
    int i;
    XSetWindowAttributes wa;
    Atom utf8string;

    /* clean up any zombies immediately */
    sigchld(0);

    signal(SIGHUP, sighup);
    signal(SIGTERM, sigterm);

 	putenv("_JAVA_AWT_WM_NONREPARENTING=1");

    /* init screen */
    screen = DefaultScreen(dpy);
    sw = DisplayWidth(dpy, screen);
    sh = DisplayHeight(dpy, screen);
    root = RootWindow(dpy, screen);
    drw = drw_create(dpy, screen, root, sw, sh);
    if (!drw_fontset_create(drw, fonts, LENGTH(fonts)))
        die("no fonts could be loaded.");
    lrpad = drw->fonts->h + horizpadbar;
    bh = user_bh ? user_bh + vertpadbar: drw->fonts->h + 2 + vertpadbar;
    th = bh;
    updategeom();

    /* init atoms */
    utf8string = XInternAtom(dpy, "UTF8_STRING", False);
    wmatom[WMProtocols] = XInternAtom(dpy, "WM_PROTOCOLS", False);
    wmatom[WMDelete] = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
    wmatom[WMState] = XInternAtom(dpy, "WM_STATE", False);
    wmatom[WMTakeFocus] = XInternAtom(dpy, "WM_TAKE_FOCUS", False);
    clientatom[IsFloating] = XInternAtom(dpy, "_IS_FLOATING", False);
    clientatom[IsSticky] = XInternAtom(dpy, "_IS_STICKY", False);
    clientatom[IsFullscreen] = XInternAtom(dpy, "_IS_FULLSCREEN", False);
    clientatom[Tag] = XInternAtom(dpy, "_TAG", False);
    clientatom[Cfact] = XInternAtom(dpy, "_CFACTS", False);
    clientatom[Scratchkey] = XInternAtom(dpy, "_SCRATCHKEY", False);
    netatom[NetActiveWindow] = XInternAtom(dpy, "_NET_ACTIVE_WINDOW", False);
    netatom[NetSupported] = XInternAtom(dpy, "_NET_SUPPORTED", False);
    netatom[NetSystemTray] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_S0", False);
    netatom[NetSystemTrayOP] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_OPCODE", False);
    netatom[NetSystemTrayOrientation] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_ORIENTATION", False);
    netatom[NetSystemTrayOrientationHorz] = XInternAtom(dpy, "_NET_SYSTEM_TRAY_ORIENTATION_HORZ", False);
    netatom[NetWMName] = XInternAtom(dpy, "_NET_WM_NAME", False);
    netatom[NetWMState] = XInternAtom(dpy, "_NET_WM_STATE", False);
    netatom[NetWMCheck] = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", False);
    netatom[NetWMFullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", False);
    netatom[NetWMWindowType] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", False);
    netatom[NetWMWindowTypeDock] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DOCK", False);
    netatom[NetWMWindowTypeDialog] = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE_DIALOG", False);
    netatom[NetClientList] = XInternAtom(dpy, "_NET_CLIENT_LIST", False);
    netatom[NetDesktopViewport] = XInternAtom(dpy, "_NET_DESKTOP_VIEWPORT", False);
    netatom[NetNumberOfDesktops] = XInternAtom(dpy, "_NET_NUMBER_OF_DESKTOPS", False);
    netatom[NetCurrentDesktop] = XInternAtom(dpy, "_NET_CURRENT_DESKTOP", False);
    netatom[NetDesktopNames] = XInternAtom(dpy, "_NET_DESKTOP_NAMES", False);
    motifatom = XInternAtom(dpy, "_MOTIF_WM_HINTS", False);
    xatom[Manager] = XInternAtom(dpy, "MANAGER", False);
    xatom[Xembed] = XInternAtom(dpy, "_XEMBED", False);
    xatom[XembedInfo] = XInternAtom(dpy, "_XEMBED_INFO", False);
    /* init cursors */
    cursor[CurNormal] = drw_cur_create(drw, XC_left_ptr);
    cursor[CurResize] = drw_cur_create(drw, XC_sizing);
    cursor[CurMove] = drw_cur_create(drw, XC_fleur);
    /* init appearance */
    scheme = ecalloc(LENGTH(colors) + 1, sizeof(Clr *));
    scheme[LENGTH(colors)] = drw_scm_create(drw, colors[0], 9);
    for (i = 0; i < LENGTH(colors); i++)
        scheme[i] = drw_scm_create(drw, colors[i], 9);
    /* init system tray */
    updatesystray();
    /* init bars */
    updatebars();
    updatestatus();
    updatebarpos(selmon);
    updatepreview();
    /* supporting window for NetWMCheck */
    wmcheckwin = XCreateSimpleWindow(dpy, root, 0, 0, 1, 1, 0, 0, 0);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMCheck], XA_WINDOW, 32,
        PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    XChangeProperty(dpy, wmcheckwin, netatom[NetWMName], utf8string, 8,
        PropModeReplace, (unsigned char *) "LG3D", 4);
    XChangeProperty(dpy, root, netatom[NetWMCheck], XA_WINDOW, 32,
        PropModeReplace, (unsigned char *) &wmcheckwin, 1);
    /* EWMH support per view */
    XChangeProperty(dpy, root, netatom[NetSupported], XA_ATOM, 32,
        PropModeReplace, (unsigned char *) netatom, NetLast);
    setnumdesktops();
    setcurrentdesktop();
    setdesktopnames();
    setviewport();
    XDeleteProperty(dpy, root, netatom[NetClientList]);
    /* select events */
    wa.cursor = cursor[CurNormal]->cursor;
    wa.event_mask = SubstructureRedirectMask|SubstructureNotifyMask
        |ButtonPressMask|PointerMotionMask|EnterWindowMask
        |LeaveWindowMask|StructureNotifyMask|PropertyChangeMask;
    XChangeWindowAttributes(dpy, root, CWEventMask|CWCursor, &wa);
    XSelectInput(dpy, root, wa.event_mask);

    grabkeys();
    focus(NULL);
}
void
setviewport(void){
    long data[] = { 0, 0 };
    XChangeProperty(dpy, root, netatom[NetDesktopViewport], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 2);
}


void
seturgent(Client *c, int urg)
{
    XWMHints *wmh;

    c->isurgent = urg;
    if (!(wmh = XGetWMHints(dpy, c->win)))
        return;
    wmh->flags = urg ? (wmh->flags | XUrgencyHint) : (wmh->flags & ~XUrgencyHint);
    XSetWMHints(dpy, c->win, wmh);
    XFree(wmh);
}

void
showhide(Client *c)
{
    if (!c)
        return;
    if (ISVISIBLE(c)) {
        /* show clients top down */
        XMoveWindow(dpy, c->win, c->x, c->y);
        if (c->needresize) {
            c->needresize = 0;
            XMoveResizeWindow(dpy, c->win, c->x, c->y, c->w, c->h);
        } else {
            XMoveWindow(dpy, c->win, c->x, c->y);
        }
        if ((!c->mon->lt[c->mon->sellt]->arrange || c->isfloating) && !c->isfullscreen)
            resize(c, c->x, c->y, c->w, c->h, 0);
        showhide(c->snext);
    } else {
          /* optional: auto-hide scratchpads when moving to other tags */
          if (c->scratchkey != 0 && !(c->tags & c->mon->tagset[c->mon->seltags]))
               c->tags = 0;
        /* hide clients bottom up */
        showhide(c->snext);
        XMoveWindow(dpy, c->win, WIDTH(c) * -2, c->y);
    }
}

void
showtagpreview(int tag)
{
    if (!selmon->previewshow) {
        XUnmapWindow(dpy, selmon->tagwin);
        return;
    }

    if (selmon->tagmap[tag]) {
        XSetWindowBackgroundPixmap(dpy, selmon->tagwin, selmon->tagmap[tag]);
        XCopyArea(dpy, selmon->tagmap[tag], selmon->tagwin, drw->gc, 0, 0, selmon->mw / scalepreview, selmon->mh / scalepreview, 0, 0);
        XSync(dpy, False);
        XMapWindow(dpy, selmon->tagwin);
    } else
        XUnmapWindow(dpy, selmon->tagwin);
}

void
sigchld(int unused)
{
    if (signal(SIGCHLD, sigchld) == SIG_ERR)
        die("can't install SIGCHLD handler:");
    while (0 < waitpid(-1, NULL, WNOHANG));
}

void
sighup(int unused)
{
    Arg a = {.i = 1};
    quit(&a);
}

void
sigterm(int unused)
{
    Arg a = {.i = 0};
    quit(&a);
}

void
spawn(const Arg *arg)
{
    if (fork() == 0) {
        if (dpy)
            close(ConnectionNumber(dpy));
        setsid();
        execvp(((char **)arg->v)[0], (char **)arg->v);
        fprintf(stderr, "dwm: execvp %s", ((char **)arg->v)[0]);
        perror(" failed");
        exit(EXIT_SUCCESS);
    }
}

void
switchcol(const Arg *arg)
{
    Client *c, *t;
    int col = 0;
    int i;

    if (!selmon->sel)
        return;
    for (i = 0, c = nexttiled(selmon->clients); c ;
         c = nexttiled(c->next), i++) {
        if (c == selmon->sel)
            col = (i + 1) > selmon->nmaster;
    }
    if (i <= selmon->nmaster)
        return;
    for (c = selmon->stack; c; c = c->snext) {
        if (!ISVISIBLE(c))
            continue;
        for (i = 0, t = nexttiled(selmon->clients); t && t != c;
             t = nexttiled(t->next), i++);
        if (t && (i + 1 > selmon->nmaster) != col) {
            focus(c);
            restack(selmon);
            break;
        }
    }
}

void
swaptags(const Arg *arg)
{
    unsigned int newtag = arg->ui & TAGMASK;
    unsigned int curtag = selmon->tagset[selmon->seltags];

    if (newtag == curtag || !curtag || (curtag & (curtag-1)))
        return;

    for (Client *c = selmon->clients; c != NULL; c = c->next) {
        if ((c->tags & newtag) || (c->tags & curtag))
            c->tags ^= curtag ^ newtag;

        if (!c->tags)
            c->tags = newtag;
    }

    selmon->tagset[selmon->seltags] = newtag;

    focus(NULL);
    arrange(selmon);
}

void
spawnscratch(const Arg *arg)
{
     if (fork() == 0) {
          if (dpy)
               close(ConnectionNumber(dpy));
          setsid();
          execvp(((char **)arg->v)[1], ((char **)arg->v)+1);
          fprintf(stderr, "dwm: execvp %s", ((char **)arg->v)[1]);
          perror(" failed");
          exit(EXIT_SUCCESS);
     }
}

void
switchtag(void)
{
    int i;
    unsigned int occ = 0;
    Client *c;
    Imlib_Image image;

    for (c = selmon->clients; c; c = c->next)
        occ |= c->tags;
    for (i = 0; i < LENGTH(tags); i++) {
        if (selmon->tagset[selmon->seltags] & 1 << i) {
            if (selmon->tagmap[i] != 0) {
                XFreePixmap(dpy, selmon->tagmap[i]);
                selmon->tagmap[i] = 0;
            }
            if (occ & 1 << i) {
                image = imlib_create_image(sw, sh);
                imlib_context_set_image(image);
                imlib_context_set_display(dpy);
                imlib_context_set_visual(DefaultVisual(dpy, screen));
                imlib_context_set_drawable(RootWindow(dpy, screen));
                //uncomment the following line and comment the other imlin_copy.. line if you don't want the bar showing on the preview
                imlib_copy_drawable_to_image(0, selmon->wx, selmon->wy, selmon->ww ,selmon->wh, 0, 0, 1);
                /* imlib_copy_drawable_to_image(0, selmon->mx, selmon->my, selmon->mw ,selmon->mh, 0, 0, 1); */
                selmon->tagmap[i] = XCreatePixmap(dpy, selmon->tagwin, selmon->mw / scalepreview, selmon->mh / scalepreview, DefaultDepth(dpy, screen));
                imlib_context_set_drawable(selmon->tagmap[i]);
                imlib_render_image_part_on_drawable_at_size(0, 0, selmon->mw, selmon->mh, 0, 0, selmon->mw / scalepreview, selmon->mh / scalepreview);
                imlib_free_image();
            }
        }
    }
}

void
tag(const Arg *arg)
{
    if (selmon->sel && arg->ui & TAGMASK) {
        selmon->sel->tags = arg->ui & TAGMASK;
        if (selmon->sel->switchtag)
            selmon->sel->switchtag = 0;
        focus(NULL);
        arrange(selmon);
    }
}

void
tagall(const Arg *arg) {
    if (!selmon->clients)
        return;

    for (Client *c = selmon->clients; c; c = c->next) {
        for (int j = 0; j < LENGTH(tags); j++) {
            if (c->tags & 1 << j && selmon->tagset[selmon->seltags] & 1 << j) {
                c->tags = c->tags ^ (1 << j & TAGMASK);
                c->tags = c->tags | 1 << arg->ui;
            }
        }
    }

    arrange(selmon);
}

void
tagallfloat(const Arg *arg) {
    if (!selmon->clients)
        return;

    for(Client *c = selmon->clients; c; c = c->next) {
        if (c->isfloating) {
            for(int j = 0; j < LENGTH(tags); j++) {
                if (c->tags & 1 << j && selmon->tagset[selmon->seltags] & 1 << j) {
                    c->tags = c->tags ^ (1 << j & TAGMASK);
                    c->tags = c->tags | 1 << arg->ui;
                }
            }
        }
    }

    arrange(selmon);
}

void
tagwith(const Arg *arg)
{
    if (selmon->sel && arg->ui & TAGMASK) {
        selmon->sel->tags = arg->ui & TAGMASK;
        if (selmon->sel->switchtag)
            selmon->sel->switchtag = 0;
        focus(NULL);
        arrange(selmon);
        view(arg);
    }
}

void
tagmon(const Arg *arg)
{
    if (!selmon->sel || !mons->next)
        return;
    sendmon(selmon->sel, dirtomon(arg->i));
}

void
togglebar(const Arg *arg)
{
    selmon->showbar = selmon->pertag->showbars[selmon->pertag->curtag] = !selmon->showbar;
    updatebarpos(selmon);
    resizebarwin(selmon);
    if (showsystray) {
        XWindowChanges wc;
        if (!selmon->showbar)
            wc.y = -bh;
        else if (selmon->showbar) {
            wc.y = 0;
            if (!selmon->topbar)
                wc.y = selmon->mh - bh;
        }
        XConfigureWindow(dpy, systray->win, CWY, &wc);
    }
    arrange(selmon);
}

void
tabmode(const Arg *arg)
{
    if (arg && arg->i >= 0)
        selmon->showtab = arg->ui % showtab_nmodes;
    else
        selmon->showtab = (selmon->showtab + 1 ) % showtab_nmodes;
    arrange(selmon);
}


void
togglefloating(const Arg *arg)
{
    if (!selmon->sel)
        return;
    if (selmon->sel->isfullscreen) /* no support for fullscreen windows */
        return;
    selmon->sel->isfloating = !selmon->sel->isfloating || selmon->sel->isfixed;
    if(selmon->sel->isfloating) {
        XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColFloat].pixel);
        /* restore last known float dimensions */
        resize(selmon->sel, selmon->sel->sfx, selmon->sel->sfy,
               selmon->sel->sfw, selmon->sel->sfh, False);
    } else {
        if (selmon->sel->issticky)
            XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColSticky].pixel);
        else {
            for (unsigned int i = 0; i < 9; i++) {
                if (&layouts[i] == selmon->lt[selmon->sellt]) {
                    XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSelLayout][i].pixel);
                    break;
                }
            }
        }
        /* save last known float dimensions */
        selmon->sel->sfx = selmon->sel->x;
        selmon->sel->sfy = selmon->sel->y;
        selmon->sel->sfw = selmon->sel->w;
        selmon->sel->sfh = selmon->sel->h;
    }
    arrange(selmon);
}

void
unfloatvisible(const Arg *arg)
{
    Client *c;

    for (c = selmon->clients; c; c = c->next) {
        if (ISVISIBLE(c) && c->isfloating) {
            c->isfloating = c->isfixed;
            if (selmon->sel->issticky)
                XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColSticky].pixel);
            else {
                for (unsigned int i = 0; i < 9; i++) {
                    if (&layouts[i] == selmon->lt[selmon->sellt]) {
                        XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSelLayout][i].pixel);
                        break;
                    }
                }
            }
        }
    }

    if (arg && arg->v)
        setlayout(arg);
    else
        arrange(selmon);
}

void
togglesticky(const Arg *arg)
{
    if (!selmon->sel)
        return;
    XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColSticky].pixel);

    selmon->sel->issticky = !selmon->sel->issticky;
    if (!selmon->sel->issticky) {
        if (selmon->sel->isfloating)
            XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSel][ColFloat].pixel);
        else {
            for (unsigned int i = 0; i < 9; i++) {
                if (&layouts[i] == selmon->lt[selmon->sellt]) {
                    XSetWindowBorder(dpy, selmon->sel->win, scheme[SchemeSelLayout][i].pixel);
                    break;
                }
            }
        }
    }
    arrange(selmon);
}

void
togglefullscr(const Arg *arg)
{
    if (selmon->sel) {
        setfullscreen(selmon->sel, !selmon->sel->isfullscreen);
    }
}

void
togglescratch(const Arg *arg)
{
     Client *c, *next, *last = NULL, *found = NULL, *monclients = NULL;
     Monitor *mon;
     int scratchvisible = 0;
     int multimonscratch = 0;
     int scratchmon = -1;
     int numscratchpads = 0;

     for (mon = mons; mon; mon = mon->next)
          for (c = mon->clients; c; c = c->next) {
               if (c->scratchkey != ((char**)arg->v)[0][0])
                    continue;
               if (scratchmon != -1 && scratchmon != mon->num)
                    multimonscratch = 1;
               if (c->mon->tagset[c->mon->seltags] & c->tags)
                    ++scratchvisible;
               scratchmon = mon->num;
               ++numscratchpads;
          }

     for (mon = mons; mon; mon = mon->next) {
          for (c = mon->stack; c; c = next) {
               next = c->snext;
               if (c->scratchkey != ((char**)arg->v)[0][0])
                    continue;

               if (!found || (mon == selmon && c->mon != selmon))
                    found = c;

               unfocus(c, 0);

               if (!multimonscratch && c->mon != selmon) {
                    detach(c);
                    detachstack(c);
                    c->next = NULL;

                    if (last)
                         last = last->next = c;
                    else
                         last = monclients = c;
               } else if (scratchvisible == numscratchpads) {
                    c->tags = 0;
               } else {
                    c->tags = c->mon->tagset[c->mon->seltags];
                    if (c->isfloating)
                         XRaiseWindow(dpy, c->win);
               }
          }
     }

     for (c = monclients; c; c = next) {
          next = c->next;
          mon = c->mon;
          c->mon = selmon;
          c->tags = selmon->tagset[selmon->seltags];

          if (selmon->clients) {
               for (last = selmon->clients; last && last->next; last = last->next);
               last->next = c;
          } else
               selmon->clients = c;
          c->next = NULL;
          attachstack(c);

          if (c->isfloating) {
               if (c->w > selmon->ww)
                    c->w = selmon->ww - c->bw * 2;
               if (c->h > selmon->wh)
                    c->h = selmon->wh - c->bw * 2;

               if (numscratchpads > 1) {
                    c->x = c->mon->wx + (c->x - mon->wx) * ((double)(abs(c->mon->ww - WIDTH(c))) / MAX(abs(mon->ww - WIDTH(c)), 1));
                    c->y = c->mon->wy + (c->y - mon->wy) * ((double)(abs(c->mon->wh - HEIGHT(c))) / MAX(abs(mon->wh - HEIGHT(c)), 1));
               } else if (c->x < c->mon->mx || c->x > c->mon->mx + c->mon->mw ||
                          c->y < c->mon->my || c->y > c->mon->my + c->mon->mh)     {
                    c->x = c->mon->wx + (c->mon->ww / 2 - WIDTH(c) / 2);
                    c->y = c->mon->wy + (c->mon->wh / 2 - HEIGHT(c) / 2);
               }
               resizeclient(c, c->x, c->y, c->w, c->h);
               XRaiseWindow(dpy, c->win);
          }
     }

     if (found) {
          focus(ISVISIBLE(found) ? found : NULL);
          arrange(NULL);
          if (found->isfloating)
               XRaiseWindow(dpy, found->win);
     } else {
          spawnscratch(arg);
     }
}

void
toggletag(const Arg *arg)
{
    unsigned int newtags;

    if (!selmon->sel)
        return;
    newtags = selmon->sel->tags ^ (arg->ui & TAGMASK);
    if (newtags) {
        selmon->sel->tags = newtags;
        focus(NULL);
        arrange(selmon);
    }
}

void
toggleview(const Arg *arg)
{
    unsigned int newtagset = selmon->tagset[selmon->seltags] ^ (arg->ui & TAGMASK);

    // the first visible client should be the same after we add a new tag
    // we also want to be sure not to mutate the focus
    Client *const c = nexttiled(selmon->clients);
    if (c) {
        Client * const selected = selmon->sel;
        pop(c);
        focus(selected);
    }

    switchtag();
    selmon->tagset[selmon->seltags] = newtagset;
    focus(NULL);
    arrange(selmon);
    updatecurrentdesktop();
}

void
togglevacant(const Arg *arg)
{
    selmon->pertag->vactags[selmon->pertag->curtag] = selmon->vactag = selmon->vactag ? 0 : 1;
    drawbar(selmon);
}

void
toggletopbar(const Arg *arg)
{
    selmon->topbar = selmon->pertag->topbar[selmon->pertag->curtag] = selmon->topbar ? 0 : 1;
    setgaps(gappoh, gappov, gappih, gappiv);
    drawbar(selmon);
}

void
togglepadding(const Arg *arg)
{
    if (selmon->pertag->tpadding[selmon->pertag->curtag]) {
        selmon->vp = selmon->pertag->vertpd[selmon->pertag->curtag] = 0;
        selmon->sp = selmon->pertag->sidepd[selmon->pertag->curtag] = 0;
        selmon->smartgaps = selmon->pertag->smartgaps[selmon->pertag->curtag] = 1;
        setgaps(gappoh, gappov, gappih, gappiv);
    } else {
        selmon->vp = selmon->pertag->vertpd[selmon->pertag->curtag] = vertpadtoggle;
        selmon->sp = selmon->pertag->sidepd[selmon->pertag->curtag] = sidepadtoggle;
        selmon->smartgaps = selmon->pertag->smartgaps[selmon->pertag->curtag] = 0;
        setgaps(sidepadtoggle, sidepadtoggle, gappih, gappiv);
    }
    updatebarpos(selmon);
    selmon->pertag->tpadding[selmon->pertag->curtag] = !selmon->pertag->tpadding[selmon->pertag->curtag];
}

void
togglepreview(const Arg *arg)
{
    showpreview = !showpreview;
}

void
unfocus(Client *c, int setfocus)
{
    if (!c)
        return;
    grabbuttons(c, 0);
    if (c->issticky)
        XSetWindowBorder(dpy, c->win, scheme[SchemeNorm][ColSticky].pixel);
    else if (c->isfloating)
        XSetWindowBorder(dpy, c->win, scheme[SchemeNorm][ColFloat].pixel);
    else {
        for (unsigned int i = 0; i < 9; i++) {
            if (&layouts[i] == selmon->lt[selmon->sellt]) {
                XSetWindowBorder(dpy, c->win, scheme[SchemeNormLayout][i].pixel);
                break;
            }
        }
    }
    if (setfocus) {
        XSetInputFocus(dpy, root, RevertToPointerRoot, CurrentTime);
        XDeleteProperty(dpy, root, netatom[NetActiveWindow]);
    }

    updatecurrentdesktop();
}

void
unmanage(Client *c, int destroyed)
{
    Monitor *m = c->mon;
    unsigned int switchtag = c->switchtag;
    XWindowChanges wc;

    if (c->swallowing) {
        unswallow(c);
        return;
    }

    Client *s = swallowingclient(c->win);
    if (s) {
        free(s->swallowing);
        s->swallowing = NULL;
        arrange(m);
        focus(NULL);
        return;
    }

    detach(c);
    detachstack(c);
    if (!destroyed) {
        wc.border_width = c->oldbw;
        XGrabServer(dpy); /* avoid race conditions */
        XSetErrorHandler(xerrordummy);
        XConfigureWindow(dpy, c->win, CWBorderWidth, &wc); /* restore border */
        XUngrabButton(dpy, AnyButton, AnyModifier, c->win);
        setclientstate(c, WithdrawnState);
        XSync(dpy, False);
        XSetErrorHandler(xerror);
        XUngrabServer(dpy);
    }
    free(c);

    if (!s) {
        arrange(m);
        focus(getclientundermouse());
        updateclientlist();
    }
    if (switchtag)
        view(&((Arg) { .ui = switchtag }));
}

void
unmapnotify(XEvent *e)
{
    Client *c;
    XUnmapEvent *ev = &e->xunmap;

    if ((c = wintoclient(ev->window))) {
        if (ev->send_event)
            setclientstate(c, WithdrawnState);
        else
            unmanage(c, 0);
    }
    else if ((c = wintosystrayicon(ev->window))) {
        /* KLUDGE! sometimes icons occasionally unmap their windows, but do
         * _not_ destroy them. We map those windows back */
        XMapRaised(dpy, c->win);
        updatesystray();
    }
}

void
updatebars(void)
{
    unsigned int w;
    Monitor *m;
    XSetWindowAttributes wa = {
        .override_redirect = True,
        .background_pixmap = ParentRelative,
        .event_mask = ButtonPressMask|ExposureMask|PointerMotionMask
    };
    XClassHint ch = {"dwm", "dwm"};
    for (m = mons; m; m = m->next) {
        if (m->barwin)
            continue;
        w = m->ww;
        if (showsystray && m == systraytomon(m))
            w -= getsystraywidth();
        m->barwin = XCreateWindow(dpy, root, m->wx + selmon->sp, m->by + m->vp, m->ww - 2 * selmon->sp, bh, 0, DefaultDepth(dpy, screen),
                                  CopyFromParent, DefaultVisual(dpy, screen),
                                  CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
        XDefineCursor(dpy, m->barwin, cursor[CurNormal]->cursor);
        if (showsystray && m == systraytomon(m))
            XMapRaised(dpy, systray->win);
        XMapRaised(dpy, m->barwin);
        m->tabwin = XCreateWindow(dpy, root, m->wx, m->ty, m->ww, th, 0, DefaultDepth(dpy, screen),
                                  CopyFromParent, DefaultVisual(dpy, screen),
                                  CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
        XChangeProperty(dpy, m->barwin, netatom[NetWMWindowType], XA_ATOM, 32,
                        PropModeReplace, (unsigned char *) & netatom[NetWMWindowTypeDock], 1);
        XDefineCursor(dpy, m->tabwin, cursor[CurNormal]->cursor);
        XMapRaised(dpy, m->tabwin);
        XSetClassHint(dpy, m->barwin, &ch);
    }
}

void
updatebarpos(Monitor *m)
{
    Client *c;
    int nvis = 0;

    m->wy = m->my;
    m->wh = m->mh;
    if (m->showbar) {
        m->wh = m->wh - m->vp - bh;
        m->by = m->topbar ? m->wy : m->wy + m->wh + m->vp;
        m->wy = m->topbar ? m->wy + bh + m->vp : m->wy;
    } else {
        m->by = -bh - m->vp;
    }

    for(c = m->clients; c; c = c->next) {
        if (ISVISIBLE(c)) ++nvis;
    }

    if (m->showtab == showtab_always
       || ((m->showtab == showtab_auto) && (nvis > 1) && (m->lt[m->sellt]->arrange == monocle))) {
        m->wh -= th;
        m->ty = m->toptab ? m->wy : m->wy + m->wh;
        if ( m->toptab )
            m->wy += th;
    } else {
        m->ty = -th;
    }
}

void
updateclientlist()
{
    Client *c;
    Monitor *m;

    XDeleteProperty(dpy, root, netatom[NetClientList]);
    for (m = mons; m; m = m->next)
        for (c = m->clients; c; c = c->next)
            XChangeProperty(dpy, root, netatom[NetClientList],
                XA_WINDOW, 32, PropModeAppend,
                (unsigned char *) &(c->win), 1);
}
void
updatecurrentdesktop(void)
{
    long rawdata[] = { selmon->tagset[selmon->seltags] };
    int i=0;
    while(*rawdata >> (i+1)) {
        i++;
    }
    long data[] = { i };
    XChangeProperty(dpy, root, netatom[NetCurrentDesktop], XA_CARDINAL, 32, PropModeReplace, (unsigned char *)data, 1);
}

int
updategeom(void)
{
    int dirty = 0;

#ifdef XINERAMA
    if (XineramaIsActive(dpy)) {
        int i, j, n, nn;
        Client *c;
        Monitor *m;
        XineramaScreenInfo *info = XineramaQueryScreens(dpy, &nn);
        XineramaScreenInfo *unique = NULL;

        for (n = 0, m = mons; m; m = m->next, n++);
        /* only consider unique geometries as separate screens */
        unique = ecalloc(nn, sizeof(XineramaScreenInfo));
        for (i = 0, j = 0; i < nn; i++)
            if (isuniquegeom(unique, j, &info[i]))
                memcpy(&unique[j++], &info[i], sizeof(XineramaScreenInfo));
        XFree(info);
        nn = j;
        if (n <= nn) { /* new monitors available */
            for (i = 0; i < (nn - n); i++) {
                for (m = mons; m && m->next; m = m->next);
                if (m)
                    m->next = createmon();
                else
                    mons = createmon();
            }
            for (i = 0, m = mons; i < nn && m; m = m->next, i++)
                if (i >= n
                || unique[i].x_org != m->mx || unique[i].y_org != m->my
                || unique[i].width != m->mw || unique[i].height != m->mh)
                {
                    dirty = 1;
                    m->num = i;
                    m->mx = m->wx = unique[i].x_org;
                    m->my = m->wy = unique[i].y_org;
                    m->mw = m->ww = unique[i].width;
                    m->mh = m->wh = unique[i].height;
                    updatebarpos(m);
                }
        } else { /* less monitors available nn < n */
            for (i = nn; i < n; i++) {
                for (m = mons; m && m->next; m = m->next);
                while ((c = m->clients)) {
                    dirty = 1;
                    m->clients = c->next;
                    detachstack(c);
                    c->mon = mons;
                    if ( attachbelow )
                        attachBelow(c);
                    else
                        attach(c);
                    attachstack(c);
                }
                if (m == selmon)
                    selmon = mons;
                cleanupmon(m);
            }
        }
        free(unique);
    } else
#endif /* XINERAMA */
    { /* default monitor setup */
        if (!mons)
            mons = createmon();
        if (mons->mw != sw || mons->mh != sh) {
            dirty = 1;
            mons->mw = mons->ww = sw;
            mons->mh = mons->wh = sh;
            updatebarpos(mons);
        }
    }
    if (dirty) {
        selmon = mons;
        selmon = wintomon(root);
    }
    return dirty;
}

void
updatemotifhints(Client *c)
{
    Atom real;
    int format;
    unsigned char *p = NULL;
    unsigned long n, extra;
    unsigned long *motif;
    int width, height;

    if (!decorhints)
        return;

    if (XGetWindowProperty(dpy, c->win, motifatom, 0L, 5L, False, motifatom,
                           &real, &format, &n, &extra, &p) == Success && p != NULL) {
        motif = (unsigned long*)p;
        if (motif[MWM_HINTS_FLAGS_FIELD] & MWM_HINTS_DECORATIONS) {
            width = WIDTH(c);
            height = HEIGHT(c);

            if (motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_ALL ||
                motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_BORDER ||
                motif[MWM_HINTS_DECORATIONS_FIELD] & MWM_DECOR_TITLE)
                c->bw = c->oldbw = borderpx;
            else
                c->bw = c->oldbw = 0;

            resize(c, c->x, c->y, width - (2*c->bw), height - (2*c->bw), 0);
        }
        XFree(p);
    }
}

void
updatenumlockmask(void)
{
    unsigned int i, j;
    XModifierKeymap *modmap;

    numlockmask = 0;
    modmap = XGetModifierMapping(dpy);
    for (i = 0; i < 8; i++)
        for (j = 0; j < modmap->max_keypermod; j++)
            if (modmap->modifiermap[i * modmap->max_keypermod + j]
                == XKeysymToKeycode(dpy, XK_Num_Lock))
                numlockmask = (1 << i);
    XFreeModifiermap(modmap);
}

void
updatesizehints(Client *c)
{
    long msize;
    XSizeHints size;

    if (!XGetWMNormalHints(dpy, c->win, &size, &msize))
        /* size is uninitialized, ensure that size.flags aren't used */
        size.flags = PSize;
    if (size.flags & PBaseSize) {
        c->basew = size.base_width;
        c->baseh = size.base_height;
    } else if (size.flags & PMinSize) {
        c->basew = size.min_width;
        c->baseh = size.min_height;
    } else
        c->basew = c->baseh = 0;
    if (size.flags & PResizeInc) {
        c->incw = size.width_inc;
        c->inch = size.height_inc;
    } else
        c->incw = c->inch = 0;
    if (size.flags & PMaxSize) {
        c->maxw = size.max_width;
        c->maxh = size.max_height;
    } else
        c->maxw = c->maxh = 0;
    if (size.flags & PMinSize) {
        c->minw = size.min_width;
        c->minh = size.min_height;
    } else if (size.flags & PBaseSize) {
        c->minw = size.base_width;
        c->minh = size.base_height;
    } else
        c->minw = c->minh = 0;
    if (size.flags & PAspect) {
        c->mina = (float)size.min_aspect.y / size.min_aspect.x;
        c->maxa = (float)size.max_aspect.x / size.max_aspect.y;
    } else
        c->maxa = c->mina = 0.0;
    c->isfixed = (c->maxw && c->maxh && c->maxw == c->minw && c->maxh == c->minh);
}

void
updatestatus(void)
{
    Monitor* m;
    if (!gettextprop(root, XA_WM_NAME, stext, sizeof(stext)))
        strcpy(stext, "dwm-"VERSION);
    for(m = mons; m; m = m->next)
        drawbar(m);
    updatesystray();
}

void
updatesystrayicongeom(Client *i, int w, int h)
{
    if (i) {
        i->h = bh;
        if (w == h)
            i->w = bh;
        else if (h == bh)
            i->w = w;
        else
            i->w = (int) ((float)bh * ((float)w / (float)h));
        applysizehints(i, &(i->x), &(i->y), &(i->w), &(i->h), False);
        /* force icons into the systray dimensions if they don't want to */
        if (i->h > bh) {
            if (i->w == i->h)
                i->w = bh;
            else
                i->w = (int) ((float)bh * ((float)i->w / (float)i->h));
            i->h = bh;
        }
    }
}

void
updatesystrayiconstate(Client *i, XPropertyEvent *ev)
{
    long flags;
    int code = 0;

    if (!showsystray || !i || ev->atom != xatom[XembedInfo] ||
            !(flags = getatomprop(i, xatom[XembedInfo], XA_ATOM)))
        return;

    if (flags & XEMBED_MAPPED && !i->tags) {
        i->tags = 1;
        code = XEMBED_WINDOW_ACTIVATE;
        XMapRaised(dpy, i->win);
        setclientstate(i, NormalState);
    }
    else if (!(flags & XEMBED_MAPPED) && i->tags) {
        i->tags = 0;
        code = XEMBED_WINDOW_DEACTIVATE;
        XUnmapWindow(dpy, i->win);
        setclientstate(i, WithdrawnState);
    }
    else
        return;
    sendevent(i->win, xatom[Xembed], StructureNotifyMask, CurrentTime, code, 0,
            systray->win, XEMBED_EMBEDDED_VERSION);
}

void
updatesystray(void)
{
    XSetWindowAttributes wa;
    XWindowChanges wc;
    Client *i;
    Monitor *m = systraytomon(NULL);
    unsigned int x = m->mx + m->mw - m->sp;
    unsigned int w = 1;

    if (!showsystray)
        return;
    if (!systray) {
        /* init systray */
        if (!(systray = (Systray *)calloc(1, sizeof(Systray))))
            die("fatal: could not malloc() %u bytes\n", sizeof(Systray));
        systray->win = XCreateSimpleWindow(dpy, root, x, m->by + m->vp, w, bh, 0, 0, scheme[SchemeSystray][0].pixel);
        wa.event_mask        = ButtonPressMask | ExposureMask;
        wa.override_redirect = True;
        wa.background_pixel  = scheme[SchemeSystray][0].pixel;
        XSelectInput(dpy, systray->win, SubstructureNotifyMask);
        XChangeProperty(dpy, systray->win, netatom[NetSystemTrayOrientation], XA_CARDINAL, 32,
                PropModeReplace, (unsigned char *)&netatom[NetSystemTrayOrientationHorz], 1);
        XChangeWindowAttributes(dpy, systray->win, CWEventMask|CWOverrideRedirect|CWBackPixel, &wa);
        XMapRaised(dpy, systray->win);
        XSetSelectionOwner(dpy, netatom[NetSystemTray], systray->win, CurrentTime);
        if (XGetSelectionOwner(dpy, netatom[NetSystemTray]) == systray->win) {
            sendevent(root, xatom[Manager], StructureNotifyMask, CurrentTime, netatom[NetSystemTray], systray->win, 0, 0);
            XSync(dpy, False);
        }
        else {
            fprintf(stderr, "dwm: unable to obtain system tray.\n");
            free(systray);
            systray = NULL;
            return;
        }
    }
    for (w = 0, i = systray->icons; i; i = i->next) {
        /* make sure the background color stays the same */
        wa.background_pixel  = scheme[SchemeSystray][0].pixel;
        XChangeWindowAttributes(dpy, i->win, CWBackPixel, &wa);
        XMapRaised(dpy, i->win);
        w += systrayspacing;
        i->x = w;
        XMoveResizeWindow(dpy, i->win, i->x, 0, i->w, i->h);
        w += i->w;
        if (i->mon != m)
            i->mon = m;
    }
    w = w ? w + systrayspacing : 1;
    x -= w;
    XMoveResizeWindow(dpy, systray->win, x, m->by + m->vp, w, bh);
    wc.x = x; wc.y = m->by + m->vp; wc.width = w; wc.height = bh;
    wc.stack_mode = Above; wc.sibling = m->barwin;
    XConfigureWindow(dpy, systray->win, CWX|CWY|CWWidth|CWHeight|CWSibling|CWStackMode, &wc);
    XMapWindow(dpy, systray->win);
    XMapSubwindows(dpy, systray->win);
    /* redraw background */
    XSetForeground(dpy, drw->gc, scheme[SchemeSystray][0].pixel);
    XFillRectangle(dpy, systray->win, drw->gc, 0, 0, w, bh);
    XSync(dpy, False);
}

void
updatetitle(Client *c)
{
    if (!gettextprop(c->win, netatom[NetWMName], c->name, sizeof c->name))
        gettextprop(c->win, XA_WM_NAME, c->name, sizeof c->name);
    if (c->name[0] == '\0') /* hack to mark broken clients */
        strcpy(c->name, broken);
}

void
updatepreview(void)
{
    Monitor *m;

    XSetWindowAttributes wa = {
        .override_redirect = True,
        .background_pixmap = ParentRelative,
        .event_mask = ButtonPressMask|ExposureMask
    };
    for (m = mons; m; m = m->next) {
        m->tagwin = XCreateWindow(dpy, root, m->wx, m->by + bh, m->mw / scalepreview, m->mh / scalepreview, 0,
                DefaultDepth(dpy, screen), CopyFromParent, DefaultVisual(dpy, screen),
                CWOverrideRedirect|CWBackPixmap|CWEventMask, &wa);
        XDefineCursor(dpy, m->tagwin, cursor[CurNormal]->cursor);
        XMapRaised(dpy, m->tagwin);
        XUnmapWindow(dpy, m->tagwin);
    }
}

void
updatewindowtype(Client *c)
{
    Atom state = getatomprop(c, netatom[NetWMState], XA_ATOM);
    Atom wtype = getatomprop(c, netatom[NetWMWindowType], XA_ATOM);

    if (state == netatom[NetWMFullscreen])
        setfullscreen(c, 1);
    if (wtype == netatom[NetWMWindowTypeDialog])
        c->isfloating = 1;
}

void
updatewmhints(Client *c)
{
    XWMHints *wmh;

    if ((wmh = XGetWMHints(dpy, c->win))) {
        if (c == selmon->sel && wmh->flags & XUrgencyHint) {
            wmh->flags &= ~XUrgencyHint;
            XSetWMHints(dpy, c->win, wmh);
        } else
            c->isurgent = (wmh->flags & XUrgencyHint) ? 1 : 0;
        if (wmh->flags & InputHint)
            c->neverfocus = !wmh->input;
        else
            c->neverfocus = 0;
        XFree(wmh);
    }
}

void
view(const Arg *arg)
{
    int i;
    unsigned int tmptag;

    if (arg->ui && (arg->ui & TAGMASK) == selmon->tagset[selmon->seltags] && arg->i != INT_MAX) {
        view(&((Arg) { .ui = 0 }));
        return;
    }

    prevmon = NULL;
    switchtag();
    selmon->seltags ^= 1; /* toggle sel tagset */
    if (arg->ui & TAGMASK) {
        selmon->pertag->prevtag = selmon->pertag->curtag;
        selmon->tagset[selmon->seltags] = arg->ui & TAGMASK;
        if (arg->ui == ~0)
            selmon->pertag->curtag = 0;
        else {
            for (i=0; !(arg->ui & 1 << i); i++) ;
            selmon->pertag->curtag = i;
        }
    } else {
        tmptag = selmon->pertag->prevtag;
        selmon->pertag->prevtag = selmon->pertag->curtag;
        selmon->pertag->curtag = tmptag;
    }

    selmon->nmaster = selmon->pertag->nmasters[selmon->pertag->curtag];
    selmon->mfact = selmon->pertag->mfacts[selmon->pertag->curtag];
    selmon->sellt = selmon->pertag->sellts[selmon->pertag->curtag];
    selmon->lt[selmon->sellt] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt];
    selmon->lt[selmon->sellt^1] = selmon->pertag->ltidxs[selmon->pertag->curtag][selmon->sellt^1];

    selmon->gappoh = (selmon->pertag->gaps[selmon->pertag->curtag] & 0xff) >> 0;
    selmon->gappov = (selmon->pertag->gaps[selmon->pertag->curtag] & 0xff00) >> 8;
    selmon->gappih = (selmon->pertag->gaps[selmon->pertag->curtag] & 0xff0000) >> 16;
    selmon->gappiv = (selmon->pertag->gaps[selmon->pertag->curtag] & 0xff000000) >> 24;

    selmon->vp = selmon->pertag->vertpd[selmon->pertag->curtag];
    selmon->sp = selmon->pertag->sidepd[selmon->pertag->curtag];
    selmon->smartgaps = selmon->pertag->smartgaps[selmon->pertag->curtag];
    selmon->vactag = selmon->pertag->vactags[selmon->pertag->curtag];
    selmon->borderpx = selmon->pertag->borderpx[selmon->pertag->curtag];
    selmon->topbar = selmon->pertag->topbar[selmon->pertag->curtag];
    selmon->showbar = selmon->pertag->showbars[selmon->pertag->curtag];

    focus(NULL);
    if (showsystray) {
        XWindowChanges wc;
        if (!selmon->showbar)
            wc.y = -bh;
        else if (selmon->showbar) {
            wc.y = 0;
            if (!selmon->topbar)
                wc.y = selmon->mh - bh;
        }
        XConfigureWindow(dpy, systray->win, CWY, &wc);
    }
    arrange(selmon);
    updatecurrentdesktop();

    if (selmon->sel && selmon->sel->isfullscreen) {
        resizeclient(selmon->sel, selmon->sel->mon->mx, selmon->sel->mon->my, selmon->sel->mon->mw, selmon->sel->mon->mh);
        XRaiseWindow(dpy, selmon->sel->win);
    }
}

void
warp(const Client *c)
{
    int x, y;

    if (!c) {
        XWarpPointer(dpy, None, root, 0, 0, 0, 0, selmon->wx + selmon->ww/2, selmon->wy + selmon->wh/2);
        return;
    }

    if (!getrootptr(&x, &y) ||
        (x > c->x - c->bw &&
         y > c->y - c->bw &&
         x < c->x + c->w + c->bw*2 &&
         y < c->y + c->h + c->bw*2) ||
        (y > c->mon->by && y < c->mon->by + bh) ||
        (c->mon->topbar && !y))
        return;

    XWarpPointer(dpy, None, c->win, 0, 0, 0, 0, c->w / 2, c->h / 2);
}

pid_t
winpid(Window w)
{

    pid_t result = 0;

#ifdef __linux__
    xcb_res_client_id_spec_t spec = {0};
    spec.client = w;
    spec.mask = XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID;

    xcb_generic_error_t *e = NULL;
    xcb_res_query_client_ids_cookie_t c = xcb_res_query_client_ids(xcon, 1, &spec);
    xcb_res_query_client_ids_reply_t *r = xcb_res_query_client_ids_reply(xcon, c, &e);

    if (!r)
        return (pid_t)0;

    xcb_res_client_id_value_iterator_t i = xcb_res_query_client_ids_ids_iterator(r);
    for (; i.rem; xcb_res_client_id_value_next(&i)) {
        spec = i.data->spec;
        if (spec.mask & XCB_RES_CLIENT_ID_MASK_LOCAL_CLIENT_PID) {
            uint32_t *t = xcb_res_client_id_value_value(i.data);
            result = *t;
            break;
        }
    }

    free(r);

    if (result == (pid_t)-1)
        result = 0;

#endif /* __linux__ */

#ifdef __OpenBSD__
        Atom type;
        int format;
        unsigned long len, bytes;
        unsigned char *prop;
        pid_t ret;

        if (XGetWindowProperty(dpy, w, XInternAtom(dpy, "_NET_WM_PID", 0), 0, 1, False, AnyPropertyType, &type, &format, &len, &bytes, &prop) != Success || !prop)
               return 0;

        ret = *(pid_t*)prop;
        XFree(prop);
        result = ret;

#endif /* __OpenBSD__ */
    return result;
}

pid_t
getparentprocess(pid_t p)
{
    unsigned int v = 0;

#ifdef __linux__
    FILE *f;
    char buf[256];
    snprintf(buf, sizeof(buf) - 1, "/proc/%u/stat", (unsigned)p);

    if (!(f = fopen(buf, "r")))
        return 0;

    fscanf(f, "%*u %*s %*c %u", &v);
    fclose(f);
#endif /* __linux__*/

#ifdef __OpenBSD__
    int n;
    kvm_t *kd;
    struct kinfo_proc *kp;

    kd = kvm_openfiles(NULL, NULL, NULL, KVM_NO_FILES, NULL);
    if (!kd)
        return 0;

    kp = kvm_getprocs(kd, KERN_PROC_PID, p, sizeof(*kp), &n);
    v = kp->p_ppid;
#endif /* __OpenBSD__ */

    return (pid_t)v;
}

int
isdescprocess(pid_t p, pid_t c)
{
    while (p != c && c != 0)
        c = getparentprocess(c);

    return (int)c;
}

Client *
termforwin(const Client *w)
{
    Client *c;
    Monitor *m;

    if (!w->pid || w->isterminal)
        return NULL;

    for (m = mons; m; m = m->next) {
        for (c = m->clients; c; c = c->next) {
            if (c->isterminal && !c->swallowing && c->pid && isdescprocess(c->pid, w->pid))
                return c;
        }
    }

    return NULL;
}

Client *
swallowingclient(Window w)
{
    Client *c;
    Monitor *m;

    for (m = mons; m; m = m->next) {
        for (c = m->clients; c; c = c->next) {
            if (c->swallowing && c->swallowing->win == w)
                return c;
        }
    }

    return NULL;
}
void
viewnextempty(const Arg *arg)
{
    view(&(const Arg){.ui = nexttag(0, 1)});
}

void
viewprevempty(const Arg *arg)
{
    view(&(const Arg){.ui = nexttag(1, 1)});
}

void
viewnext(const Arg *arg)
{
    view(&(const Arg){.ui = nexttag(0, 0)});
}

void
viewprev(const Arg *arg)
{
    view(&(const Arg){.ui = nexttag(1, 0)});
}
 
Client *
wintoclient(Window w)
{
    Client *c;
    Monitor *m;

    for (m = mons; m; m = m->next)
        for (c = m->clients; c; c = c->next)
            if (c->win == w)
                return c;
    return NULL;
}

Client *
wintosystrayicon(Window w) {
    Client *i = NULL;

    if (!showsystray || !w)
        return i;
    for (i = systray->icons; i && i->win != w; i = i->next) ;
    return i;
}

Monitor *
wintomon(Window w)
{
    int x, y;
    Client *c;
    Monitor *m;

    if (w == root && getrootptr(&x, &y))
        return recttomon(x, y, 1, 1);
    for (m = mons; m; m = m->next)
        if (w == m->barwin || w == m->tabwin)
            return m;
    if ((c = wintoclient(w)))
        return c->mon;
    return selmon;
}

/* Selects for the view of the focused window. The list of tags */
/* to be displayed is matched to the focused window tag list. */
void
winview(const Arg* arg){
    Window win, win_r, win_p, *win_c;
    unsigned nc;
    int unused;
    Client* c;
    Arg a;

    if (!XGetInputFocus(dpy, &win, &unused)) return;
    while(XQueryTree(dpy, win, &win_r, &win_p, &win_c, &nc)
          && win_p != win_r) win = win_p;

    if (!(c = wintoclient(win))) return;

    a.ui = c->tags;
    view(&a);
}

/* There's no way to check accesses to destroyed windows, thus those cases are
 * ignored (especially on UnmapNotify's). Other types of errors call Xlibs
 * default error handler, which may call exit. */
int
xerror(Display *dpy, XErrorEvent *ee)
{
    if (ee->error_code == BadWindow
    || (ee->request_code == X_SetInputFocus && ee->error_code == BadMatch)
    || (ee->request_code == X_PolyText8 && ee->error_code == BadDrawable)
    || (ee->request_code == X_PolyFillRectangle && ee->error_code == BadDrawable)
    || (ee->request_code == X_PolySegment && ee->error_code == BadDrawable)
    || (ee->request_code == X_ConfigureWindow && ee->error_code == BadMatch)
    || (ee->request_code == X_GrabButton && ee->error_code == BadAccess)
    || (ee->request_code == X_GrabKey && ee->error_code == BadAccess)
    || (ee->request_code == X_CopyArea && ee->error_code == BadDrawable))
        return 0;
    fprintf(stderr, "dwm: fatal error: request code=%d, error code=%d\n",
        ee->request_code, ee->error_code);
    return xerrorxlib(dpy, ee); /* may call exit */
}

int
xerrordummy(Display *dpy, XErrorEvent *ee)
{
    return 0;
}

/* Startup Error handler to check if another window manager
 * is already running. */
int
xerrorstart(Display *dpy, XErrorEvent *ee)
{
    die("dwm: another window manager is already running");
    return -1;
}

Monitor *
systraytomon(Monitor *m) {
    Monitor *t;
    int i, n;
    if (!systraypinning) {
        if (!m)
            return selmon;
        return m == selmon ? m : NULL;
    }
    for(n = 1, t = mons; t && t->next; n++, t = t->next) ;
    for(i = 1, t = mons; t && t->next && i < systraypinning; i++, t = t->next) ;
    if (systraypinningfailfirst && n < systraypinning)
        return mons;
    return t;
}

void
zoom(const Arg *arg)
{
    if (selmon->sel && selmon->sel->isfloating)
        togglefloating(NULL);

    Client *c = selmon->sel;
    Client *at = NULL, *cold, *cprevious = NULL;

    if (!selmon->lt[selmon->sellt]->arrange
    || (selmon->sel && selmon->sel->isfloating))
        return;
    if (c == nexttiled(selmon->clients)) {
        at = findbefore(prevzoom);
        if (at)
            cprevious = nexttiled(at->next);
        if (!cprevious || cprevious != prevzoom) {
            prevzoom = NULL;
            if (!c || !(c = nexttiled(c->next)))
                return;
        } else
            c = cprevious;
    }
    cold = nexttiled(selmon->clients);
    if (c != cold && !at)
        at = findbefore(c);
    detach(c);
    attach(c);
    /* swap windows instead of pushing the previous one down */
    if (c != cold && at) {
        prevzoom = cold;
        if (cold && at != cold) {
            detach(cold);
            cold->next = at->next;
            at->next = cold;
        }
    }
    focus(c);
    arrange(c->mon);
}

int
main(int argc, char *argv[])
{
    if (argc == 2 && !strcmp("-v", argv[1]))
        die("dwm-"VERSION);
    else if (argc != 1 && strcmp("-s", argv[1]))
        die("usage: dwm [-v]");
    if (!setlocale(LC_CTYPE, "") || !XSupportsLocale())
        fputs("warning: no locale support\n", stderr);
    if (!(dpy = XOpenDisplay(NULL)))
        die("dwm: cannot open display");
    if (!(xcon = XGetXCBConnection(dpy)))
        die("dwm: cannot get xcb connection\n");
    if (argc > 1 && !strcmp("-s", argv[1])) {
        XStoreName(dpy, RootWindow(dpy, DefaultScreen(dpy)), argv[2]);
        XCloseDisplay(dpy);
        return 0;
    }
    checkotherwm();
    setup();
#ifdef __OpenBSD__
    if (pledge("stdio rpath proc exec ps", NULL) == -1)
        die("pledge");
#endif /* __OpenBSD__ */
    scan();
    run();
    if (restart)
        execvp(argv[0], argv);
    cleanup();
    XCloseDisplay(dpy);
    return EXIT_SUCCESS;
}

void
focusmaster(const Arg *arg)
{
    Client *c;

    if (selmon->nmaster < 1)
        return;

    c = nexttiled(selmon->clients);

    if (c)
        focus(c);
}

void
transfer(const Arg *arg) {
    Client *c, *mtail = selmon->clients, *stail = NULL, *insertafter;
    int transfertostack = 0, i, nmasterclients;

    for (i = 0, c = selmon->clients; c; c = c->next) {
        if (!ISVISIBLE(c) || c->isfloating) continue;
        if (selmon->sel == c) { transfertostack = i < selmon->nmaster && selmon->nmaster != 0; }
        if (i < selmon->nmaster) { nmasterclients++; mtail = c; }
        stail = c;
        i++;
    }
    if (!selmon->sel || selmon->sel->isfloating || i == 0) {
        return;
    } else if (transfertostack) {
        selmon->nmaster = MIN(i, selmon->nmaster) - 1;
        insertafter = stail;
    } else {
        selmon->nmaster = selmon->nmaster + 1;
        insertafter = mtail;
    }
    if (insertafter != selmon->sel) {
        detach(selmon->sel);
        if (selmon->nmaster == 1 && !transfertostack) {
         attach(selmon->sel); // Head prepend case
        } else {
            selmon->sel->next = insertafter->next;
            insertafter->next = selmon->sel;
        }
    }
    arrange(selmon);
}

void
insertclient(Client *item, Client *insertItem, int after) {
    Client *c;

    if (item == NULL || insertItem == NULL || item == insertItem) return;
        detach(insertItem);
    if (!after && selmon->clients == item) {
        attach(insertItem);
        return;
    }
    if (after) {
        c = item;
    } else {
        for (c = selmon->clients; c; c = c->next) { if (c->next == item) break; }
    }
    insertItem->next = c->next;
    c->next = insertItem;
}
