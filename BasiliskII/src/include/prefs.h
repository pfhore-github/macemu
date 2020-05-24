/*
 *  prefs.h - Preferences handling
 *
 *  Basilisk II (C) 1997-2008 Christian Bauer
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef PREFS_H
#define PREFS_H

#include <stdio.h>
#include <string>
#include <vector>
extern void PrefsInit(const char *vmdir, int &argc, char **&argv);
extern void PrefsExit(void);

extern void PrefsPrintUsage(void);

extern void AddPrefsDefaults(void);
extern void AddPlatformPrefsDefaults(void);

// Preferences loading/saving
extern void LoadPrefs(const char *vmdir);
extern void SavePrefs(void);

extern void LoadPrefsFromStream(FILE *f);
extern void SavePrefsToStream(FILE *f);

// Public preferences access functions
extern void PrefsAddString(const std::string& name, const char *s);
extern void PrefsAddBool(const std::string& name, bool b);
extern void PrefsAddInt32(const std::string& name, int32_t val);

extern void PrefsReplaceString(const std::string&name,
							   const std::vector<std::string>&ss);
extern void PrefsReplaceBool(const std::string&name, bool b);
extern void PrefsReplaceInt32(const std::string&name, int32_t val);

extern std::vector<std::string> PrefsFindStrings(const std::string& name);
extern std::string PrefsFindString(const std::string& name);
extern bool PrefsFindBool(const std::string& name);
extern int32_t PrefsFindInt32(const std::string& name);

extern void PrefsRemoveItem(const std::string& name, int index = 0);

#ifdef SHEEPSHAVER
// Platform specific functions:
extern void prefs_init();
extern void prefs_exit();
#endif

/*
 *  Definition of preferences items
 */
enum prefs_type {
	TYPE_STRING,
	TYPE_BOOLEAN,
	TYPE_INT32,
};
#include <unordered_map>
// Item descriptor
struct prefs_desc {
	prefs_type type;	// Type (see above)
	bool multiple;		// Can this item occur multiple times (only for TYPE_STRING)?
	const char *help;	// Help/descriptive text about this item
};

// List of common preferences items (those which exist on all platforms)
extern std::unordered_map<std::string, prefs_desc> common_prefs_items;

// List of platform-specific preferences items
extern std::unordered_map<std::string, prefs_desc> platform_prefs_items;

#endif
