#include "config.h"
#include "ini.h"
#include "..\pluginmain.h"

string config_path;
settings_t settings;

void SetConfigPath()
{
	char szCurrentDirectory[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
	strcat_s(szCurrentDirectory, "\\");

	config_path = szCurrentDirectory + string("Rtti.ini");
}

void LoadConfig()
{
	IniManager iniReader(config_path);
	settings.auto_label_vftable = iniReader.ReadBoolean("settings", "auto_label_vftable", true);
}

void SaveConfig() 
{
	IniManager iniWriter(config_path);
	iniWriter.WriteBoolean("settings", "auto_label_vftable", settings.auto_label_vftable);

	_plugin_menuentrysetchecked(pluginHandle, MENU_AUTO_LABEL_VFTABLE, settings.auto_label_vftable);
}