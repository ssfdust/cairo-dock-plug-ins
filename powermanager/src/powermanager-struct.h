#ifndef __POWERMANAGER_STRUCT__
#define  __POWERMANAGER_STRUCT__

#include <cairo-dock.h>


typedef enum {
	POWER_MANAGER_NOTHING = 0,
	POWER_MANAGER_CHARGE,
	POWER_MANAGER_TIME,
	POWER_MANAGER_NB_QUICK_INFO_TYPE
  } MyAppletQuickInfoType;

typedef enum {
	POWER_MANAGER_EFFECT_NONE = 0,
	POWER_MANAGER_EFFECT_ZOOM,
	POWER_MANAGER_EFFECT_TRANSPARENCY,
	POWER_MANAGER_EFFECT_BAR,
	} MyAppletEffect;

typedef enum {
	POWER_MANAGER_CHARGE_CRITICAL = 0,
	POWER_MANAGER_CHARGE_LOW,
	POWER_MANAGER_CHARGE_FULL,
	POWER_MANAGER_NB_CHARGE_LEVEL,
	} MyAppletCharge;

struct _AppletConfig {
	gchar *defaultTitle;
	MyAppletQuickInfoType quickInfoType;
	gint iCheckInterval;
	
	gboolean batteryWitness;
	gboolean highBatteryWitness;
	gboolean lowBatteryWitness;
	gboolean criticalBatteryWitness;
	gboolean bUseApprox;
	gchar *batteryWitnessAnimation;
	gint lowBatteryValue;
	const gchar *cGThemePath;
	gchar *cSoundPath[POWER_MANAGER_NB_CHARGE_LEVEL];
	gboolean bUseDBusFallback;
	
	gboolean bUseGauge;
	gchar *cUserBatteryIconName;
	gchar *cUserChargeIconName;
	MyAppletEffect iEffect;
  } ;

#define PM_NB_VALUES 100
struct _AppletData {
	cairo_surface_t *pSurfaceBattery;
	cairo_surface_t *pSurfaceCharge;
	gboolean dbus_enable;
	gboolean battery_present;
	gint iCapacity;
	gboolean on_battery, previously_on_battery;
	gdouble battery_time, previous_battery_time;
	gdouble battery_charge, previous_battery_charge;
	gboolean alerted;
	gboolean bCritical;
	gint checkLoop;
	Gauge *pGauge;
	gchar *cBatteryStateFilePath;
	
	gdouble fRateHistory[PM_NB_VALUES];
	gint iCurrentIndex;
	gint iIndexMax;
	gboolean bUseDBus;
	} ;


#endif
