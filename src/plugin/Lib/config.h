#pragma once
#include <string>

struct settings_t
{
    bool auto_label_vftable;
    bool show_rtti_comments;
};

// Load the configuration file with the settings of the plugin
void LoadConfig();

// Save the configuration file for the settings of the plugin
void SaveConfig();

// Set the config path to the current directory
void SetConfigPath();

extern std::string config_path;
extern settings_t settings;
