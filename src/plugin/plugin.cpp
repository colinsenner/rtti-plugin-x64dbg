#include "plugin.h"
#include "Lib\Rtti.h"
#include "Lib\config.h"
#include "Lib\ini.h"
#include <unordered_map>

#define RTTI_COMMAND "rtti"
#define RTTI_PLUGIN_VERSION "1.0.6"

// addr → demangled class name; empty string means "checked, not a C++ object"
// static std::unordered_map<duint, std::string> s_rttiCache;

// Checks the settings and auto-labels the enabled ones
bool AutoLabel(RTTI rtti)
{
    if (!rtti.IsValid())
    {
        return false;
    }

    if (settings.auto_label_vftable)
    {
        char sz_vftable_label[MAX_COMMENT_SIZE] = "";

        // If there isn't a label already there
        if (!DbgGetLabelAt(rtti.GetVftable(), SEG_DEFAULT, sz_vftable_label))
        {
            string label = rtti.name + "_vftable";
            if (!DbgSetLabelAt(rtti.GetVftable(), label.c_str()))
            {
                return false;
            }
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

    char cmd[256] = {0};
    sprintf_s(cmd, "%s %zX", RTTI_COMMAND, alignedStart);

    // Run the cbRttiCommand
    DbgCmdExec(cmd);
}

// 'rtti <addr>' command
static bool cbRttiCommand(int argc, char *argv[])
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

            dprintf("%s\n", rttiInfo.c_str());
        }
        else
        {
            dprintf("No RTTI information found for address %p\n", addr);
        }
    }

    return true;
}

PLUG_EXPORT void CBADDRINFO(CBTYPE, void *callbackInfo)
{
    auto *info = reinterpret_cast<PLUG_CB_ADDRINFO *>(callbackInfo);

    if (!info || !info->addrinfo)
    {
        return;
    }

    if (!settings.show_rtti_comments)
    {
        return;
    }

    if (!DbgIsDebugging())
    {
        return;
    }

    if (!(info->addrinfo->flags & (flaglabel | flagcomment)))
    {
        return;
    }

    duint addr = info->addr;

    string className = RTTI::GetNameOnly(addr);
    if (!className.empty())
    {
        if (info->addrinfo->flags & flaglabel)
        {
            strncpy_s(info->addrinfo->label, MAX_LABEL_SIZE, className.c_str(), _TRUNCATE);
        }
        if (info->addrinfo->flags & flagcomment)
        {
            strncpy_s(info->addrinfo->comment, MAX_COMMENT_SIZE, className.c_str(), _TRUNCATE);
        }
        info->retval = true;
    }
}

PLUG_EXPORT void CBMENUENTRY(CBTYPE cbType, PLUG_CB_MENUENTRY *info)
{
    switch (info->hEntry)
    {
    case MENU_AUTO_LABEL_VFTABLE:
        settings.auto_label_vftable = !settings.auto_label_vftable;
        SaveConfig();
        break;

    case MENU_DUMP_RTTI:
        DumpRttiWindow(GUI_DUMP);
        break;

    case MENU_ABOUT:
        MessageBoxA(GuiGetWindowHandle(),
                    "RTTI plugin version v" RTTI_PLUGIN_VERSION "\n\nhttps://github.com/colinsenner/rtti-plugin-x64dbg",
                    "About", 0);
        break;

    case MENU_SHOW_RTTI_COMMENTS:
        settings.show_rtti_comments = !settings.show_rtti_comments;
        SaveConfig();
        break;

    default:
        break;
    }
}

// Initialize your plugin data here.
bool pluginInit(PLUG_INITSTRUCT *initStruct)
{
    _plugin_registercommand(pluginHandle, RTTI_COMMAND, cbRttiCommand, true);
    _plugin_registercallback(pluginHandle, CB_ADDRINFO, CBADDRINFO);
    return true;
}

void pluginStop()
{
    _plugin_unregistercommand(pluginHandle, RTTI_COMMAND);
    _plugin_unregistercallback(pluginHandle, CB_ADDRINFO);
}

// Do GUI/Menu related things here.
void pluginSetup()
{
    SetConfigPath();
    LoadConfig();

    int labelMenu = _plugin_menuadd(hMenu, "Auto-Label");
    _plugin_menuaddentry(labelMenu, MENU_AUTO_LABEL_VFTABLE, "vftable");
    _plugin_menuaddentry(hMenuDump, MENU_DUMP_RTTI, "&Dump Rtti");

    _plugin_menuaddentry(hMenu, MENU_SHOW_RTTI_COMMENTS, "Show &RTTI in registers");

    // About menu
    _plugin_menuaddentry(hMenu, MENU_ABOUT, "&About");

    // Update checked status
    _plugin_menuentrysetchecked(pluginHandle, MENU_AUTO_LABEL_VFTABLE, settings.auto_label_vftable);
    _plugin_menuentrysetchecked(pluginHandle, MENU_SHOW_RTTI_COMMENTS, settings.show_rtti_comments);
}
