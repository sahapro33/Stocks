#if defined _stocks_sahapro33_included
#endinput
#endif

#define _stocks_sahapro33_included

#include <smlib/files>

// Defines
#if !defined DEBUG
#define DEBUG 		1
#endif

#define L_Info		0
#define L_Warning	1
#define L_Error		2
#define L_Debug		3


#if !defined PLUGIN_NAME_FOLDER
#define PLUGIN_NAME_FOLDER	"plugin_folder"
#endif
#if !defined TIME_FORMAT
#define TIME_FORMAT 		"%Y-%m-%d"
#endif

// Vars
stock char g_sCurrentMap[PLATFORM_MAX_PATH], g_sConfigMapPath[PLATFORM_MAX_PATH], 
/* */g_sConfigDefaultMapPath[PLATFORM_MAX_PATH], 
/* */g_sDownloadListMapPath[PLATFORM_MAX_PATH], 
/* */g_sDownloadListDefaultMapPath[PLATFORM_MAX_PATH], 
#if DEBUG == 1
/* */g_sDebugLogFile[PLATFORM_MAX_PATH], 
#endif
/* */g_sLogFile[PLATFORM_MAX_PATH];
stock KeyValues g_kvMapConfig, g_kvSettingsConfig;
bool is_load, is_load_debug;

/*
 * Logging
 */
stock void CreateDebugLogFolder()
{
	char path[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, path, sizeof(path), "logs/%s/debug", PLUGIN_NAME_FOLDER);
	if (!DirExists(path))
	{
		if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
		{
			LogToFileEx(g_sLogFile, "[ERROR] Error create directory \"%s\"", path);
			return;
		}
	}
	LoadDebugLogFile();
}
stock void LoadDebugLogFile()
{
	FormatTime(g_sDebugLogFile, sizeof(g_sDebugLogFile), TIME_FORMAT, GetTime());
	BuildPath(Path_SM, g_sDebugLogFile, sizeof(g_sDebugLogFile), "logs/%s/debug/log_%s.log", PLUGIN_NAME_FOLDER, g_sDebugLogFile);
}

stock void CreateLogFolder()
{
	char path[PLATFORM_MAX_PATH];
	BuildPath(Path_SM, path, sizeof(path), "logs/%s", PLUGIN_NAME_FOLDER);
	if (!DirExists(path))
	{
		if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
		{
			LogToFileEx(g_sLogFile, "[ERROR] Error create directory \"%s\"", path);
			return;
		}
	}
	LoadLogFile();
}
stock void LoadLogFile()
{
	FormatTime(g_sLogFile, sizeof(g_sLogFile), TIME_FORMAT, GetTime());
	BuildPath(Path_SM, g_sLogFile, sizeof(g_sLogFile), "logs/%s/log_%s.log", PLUGIN_NAME_FOLDER, g_sLogFile);
}

stock void Log(int type, const char[] message, any...)
{
	if (!is_load)
	{
		BuildPath(Path_SM, g_sLogFile, sizeof(g_sLogFile), "logs/%s.log", PLUGIN_NAME_FOLDER);
		CreateLogFolder();
		is_load = true;
	}
	
	int message_size = strlen(message) + 512;
	char[] _message = new char[message_size];
	VFormat(_message, message_size, message, 3);
	switch (type)
	{
		case L_Warning: { LogToFileEx(g_sLogFile, "[WARNING] %s", _message); }
		case L_Error: { LogToFileEx(g_sLogFile, "[ERROR] %s", _message); }
		case L_Debug:
		{
			#if DEBUG == 1
			if (!is_load_debug)
			{
				BuildPath(Path_SM, g_sDebugLogFile, sizeof(g_sDebugLogFile), "logs/%s_debug.log", PLUGIN_NAME_FOLDER);
				CreateDebugLogFolder();
				is_load_debug = true;
			}
			LogToFileEx(g_sDebugLogFile, "[DEBUG] %s", _message);
			#endif
		}
		default: { LogToFileEx(g_sLogFile, "%s", _message); }
	}
}

/*
 * Configs
 */
stock void LoadPluginSettings()
{
	char path[PLATFORM_MAX_PATH];
	if (g_kvSettingsConfig)
		delete g_kvSettingsConfig;
	
	g_kvSettingsConfig = new KeyValues("settings");
	BuildPath(Path_SM, path, sizeof(path), "configs/%s/settings.ini", PLUGIN_NAME_FOLDER);
	if (!g_kvSettingsConfig.ImportFromFile(path))
	{
		Log(L_Error, "Error load file '%s'", path);
		SetFailState("Error load file '%s'", path);
	}
}
stock void LoadMapSettings()
{
	GetCurrentMap(g_sCurrentMap, sizeof(g_sCurrentMap));
	
	BuildPath(Path_SM, g_sConfigDefaultMapPath, sizeof(g_sConfigDefaultMapPath), "configs/%s/maps/default.ini", PLUGIN_NAME_FOLDER);
	BuildPath(Path_SM, g_sConfigMapPath, sizeof(g_sConfigMapPath), "configs/%s/maps/%s.ini", PLUGIN_NAME_FOLDER, g_sCurrentMap);
	
	Log(L_Info, "Map start '%s'", g_sCurrentMap);
	
	if (FileExists(g_sConfigMapPath))
	{
		if (g_kvMapConfig != null)delete g_kvMapConfig;
		g_kvMapConfig = new KeyValues("settings");
		if (!g_kvMapConfig.ImportFromFile(g_sConfigMapPath))
		{
			Log(L_Error, "Config file \"%s\" error load", g_sConfigMapPath);
			SetFailState("[ERROR] Config file \"%s\" error load", g_sConfigMapPath);
			return;
		}
	}
	else
	{
		Log(L_Warning, "Map config file \"%s\" not found", g_sConfigMapPath);
		Log(L_Info, "Map config file \"%s\" creating", g_sConfigMapPath);
		
		CreateMapConfig();
	}
}
stock void LoadMapSettings()
{
	GetCurrentMap(g_sCurrentMap, sizeof(g_sCurrentMap));
	
	BuildPath(Path_SM, g_sDownloadListDefaultMapPath, sizeof(g_sDownloadListDefaultMapPath), "configs/%s/maps/default_dl.ini", PLUGIN_NAME_FOLDER);
	BuildPath(Path_SM, g_sDownloadListMapPath, sizeof(g_sDownloadListMapPath), "configs/%s/maps/%s_dl.ini", PLUGIN_NAME_FOLDER, g_sCurrentMap);
	
	if (!FileExists(g_sDownloadListMapPath))
	{
		Log(L_Warning, "Map DownloadList file \"%s\" error load", g_sDownloadListMapPath);
		Log(L_Warning, "Map config file \"%s\" not found", g_sConfigMapPath);
		Log(L_Info, "Map config file \"%s\" creating", g_sConfigMapPath);
		
		CreateMapDownloadList();
	}
	else File_ReadDownloadList(g_sDownloadListMapPath);
}
stock void CreateMapDownloadList()
{
	if (!FileExists(g_sDownloadListDefaultMapPath))
	{
		Log(L_Error, "Config file \"%s\" not found", g_sDownloadListDefaultMapPath);
		SetFailState("[ERROR] Config file \"%s\" not found", g_sDownloadListDefaultMapPath);
		return;
	}
	
	if (StrContains(g_sDownloadListMapPath, "workshop") != -1)
	{
		char path[PLATFORM_MAX_PATH];
		BuildPath(Path_SM, path, sizeof(path), "configs/%s/maps/workshop/", PLUGIN_NAME_FOLDER);
		if (!DirExists(path))
		{
			if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
			{
				Log(L_Error, "Error create directory \"%s\"", path);
				SetFailState("[ERROR] Error create directory \"%s\"", path);
				return;
			}
		}
		path[0] = '\0';
		for (int pos = 9; pos < sizeof(g_sCurrentMap); pos++)
		{
			if (!IsCharNumeric(g_sCurrentMap[pos]))
				continue;
			path[pos - 9] = g_sCurrentMap[pos];
			path[pos - 8] = '\0';
		}
		
		BuildPath(Path_SM, path, sizeof(path), "configs/%s/maps/workshop/%s/", PLUGIN_NAME_FOLDER, path);
		if (!DirExists(path))
		{
			if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
			{
				Log(L_Error, "Error directory \"%s\" create", path);
				SetFailState("[ERROR] Error directory \"%s\" create", path);
				return;
			}
		}
	}
	
	if (!File_Copy(g_sDownloadListDefaultMapPath, g_sDownloadListMapPath))
	{
		Log(L_Error, "Error create file \"%s\"", g_sDownloadListMapPath);
		SetFailState("[ERROR] Error create file \"%s\"", g_sDownloadListMapPath);
		return;
	}
	Log(L_Info, "Map DownloadList file \"%s\" successfully created", g_sDownloadListMapPath);
	
	ForceChangeLevel(g_sCurrentMap, "Restart");
}
stock void CreateMapConfig()
{
	if (!FileExists(g_sConfigDefaultMapPath))
	{
		Log(L_Error, "Config file \"%s\" not found", g_sConfigDefaultMapPath);
		SetFailState("[ERROR] Config file \"%s\" not found", g_sConfigDefaultMapPath);
		return;
	}
	
	if (StrContains(g_sConfigMapPath, "workshop") != -1)
	{
		char path[PLATFORM_MAX_PATH];
		BuildPath(Path_SM, path, sizeof(path), "configs/%s/maps/workshop/", PLUGIN_NAME_FOLDER);
		if (!DirExists(path))
		{
			if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
			{
				Log(L_Error, "Error create directory \"%s\"", path);
				SetFailState("[ERROR] Error create directory \"%s\"", path);
				return;
			}
		}
		path[0] = '\0';
		for (int pos = 9; pos < sizeof(g_sCurrentMap); pos++)
		{
			if (!IsCharNumeric(g_sCurrentMap[pos]))
				continue;
			path[pos - 9] = g_sCurrentMap[pos];
			path[pos - 8] = '\0';
		}
		
		BuildPath(Path_SM, path, sizeof(path), "configs/%s/maps/workshop/%s/", PLUGIN_NAME_FOLDER, path);
		if (!DirExists(path))
		{
			if (!CreateDirectory(path, FPERM_U_READ + FPERM_U_WRITE + FPERM_U_EXEC + FPERM_G_READ + FPERM_G_WRITE + FPERM_G_EXEC + FPERM_O_READ + FPERM_O_WRITE + FPERM_O_EXEC))
			{
				Log(L_Error, "Error directory \"%s\" create", path);
				SetFailState("[ERROR] Error directory \"%s\" create", path);
				return;
			}
		}
	}
	
	if (!File_Copy(g_sConfigDefaultMapPath, g_sConfigMapPath)) { Log(L_Error, "Error create file \"%s\"", g_sConfigMapPath); SetFailState("[ERROR] Error create file \"%s\"", g_sConfigMapPath); return; }
	Log(L_Info, "Map config file \"%s\" successfully created", g_sConfigMapPath);
	
	ForceChangeLevel(g_sCurrentMap, "Restart");
} 