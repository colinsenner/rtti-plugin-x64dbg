#include "plugin.h"
#include "Lib\ini.h"
#include "Lib\config.h"
#include "Lib\Rtti.h"

#define RTTI_COMMAND "rtti"
#define RTTI_PLUGIN_VERSION "1.0"

// Checks the settings and auto-labels the enabled ones
bool AutoLabel(RTTI rtti)
{
	if (!rtti.IsValid())
		return false;

	if (settings.auto_label_vftable)
	{
		char sz_vftable_label[MAX_COMMENT_SIZE] = "";

		// If there isn't a label already there
		if (!DbgGetLabelAt(rtti.GetVftable(), SEG_DEFAULT, sz_vftable_label))
		{
			string label = rtti.name + "_vftable";
			if (!DbgSetLabelAt(rtti.GetVftable(), label.c_str()))
				return false;
		}
	}

	return true;
}

// Get the current window selection, aligns it to 4 byte boundaries and dumps it
void DumpRttiWindow(int hWindow)
{
	if (!DbgIsDebugging())
	{
		dputs("You need to be debugging to use this command");
		return;
	}

	SELECTIONDATA sel;
	GuiSelectionGet(hWindow, &sel);
	duint alignedStart = sel.start - (sel.start % (sizeof duint));

	char cmd[256] = { 0 };
	sprintf_s(cmd, "%s %zX", RTTI_COMMAND, alignedStart);
	
	// Run the cbRttiCommand
	DbgCmdExec(cmd);
}

// 'rtti <addr>' command
static bool cbRttiCommand(int argc, char* argv[])
{
	if (argc > 2)
	{
		dprintf("Usage: rtti <address>\n");
		return false;
	}

	// command 'rtti' - Assume the selected bytes from the dump window
	if (argc == 1)
	{
		DumpRttiWindow(GUI_DUMP);
	}
	// command 'rtti <address>'
	else if (argc == 2)
	{
		duint addr = 0;
		int numFieldsAssigned = sscanf_s(argv[1], "%zx", &addr);

		if (numFieldsAssigned != 1)
		{
			dprintf("Usage: rtti <address>\n");
			return false;
		}

		RTTI rtti(addr);
		
		if (rtti.IsValid())
		{
			AutoLabel(rtti);
			string rttiInfo = rtti.ToString();

			dprintf(rttiInfo.c_str());
		}
		else
			dprintf("No RTTI information found for address %p\n", addr);
	}
	
	return true;
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY* info)
{
    switch(info->hEntry)
    {
    case MENU_AUTO_LABEL_VFTABLE:
		settings.auto_label_vftable = !settings.auto_label_vftable;
		SaveConfig();
        break;

    case MENU_DUMP_RTTI:
		DumpRttiWindow(GUI_DUMP);
        break;

	case MENU_ABOUT:
		MessageBoxA(GuiGetWindowHandle(), "RTTI plugin version v" RTTI_PLUGIN_VERSION "\n\nhttps://gitlab.com/colinsenner/Rtti-plugin-x64", "About", 0);
		break;
    default:
        break;
    }
}

//Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT* initStruct)
{
	_plugin_registercommand(pluginHandle, RTTI_COMMAND, cbRttiCommand, true);
    return true; //Return false to cancel loading the plugin.
}

//Deinitialize your plugin data here.
void pluginStop()
{
	_plugin_unregistercommand(pluginHandle, RTTI_COMMAND);
}

//Do GUI/Menu related things here.
void pluginSetup()
{
	SetConfigPath();
	LoadConfig();

    int labelMenu = _plugin_menuadd(hMenu, "Auto-Label");
	_plugin_menuaddentry(labelMenu, MENU_AUTO_LABEL_VFTABLE, "vftable");
    _plugin_menuaddentry(hMenuDump, MENU_DUMP_RTTI, "&Dump Rtti");

	// About menu
	_plugin_menuaddentry(hMenu, MENU_ABOUT, "&About");

	// Update the checked status
	_plugin_menuentrysetchecked(pluginHandle, MENU_AUTO_LABEL_VFTABLE, settings.auto_label_vftable);
}
