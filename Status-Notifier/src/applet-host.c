/**
* This file is a part of the Cairo-Dock project
*
* Copyright : (C) see the 'copyright' file.
* E-mail    : see the 'copyright' file.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "applet-struct.h"
#include "applet-item.h"
#include "applet-host.h"

#define CD_STATUS_NOTIFIER_HOST_ADDR "org.kde.StatusNotifierHost"

#define CD_STATUS_NOTIFIER_WATCHER_ADDR2 "org.kde.NotificationItemWatcher"
#define CD_STATUS_NOTIFIER_WATCHER_OBJ2 "/NotificationItemWatcher"
#define CD_STATUS_NOTIFIER_WATCHER_IFACE2 "org.kde.NotificationItemWatcher"

#define CD_STATUS_NOTIFIER_WATCHER_ADDR "org.kde.StatusNotifierWatcher"
#define CD_STATUS_NOTIFIER_WATCHER_OBJ "/StatusNotifierWatcher"
#define CD_STATUS_NOTIFIER_WATCHER_IFACE "org.kde.StatusNotifierWatcher"

#define CD_INDICATOR_APPLICATION_ADDR "org.ayatana.indicator.application"
#define CD_INDICATOR_APPLICATION_OBJ "/org/ayatana/indicator/application/service"
#define CD_INDICATOR_APPLICATION_IFACE "org.ayatana.indicator.application.service"


void _cd_cclosure_marshal_VOID__STRING_INT_STRING_STRING_STRING_STRING_STRING (GClosure *closure,
	GValue *return_value G_GNUC_UNUSED,
	guint n_param_values,
	const GValue *param_values,
	gpointer invocation_hint G_GNUC_UNUSED,
	gpointer marshal_data)
{
	g_print ("%s ()\n", __func__);
	typedef void (*GMarshalFunc_VOID__STRING_INT_STRING_STRING_STRING_STRING_STRING) (
		gpointer     data1,
		gchar      *arg_1,
		gint        arg_2,
		gchar      *arg_3,
		gchar      *arg_4,
		gchar      *arg_5,
		gchar      *arg_6,
		gchar      *arg_7,
		gpointer     data2);
	register GMarshalFunc_VOID__STRING_INT_STRING_STRING_STRING_STRING_STRING callback;
	register GCClosure *cc = (GCClosure*) closure;
	register gpointer data1, data2;
	g_return_if_fail (n_param_values == 8);  // return_value est NULL ici, car la callback ne renvoit rien.

	if (G_CCLOSURE_SWAP_DATA (closure))
	{
		data1 = closure->data;
		data2 = g_value_peek_pointer (param_values + 0);
	}
	else
	{
		data1 = g_value_peek_pointer (param_values + 0);
		data2 = closure->data;
	}
	callback = (GMarshalFunc_VOID__STRING_INT_STRING_STRING_STRING_STRING_STRING) (marshal_data ? marshal_data : cc->callback);

	callback (data1,
		(char*) g_value_get_string (param_values + 1),
		g_value_get_int (param_values + 2),
		(char*) g_value_get_string (param_values + 3),
		(char*) g_value_get_string (param_values + 4),
		(char*) g_value_get_string (param_values + 5),
		(char*) g_value_get_string (param_values + 6),
		(char*) g_value_get_string (param_values + 7),
		data2);
}
 

static CDStatusNotifierItem * _cd_satus_notifier_find_item_from_service (const gchar *cService)
{
	CDStatusNotifierItem *pItem;
	GList *it;
	for (it = myData.pItems; it != NULL; it = it->next)
	{
		pItem = it->data;
		if (pItem->cService && strcmp (pItem->cService, cService) == 0)
			return pItem;
	}
	return NULL;
}


static void on_new_item (DBusGProxy *proxy_watcher, const gchar *cService, CairoDockModuleInstance *myApplet)
{
	CD_APPLET_ENTER;
	g_print ("new item : '%s'\n", cService);
	
	CDStatusNotifierItem *pItem = cd_satus_notifier_create_item (cService, NULL);
	CD_APPLET_LEAVE_IF_FAIL (pItem != NULL);
	
	myData.pItems = g_list_prepend (myData.pItems, pItem);
	
	if (myConfig.bCompactMode)
	{
		cairo_dock_load_icon_image (myIcon, myContainer);
	}
	else
	{
		Icon *pIcon = cairo_dock_create_dummy_launcher (g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
			g_strdup (pItem->cIconName),
			g_strdup (pItem->cService),
			NULL,
			pItem->iCategory);
		CD_APPLET_ADD_ICON_IN_MY_ICONS_LIST (pIcon);
	}
	
	CD_APPLET_LEAVE ();
}

static void on_removed_item (DBusGProxy *proxy_watcher, const gchar *cService, CairoDockModuleInstance *myApplet)
{
	CD_APPLET_ENTER;
	g_print ("item removed : '%s'\n", cService);
	
	gchar *str = strchr (cService, '/');
	if (str)
		*str = '\0';
	
	CDStatusNotifierItem *pItem = _cd_satus_notifier_find_item_from_service (cService);
	CD_APPLET_LEAVE_IF_FAIL (pItem != NULL);
	
	myData.pItems = g_list_remove (myData.pItems, pItem);
	
	if (myConfig.bCompactMode)
	{
		cairo_dock_load_icon_image (myIcon, myContainer);
	}
	else
	{
		Icon *pIcon = cairo_dock_create_dummy_launcher (g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
			g_strdup (pItem->cIconName),
			g_strdup (pItem->cService),
			NULL,
			pItem->iCategory);
		CD_APPLET_ADD_ICON_IN_MY_ICONS_LIST (pIcon);
	}
	
	cd_free_item (pItem);
	
	CD_APPLET_LEAVE ();
}


static int _compare_items (const CDStatusNotifierItem *i1, const CDStatusNotifierItem *i2)
{
	if (!i1)
		return -1;
	if (!i2)
		return 1;
	return (i1->iCategory < i2->iCategory ? -1 : (i1->iCategory > i2->iCategory ? 1 : 0));
}

static void _load_my_icon_image (Icon *pIcon)
{
	CD_APPLET_ENTER;
	int iWidth = pIcon->iImageWidth;
	int iHeight = pIcon->iImageHeight;
	
	if (myConfig.bCompactMode)
	{
		pIcon->pIconBuffer = cairo_dock_create_blank_surface (iWidth, iHeight);
		cairo_t *pIconContext = cairo_create (pIcon->pIconBuffer);
		/// for each item
			/// create surface
			/// draw
		cairo_destroy (pIconContext);
	}
	else
	{
		gchar *cIconPath = cairo_dock_search_icon_s_path (pIcon->cFileName);
		if (cIconPath != NULL && *cIconPath != '\0')
			pIcon->pIconBuffer = cairo_dock_create_surface_from_image_simple (cIconPath,
				iWidth,
				iHeight);
		g_free (cIconPath);
	}
	
	CD_APPLET_LEAVE ();
}


static void on_new_application (DBusGProxy *proxy_watcher, const gchar *cIconName, gint iPosition, const gchar *cAdress, const gchar *cObjectPath, const gchar *cIconThemePath, const gchar *cLabel, const gchar *cLabelGuide, CairoDockModuleInstance *myApplet)
{
	g_print ("%s (%s, %s, %s, %s, %d)\n", __func__, cAdress, cObjectPath, cIconName, cIconThemePath, iPosition);
	g_return_if_fail (cLabelGuide != NULL && cObjectPath != NULL);
	
	CDStatusNotifierItem *pItem = cd_satus_notifier_create_item (cAdress, cObjectPath);
	g_return_if_fail (pItem != NULL);
	pItem->iPosition = iPosition;
	myData.pItems = g_list_prepend (myData.pItems, pItem);
	
	if (! myConfig.bCompactMode)
	{
		Icon *pIcon = cairo_dock_create_dummy_launcher (
			g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
			g_strdup (pItem->cIconName),
			g_strdup (pItem->cService),
			NULL,
			pItem->iCategory);
		CD_APPLET_ADD_ICON_IN_MY_ICONS_LIST (pIcon);
	}
}

static void on_removed_application (DBusGProxy *proxy_watcher, gint iPosition, CairoDockModuleInstance *myApplet)
{
	g_print ("%s (%d)\n", __func__, iPosition);
	CDStatusNotifierItem *pItem = cd_satus_notifier_get_item_from_position (iPosition);
	g_return_if_fail (pItem != NULL);
	
	Icon *pIcon = cd_satus_notifier_get_icon_from_item (pItem);
	g_return_if_fail (pIcon != NULL);
	
	CD_APPLET_REMOVE_ICON_FROM_MY_ICONS_LIST (pIcon);
}


static void _on_get_applications_from_service (DBusGProxy *proxy, DBusGProxyCall *call_id, CairoDockModuleInstance *myApplet)
{
	g_print ("%s ()\n", __func__);
	CD_APPLET_ENTER;
	myData.pGetApplicationsCall = NULL;
	
	//\______________________ get the applications list from the service.
	GPtrArray *pApplications = NULL;
	GError *erreur = NULL;
	GType g_type_ptrarray = g_type_ptrarray = dbus_g_type_get_collection ("GPtrArray",
		dbus_g_type_get_struct("GValueArray",
			G_TYPE_STRING,  // iconname
			G_TYPE_INT,  // position
			G_TYPE_STRING,  // dbusaddress
			DBUS_TYPE_G_OBJECT_PATH,  // dbusobject
			G_TYPE_STRING,  // iconpath
			G_TYPE_STRING,  // label
			G_TYPE_STRING,  // labelguide
			G_TYPE_INVALID));
	gboolean bSuccess = dbus_g_proxy_end_call (proxy,
		call_id,
		&erreur,
		g_type_ptrarray, &pApplications,
		G_TYPE_INVALID);
	if (erreur != NULL)
	{
		g_print ("couldn't get applications in the systray (%s)\n", erreur->message);
		CD_APPLET_LEAVE ();
	}
	if (pApplications == NULL)
		CD_APPLET_LEAVE ();
	
	//\______________________ build each items.
	guint i, j;
	GValueArray *va;
	GValue *v;
	CDStatusNotifierItem *pItem;
	GList *pIcons = NULL;
	g_print ("%d apps in the systray\n", pApplications->len);
	for (i = 0; i < pApplications->len; i ++)
	{
		va = pApplications->pdata[i];
		if (! va)
			continue;
		
		const gchar *cIconName = NULL;
		gint iPosition = 0;
		const gchar *cAdress = NULL;
		const gchar *cObjectPath = NULL;
		const gchar *cIconThemePath = NULL;
		const gchar *cLabel = NULL;
		const gchar *cLabelGuide = NULL;
		
		v = g_value_array_get_nth (va, 0);
		if (v && G_VALUE_HOLDS_STRING (v))
			cIconName = g_value_get_string (v);
		
		v = g_value_array_get_nth (va, 1);
		if (v && G_VALUE_HOLDS_INT (v))
			iPosition = g_value_get_int (v);
		
		v = g_value_array_get_nth (va, 2);
		if (v && G_VALUE_HOLDS_STRING (v))
			cAdress = g_value_get_string (v);
		
		v = g_value_array_get_nth (va, 3);
		if (v && G_VALUE_HOLDS_BOXED (v))
			cObjectPath = (gchar*)g_value_get_boxed (v);
		
		v = g_value_array_get_nth (va, 4);
		if (v && G_VALUE_HOLDS_STRING (v))
			cIconThemePath = g_value_get_string (v);
		
		v = g_value_array_get_nth (va, 5);
		if (v && G_VALUE_HOLDS_STRING (v))
			cLabel = g_value_get_string (v);
		
		v = g_value_array_get_nth (va, 6);
		if (v && G_VALUE_HOLDS_STRING (v))
			cLabelGuide = g_value_get_string (v);
		
		g_print (" + item {%s ; %d ; %s ; %s ; %s ; %s ; %s}\n",
			cIconName,
			iPosition,
			cAdress,
			cObjectPath,
			cIconThemePath,
			cLabel,
			cLabelGuide);
		
		pItem = cd_satus_notifier_create_item (cAdress, cObjectPath);
		if (! pItem)
			continue;
		pItem->iPosition = iPosition;
		myData.pItems = g_list_prepend (myData.pItems, pItem);
		
		if (! myConfig.bCompactMode)
		{
			Icon *pIcon = cairo_dock_create_dummy_launcher (
				g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
				g_strdup (pItem->cIconName),
				g_strdup (pItem->cService),
				NULL,
				pItem->iCategory);
			pIcons = g_list_prepend (pIcons, pIcon);
		}
	}
	
	if (! myConfig.bCompactMode)
		CD_APPLET_LOAD_MY_ICONS_LIST (pIcons, NULL, "Slide", NULL);
	
	g_ptr_array_free (pApplications, TRUE);
	CD_APPLET_LEAVE ();
}

static void _cd_satus_notifier_get_indicator_application (void)
{
	g_print ("%s ()\n", __func__);
	// get the service
	myData.pProxyIndicatorApplicationService = cairo_dock_create_new_session_proxy (
		CD_INDICATOR_APPLICATION_ADDR,
		CD_INDICATOR_APPLICATION_OBJ,
		CD_INDICATOR_APPLICATION_IFACE);
	
	// connect to the signals.
	dbus_g_object_register_marshaller(_cd_cclosure_marshal_VOID__STRING_INT_STRING_STRING_STRING_STRING_STRING,
			G_TYPE_NONE, G_TYPE_STRING, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INVALID);
	dbus_g_proxy_add_signal(myData.pProxyIndicatorApplicationService, "ApplicationAdded",
		G_TYPE_STRING,  // iconname
		G_TYPE_INT,  // position
		G_TYPE_STRING,  // dbusaddress
		G_TYPE_STRING,  // dbusobject
		G_TYPE_STRING,  // iconpath
		G_TYPE_STRING,  // label
		G_TYPE_STRING,  // labelguide
		G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(myData.pProxyIndicatorApplicationService, "ApplicationAdded",
		G_CALLBACK(on_new_application), myApplet, NULL);
	
	dbus_g_proxy_add_signal(myData.pProxyIndicatorApplicationService, "ApplicationRemoved",
		G_TYPE_INT,  // position
		G_TYPE_INVALID);
	dbus_g_proxy_connect_signal(myData.pProxyIndicatorApplicationService, "ApplicationRemoved",
		G_CALLBACK(on_removed_application), myApplet, NULL);
	
	myData.pGetApplicationsCall = dbus_g_proxy_begin_call (myData.pProxyIndicatorApplicationService,
		"GetApplications",
		(DBusGProxyCallNotify)_on_get_applications_from_service,
		myApplet,
		(GDestroyNotify) NULL,
		G_TYPE_INVALID);
}

static void _on_get_applications_from_watcher (DBusGProxy *proxy, DBusGProxyCall *call_id, CairoDockModuleInstance *myApplet)
{
	g_print ("%s ()\n", __func__);
	CD_APPLET_ENTER;
	myData.pGetApplicationsCall = NULL;
	
	gchar **pApplications = NULL;
	GError *erreur = NULL;
	gboolean bSuccess = dbus_g_proxy_end_call (proxy,
		call_id,
		&erreur,
		G_TYPE_STRV, &pApplications,
		G_TYPE_INVALID);
	if (erreur != NULL)
	{
		cd_debug ("couldn't get applications from the watcher (%s)", erreur->message);
		bSuccess = FALSE;
	}
	
	if (bSuccess)
	{
		if (pApplications == NULL)
			CD_APPLET_LEAVE ();
		GList *pIcons = NULL;
		guint i;
		CDStatusNotifierItem *pItem;
		for (i = 0; pApplications[i] != NULL; i ++)
		{
			pItem = cd_satus_notifier_create_item (pApplications[i], NULL);
			myData.pItems = g_list_prepend (myData.pItems, pItem);
			
			if (! myConfig.bCompactMode)
			{
				Icon *pIcon = cairo_dock_create_dummy_launcher (
					g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
					g_strdup (pItem->cIconName),
					g_strdup (pItem->cService),
					NULL,
					pItem->iCategory);
				pIcons = g_list_prepend (pIcons, pIcon);
			}
		}
		
		g_strfreev (pApplications);
		
		if (myConfig.bCompactMode)
		{
			cairo_dock_load_icon_image (myIcon, myContainer);
		}
		else
		{
			CD_APPLET_LOAD_MY_ICONS_LIST (pIcons, NULL, "Slide", NULL);
		}
	}
	else  // un watcher asocial comme celui d'Ubuntu, on essaye avec l'"indicator-application".
	{
		g_print ("this watcher is not so friendly, let's try the 'application indicator'\n");
		_cd_satus_notifier_get_indicator_application ();
	}
	CD_APPLET_LEAVE ();
}

static void _on_register_host (DBusGProxy *proxy, DBusGProxyCall *call_id, CairoDockModuleInstance *myApplet)
{
	g_print ("%s ()\n", __func__);
	CD_APPLET_ENTER;
	GError *erreur = NULL;
	gboolean bSuccess = dbus_g_proxy_end_call (proxy,
		call_id,
		&erreur,
		G_TYPE_INVALID);
	if (erreur != NULL)
	{
		cd_debug ("couldn't register to a Notification Watcher (%s)", erreur->message);
		g_error_free (erreur);
		erreur = NULL;
		bSuccess = FALSE;
	}
	
	if (bSuccess)  // il y'a un watcher.
	{
		g_print ("found a friendly watcher, now ask for the items...\n");
		// get the items
		DBusGProxy *pProxyWatcherProps = cairo_dock_create_new_session_proxy (
			CD_STATUS_NOTIFIER_WATCHER_ADDR,
			CD_STATUS_NOTIFIER_WATCHER_OBJ,
			DBUS_INTERFACE_PROPERTIES);
		myData.pGetApplicationsCall = dbus_g_proxy_begin_call (pProxyWatcherProps,
			"Get",
			(DBusGProxyCallNotify)_on_get_applications_from_watcher,
			myApplet,
			(GDestroyNotify) NULL,
			G_TYPE_STRING, CD_STATUS_NOTIFIER_WATCHER_IFACE,
			G_TYPE_STRING, "RegisteredStatusNotifierItems",
			G_TYPE_INVALID);
		
		// connect to the signals.
		dbus_g_proxy_add_signal(myData.pProxyWatcher, "ServiceRegistered",
			G_TYPE_STRING, G_TYPE_INVALID);  // StatusNotifierItemRegistered
		dbus_g_proxy_connect_signal(myData.pProxyWatcher, "ServiceRegistered",
			G_CALLBACK(on_new_item), myApplet, NULL);
		dbus_g_proxy_add_signal(myData.pProxyWatcher, "ServiceUnregistered",
			G_TYPE_STRING, G_TYPE_INVALID);  // StatusNotifierItemUnregistered
		dbus_g_proxy_connect_signal(myData.pProxyWatcher, "ServiceUnregistered",
			G_CALLBACK(on_removed_item), myApplet, NULL);
	}
	else  // pas de watcher, ou un asocial comme celui d'Ubuntu, on essaye avec l'"indicator-application".
	{
		g_print ("no friendy watcher, let's try the 'application indicator'\n");
		_cd_satus_notifier_get_indicator_application ();
	}
	CD_APPLET_LEAVE ();
}
static void _cd_satus_notifier_register_host (void)
{
	// register to the watcher.
	g_print ("registering to the watcher...\n");
	myData.pRegisterHostCall = dbus_g_proxy_begin_call (myData.pProxyWatcher, "RegisterNotificationHost",
		(DBusGProxyCallNotify)_on_register_host,
		myApplet,
		(GDestroyNotify) NULL,
		G_TYPE_STRING, myData.cHostName,
		G_TYPE_INVALID);
}

static void _cd_satus_notifier_get_items (void)
{
	g_print ("%s ()\n", __func__);
	
	// get the items.
	DBusGProxy *pProxyWatcherProps = cairo_dock_create_new_session_proxy (
		CD_STATUS_NOTIFIER_WATCHER_ADDR,
		CD_STATUS_NOTIFIER_WATCHER_OBJ,
		"org.freedesktop.DBus.Properties");
	gchar **cItemsName = cairo_dock_dbus_get_property_as_string_list (pProxyWatcherProps, CD_STATUS_NOTIFIER_WATCHER_IFACE, "RegisteredStatusNotifierItems");
	g_object_unref (pProxyWatcherProps);
	
	// create all the icons.
	if (cItemsName != NULL)
	{
		GList *pIcons = NULL;
		CDStatusNotifierItem *pItem;
		int i;
		for (i = 0; cItemsName[i] != NULL; i ++)
		{
			pItem = cd_satus_notifier_create_item (cItemsName[i], NULL);
			myData.pItems = g_list_prepend (myData.pItems, pItem);
			
			if (! myConfig.bCompactMode)
			{
				Icon *pIcon = cairo_dock_create_dummy_launcher (g_strdup (pItem->cTitle?pItem->cTitle:pItem->cId),
					g_strdup (pItem->cIconName),
					g_strdup (pItem->cService),
					NULL,
					pItem->iCategory);
				pIcons = g_list_prepend (pIcons, pIcon);
			}
		}
		g_strfreev (cItemsName);
		
		if (! myConfig.bCompactMode)
			CD_APPLET_LOAD_MY_ICONS_LIST (pIcons, NULL, "Slide", NULL);
	}
	
	myIcon->iface.load_image = _load_my_icon_image;
}

static gboolean _get_watcher (CairoDockModuleInstance *myApplet)
{
	myData.pProxyWatcher = cairo_dock_create_new_session_proxy (
		CD_STATUS_NOTIFIER_WATCHER_ADDR,
		CD_STATUS_NOTIFIER_WATCHER_OBJ,
		CD_STATUS_NOTIFIER_WATCHER_IFACE);
	if (myData.pProxyWatcher != NULL)
	{
		myData.iSidGetWatcher = 0;
		_cd_satus_notifier_register_host ();
		return FALSE;
	}
	return TRUE;
}
void cd_satus_notifier_launch_service (void)
{
	// Register the service name on the bus.
	pid_t pid = getpid ();
	myData.cHostName = g_strdup_printf (CD_STATUS_NOTIFIER_HOST_ADDR"-%d", pid);
	g_print ("registering name '%s' on the bus ...\n", myData.cHostName);
	cairo_dock_register_service_name (myData.cHostName);
	
	// get the watcher.
	g_print ("getting the watcher ...\n");
	myData.pProxyWatcher = cairo_dock_create_new_session_proxy (
		CD_STATUS_NOTIFIER_WATCHER_ADDR,
		CD_STATUS_NOTIFIER_WATCHER_OBJ,
		CD_STATUS_NOTIFIER_WATCHER_IFACE);  /// dbus_g_proxy_new_for_name_owner ?...
	if (myData.pProxyWatcher == NULL)  // no watcher yet, let's try again in a few moment.
	{
		g_print ("no watcher yet, let's try again in a few moment\n");
		myData.iSidGetWatcher = g_timeout_add (1000., (GSourceFunc)_get_watcher, myApplet);
		return;
	}
	
	_cd_satus_notifier_register_host ();
}


void cd_satus_notifier_stop_service (void)
{
	if (myData.iSidGetWatcher != 0)
		g_source_remove (myData.iSidGetWatcher);
	
	g_object_unref (myData.pProxyWatcher);
	g_object_unref (myData.pProxyIndicatorApplicationService);
	
	g_list_foreach (myData.pItems, (GFunc) cd_free_item, NULL);
	g_list_free (myData.pItems);
	
	if (! myConfig.bCompactMode)
		CD_APPLET_DELETE_MY_ICONS_LIST;
}
