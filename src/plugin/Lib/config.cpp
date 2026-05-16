#include "config.h"
#include "..\pluginmain.h"
#include "ini.h"

std::string config_path;
settings_t settings;

void SetConfigPath()
{
    char szCurrentDirectory[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, szCurrentDirectory);
    strcat_s(szCurrentDirectory, "\\");

    config_path = szCurrentDirectory + std::string("Rtti.ini");
}

void LoadConfig()
{
    IniManager iniReader(config_path);
    settings.auto_label_vftable = iniReader.ReadBoolean("settings", "auto_label_vftable", true);
    settings.show_rtti_comments = iniReader.ReadBoolean("settings", "show_rtti_comments", true);
}

void SaveConfig()
{
    IniManager iniWriter(config_path);
    iniWriter.WriteBoolean("settings", "auto_label_vftable", settings.auto_label_vftable);
    iniWriter.WriteBoolean("settings", "show_rtti_comments", settings.show_rtti_comments);

    _plugin_menuentrysetchecked(pluginHandle, MENU_AUTO_LABEL_VFTABLE, settings.auto_label_vftable);
    _plugin_menuentrysetchecked(pluginHandle, MENU_SHOW_RTTI_COMMENTS, settings.show_rtti_comments);
}