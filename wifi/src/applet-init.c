#include "stdlib.h"

#include "applet-config.h"
#include "applet-notifications.h"
#include "applet-struct.h"
#include "applet-wifi.h"
#include "applet-draw.h"
#include "applet-init.h"


CD_APPLET_DEFINITION ("wifi",
	1, 6, 2,
	CAIRO_DOCK_CATEGORY_ACCESSORY,
	N_("This applet shows you the signal strength of the first active wifi connection\n"
	"Left-click to pop-up some info,"
	"Middle-click to re-check immediately."),
	"ChAnGFu (Rémy Robertson)");


static void _set_data_renderer (CairoDockModuleInstance *myApplet, gboolean bReload)
{
	CairoDataRendererAttribute *pRenderAttr = NULL;  // les attributs du data-renderer global.
	if (myConfig.iDisplayType == CD_WIFI_GAUGE)
	{
		CairoGaugeAttribute attr;  // les attributs de la jauge.
		memset (&attr, 0, sizeof (CairoGaugeAttribute));
		pRenderAttr = CAIRO_DATA_RENDERER_ATTRIBUTE (&attr);
		pRenderAttr->cModelName = "gauge";
		attr.cThemePath = myConfig.cGThemePath;
	}
	else if (myConfig.iDisplayType == CD_WIFI_GRAPH)
	{
		CairoGraphAttribute attr;  // les attributs du graphe.
		memset (&attr, 0, sizeof (CairoGraphAttribute));
		pRenderAttr = CAIRO_DATA_RENDERER_ATTRIBUTE (&attr);
		pRenderAttr->cModelName = "graph";
		pRenderAttr->iMemorySize = 32;  // bon compromis sur la lisibilite.
		attr.iType = myConfig.iGraphType;
		attr.iRadius = 10;
		attr.fHighColor = myConfig.fHigholor;
		attr.fLowColor = myConfig.fLowColor;
		memcpy (attr.fBackGroundColor, myConfig.fBgColor, 4*sizeof (double));
	}
	else if (myConfig.iDisplayType == CD_WIFI_BAR)
	{
		/// A FAIRE...
	}
	if (pRenderAttr != NULL)
	{
		pRenderAttr->iLatencyTime = myConfig.iCheckInterval * 1000 * myConfig.fSmoothFactor;
		pRenderAttr->iNbValues = 2;
		//pRenderAttr->bWriteValues = TRUE;
		if (! bReload)
			CD_APPLET_ADD_DATA_RENDERER_ON_MY_ICON (pRenderAttr);
		else
			CD_APPLET_RELOAD_MY_DATA_RENDERER (pRenderAttr);
	}
}

CD_APPLET_INIT_BEGIN
	if (myDesklet != NULL)
		CD_APPLET_SET_DESKLET_RENDERER ("Simple");
	
	// Initialisation du rendu.
	_set_data_renderer (myApplet, FALSE);
	
	// Initialisation de la tache periodique de mesure.
	myData.iPreviousQuality = -1;  // force le dessin.
	myData.prev_prcnt = -1;
	myData.pTask = cairo_dock_new_task (myConfig.iCheckInterval,
		(CairoDockGetDataAsyncFunc) cd_wifi_get_data,
		(CairoDockUpdateSyncFunc) cd_wifi_update_from_data,
		myApplet);
	cairo_dock_launch_task_delayed (myData.pTask, 5000);
	
	CD_APPLET_REGISTER_FOR_CLICK_EVENT;
	CD_APPLET_REGISTER_FOR_BUILD_MENU_EVENT;
	CD_APPLET_REGISTER_FOR_MIDDLE_CLICK_EVENT;
CD_APPLET_INIT_END


CD_APPLET_STOP_BEGIN
	//\_______________ On se desabonne de nos notifications.
	CD_APPLET_UNREGISTER_FOR_CLICK_EVENT;
	CD_APPLET_UNREGISTER_FOR_BUILD_MENU_EVENT;
	CD_APPLET_UNREGISTER_FOR_MIDDLE_CLICK_EVENT;
CD_APPLET_STOP_END


CD_APPLET_RELOAD_BEGIN
	//\_______________ On recharge les donnees qui ont pu changer.
	if (myDesklet != NULL)
		CD_APPLET_SET_DESKLET_RENDERER ("Simple");
	
	int i; // reset surfaces utilisateurs.
	for (i = 0; i < WIFI_NB_QUALITY; i ++) {
		if (myData.pSurfaces[i] != NULL) {
			cairo_surface_destroy (myData.pSurfaces[i]);
			myData.pSurfaces[i] = NULL;
		}
	}
	
	//\_______________ On relance avec la nouvelle config ou on redessine.
	if (CD_APPLET_MY_CONFIG_CHANGED) {
		_set_data_renderer (myApplet, TRUE);
		
		CD_APPLET_SET_QUICK_INFO_ON_MY_ICON (NULL);
		myData.iPreviousQuality = -1;  // on force le redessin.
		myData.prev_prcnt = -1;
		cairo_dock_stop_task (myData.pTask);  // on stoppe avant car  on ne veut pas attendre la prochaine iteration.
		cairo_dock_change_task_frequency (myData.pTask, myConfig.iCheckInterval);
		cairo_dock_launch_task (myData.pTask);  // mesure immediate.
	}
	else {  // on redessine juste l'icone.
		myData.iPreviousQuality = -1;  // force le redessin.
		if (myData.bAcquisitionOK) {
			cd_wifi_draw_icon ();
		}
		else {
			cd_wifi_draw_no_wireless_extension ();
		}
	}
CD_APPLET_RELOAD_END
