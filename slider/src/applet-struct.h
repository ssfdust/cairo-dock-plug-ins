
#ifndef __CD_APPLET_STRUCT__
#define  __CD_APPLET_STRUCT__

#include <glib/gi18n.h>
#include <glib/gstdio.h>


typedef enum {
	SLIDER_DEFAULT = 0,
	SLIDER_FADE,
	SLIDER_BLANK_FADE,
	SLIDER_FADE_IN_OUT,
	SLIDER_SIDE_KICK,
	SLIDER_DIAPORAMA,
	SLIDER_GROW_UP,
	SLIDER_SHRINK_DOWN,
	SLIDER_RANDOM,
	SLIDER_NB_ANIMATION,
} SliderAnimation;

typedef enum {
	SLIDER_PAUSE = 0,
	SLIDER_OPEN_IMAGE,
	SLIDER_NB_CLICK_OPTION,
} SliderClickOption;

typedef struct {
	double fImgX;
	double fImgY;
	double fImgW;
	double fImgH;
} myImgLips;

//\___________ structure containing the applet's configuration parameters.
typedef struct {
	gint iSlideTime;
	gchar *cDirectory;
	gchar *cFrameImage;
	gchar *cReflectImage;
	gboolean bSubDirs;
	gboolean bNoStrench;
	gboolean bFillIcon;
	gboolean bRandom;
	gdouble pBackgroundColor[4];
	gdouble pFrameAlpha;
	gdouble pFrameOffset;
	gdouble pReflectAlpha;
	SliderAnimation iAnimation;
	SliderClickOption iClickOption;
} AppletConfig;

//\___________ structure containing the applet's data, like surfaces, dialogs, results of calculus, etc.
typedef struct {
	GList *pList;
	GList *pElement;
	gboolean bPause;
	gdouble fAnimAlpha;
	gdouble fAnimCNT;
	gint iAnimCNT;
	gint iAnimTimerID;
	gint iTimerID;
	myImgLips pImgL;
	myImgLips pPrevImgL;
	cairo_surface_t* pCairoSurface;
	cairo_surface_t* pPrevCairoSurface;
	gdouble fSurfaceWidth, fSurfaceHeight;
	cairo_surface_t* pCairoFrameSurface;
	cairo_surface_t* pCairoReflectSurface;
	SliderAnimation iAnimation;
	gchar *cNowImage;
} AppletData;


#endif
