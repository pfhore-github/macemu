/*
 *  prefs.cpp - Preferences handling
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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

#include "sysdeps.h"
#include "sys.h"
#include "prefs.h"

#include <vector>
#include <unordered_map>
#include <variant>
#include <string>
typedef std::unordered_map<std::string,
						   std::variant<std::vector<std::string>,
										int,
										bool>
						   > pref_t;
static pref_t the_prefs;

static auto find_prefs_desc(const std::string& keyword) {
	auto d = common_prefs_items.find(keyword);
	if (d != common_prefs_items.end()) {
		return d;
	}
	return platform_prefs_items.find(keyword);
}

/*
 *  Initialize preferences
 */

void PrefsInit(const char *vmdir, int &argc, char **&argv)
{
	// Set defaults
	AddPrefsDefaults();
	AddPlatformPrefsDefaults();

	// Load preferences from settings file
	LoadPrefs(vmdir);

	// Override prefs with command line options
	for (int i=1; i<argc; i++) {

		// Options are of the form '--keyword'
		const char *option = argv[i];
		if (!option || strlen(option) < 3 || option[0] != '-' || option[1] != '-')
			continue;
		const char *keyword = option + 2;

		// Find descriptor for keyword
		auto d = find_prefs_desc(keyword);
		if( d == platform_prefs_items.end() ) {
			continue;
		}
		
		argv[i] = NULL;

		// Get value
		i++;
		if (i >= argc) {
			fprintf(stderr, "Option '%s' must be followed by a value\n", option);
			continue;
		}
		const char *value = argv[i];
		argv[i] = NULL;

		// Add/replace prefs item
		switch (d->second.type) {
		case TYPE_STRING:
			if( d->second.multiple ) {
				std::get<0>(the_prefs[keyword]).push_back(value);
			} else {
				the_prefs[keyword] = { value };
			}
			break;
		case TYPE_INT32:
			the_prefs[keyword] = atoi(value);
			break;
		case TYPE_BOOLEAN: 
			if (!strcmp(value, "true") || !strcmp(value, "on") || !strcmp(value, "yes")) {
				the_prefs[keyword] = true;
			} else if (!strcmp(value, "false") || !strcmp(value, "off") || !strcmp(value, "no")) {
				the_prefs[keyword] = false;
			} else
				fprintf(stderr, "Value for option '%s' must be 'true' or 'false'\n", option);
			break;
		default:
			break;
		}
	}
	
	// Remove processed arguments
	for (int i=1; i<argc; i++) {
		int k;
		for (k=i; k<argc; k++)
			if (argv[k] != NULL)
				break;
		if (k > i) {
			k -= i;
			for (int j=i+k; j<argc; j++)
				argv[j-k] = argv[j];
			argc -= k;
		}
	}

#ifdef SHEEPSHAVER
	// System specific initialization
	prefs_init();
#endif
}


/*
 *  Deinitialize preferences
 */

void PrefsExit(void)
{
#ifdef SHEEPSHAVER
	// System specific deinitialization
	prefs_exit();
#endif

	// Free prefs list
	the_prefs.clear();
}


/*
 *  Print preferences options help
 */

static void print_options(const std::unordered_map<std::string, prefs_desc>& list) 
{
	for( auto & i : list ) {
		if (i.second.help) {
			std::string typestr, defstr;
			char numstr[32];
			switch (i.second.type) {
			case TYPE_STRING: {
					typestr = "STRING";
					auto v = PrefsFindString(i.first);
					if (v.empty())
						defstr = "none";
					else
						defstr = v[0];
					break;
			}
				case TYPE_BOOLEAN:
					typestr = "BOOL";
					if (PrefsFindBool(i.first))
						defstr = "true";
					else
						defstr = "false";
					break;
				case TYPE_INT32:
					typestr = "NUMBER";
					sprintf(numstr, "%d", PrefsFindInt32(i.first));
					defstr = numstr;
					break;
				default:
					typestr = "<unknown>";
					defstr = "none";
					break;
			}
			printf("  --%s %s\n    %s [default=%s]\n",
				   i.first.c_str(), typestr.c_str(), i.second.help, defstr.c_str());
		}
	}
}

void PrefsPrintUsage(void)
{
	printf("\nGeneral options:\n");
	print_options(common_prefs_items);
	printf("\nPlatform-specific options:\n");
	print_options(platform_prefs_items);
	printf("\nBoolean options are specified as '--OPTION true|on|yes' or\n'--OPTION false|off|no'.\n");
}


void PrefsReplaceString(const std::string& name,
						const std::vector<std::string>& ss)
{
	the_prefs[name] = ss;
	return;
}

void PrefsReplaceBool(const std::string& name, bool b)
{
	the_prefs[name] = b;
}

void PrefsReplaceInt32(const std::string& name, int32 val)
{
	the_prefs[name] = val;
}


/*
 *  Get prefs items
 */

std::vector<std::string> PrefsFindStrings(const std::string& name)
{
	auto d = the_prefs.find(name);
	if( d != the_prefs.end() ) {
		if( auto d2 = std::get_if<0>( &d->second) ) {
			return (*d2);
		}
	}
	return {};
}

std::string PrefsFindString(const std::string& name)
{
	auto d = the_prefs.find(name);
	if( d != the_prefs.end() ) {
		if( auto d2 = std::get_if<0>( &d->second) ) {
			if( d2->size() == 1 ) {
				return (*d2)[0];
			}
		}
	}
	return std::string();
}

bool PrefsFindBool(const std::string& name)
{
	auto d = the_prefs.find(name);
	if( d != the_prefs.end() ) {
		if( auto d2 = std::get_if<2>( &d->second) ) {
			return *d2;
		}
	}
	return false;
}

int32 PrefsFindInt32(const std::string& name)
{
	auto d = the_prefs.find(name);
	if( d != the_prefs.end() ) {
		if( auto d2 = std::get_if<1>( &d->second) ) {
			return *d2;
		}
	}
	return 0;
}


/*
 *  Remove prefs items
 */

void PrefsRemoveItem(const std::string& name, int index)
{
	auto d = the_prefs.find(name);
	if( d != the_prefs.end() ) {
		if( auto d2 = std::get_if<0>( &d->second) ) {
			if( d2->size() == 1 ) {
				the_prefs.erase(d);				
			} else if( (uint32_t)index < d2->size() ) {
				d2->erase(d2->begin() + index);
			}
		} else {
			the_prefs.erase(d);
		}
	}
}


/*
 *  Load prefs from stream (utility function for LoadPrefs() implementation)
 */

void LoadPrefsFromStream(FILE *f)
{
	char line[256];
	while(fgets(line, sizeof(line), f)) {
		// Remove newline, if present
		int len = strlen(line);
		if (len > 0 && line[len-1] == '\n') {
			line[len-1] = '\0';
			len--;
		}
		if (len == 0)
			continue;

		// Comments begin with "#" or ";"
		if (line[0] == '#' || line[0] == ';')
			continue;

		// Terminate string after keyword
		char *p = line;
		while (*p && !isspace(*p)) p++;
		if (*p != '\0')
			*p++ = 0;

		// Skip whitespace until value
		while (*p && isspace(*p)) p++;
		char *keyword = line;
		char *value = p;

		// Look for keyword first in prefs item list
		auto desc = find_prefs_desc(keyword);
		if (desc == platform_prefs_items.end()) {
			printf("WARNING: Unknown preferences keyword '%s'\n", keyword);
			continue;
		}

		// Add item to prefs
		switch (desc->second.type) {
			case TYPE_STRING:
				if (desc->second.multiple)
					std::get<0>(the_prefs[keyword]).push_back(value);
				else
					the_prefs[keyword] = std::vector<std::string>({ value });
				break;
			case TYPE_BOOLEAN:
				the_prefs[keyword] = !strcmp(value, "true");
				break;
			case TYPE_INT32:
				the_prefs[keyword] = atoi(value);
				break;
			default:
				break;
		}
	}
}


/*
 *  Save settings to stream (utility function for SavePrefs() implementation)
 */

static void write_prefs(FILE *f,
						const std::unordered_map<std::string, prefs_desc>&list)
{
	for( auto i : list ) {
		const char* key = i.first.c_str();
		switch (i.second.type) {
			case TYPE_STRING: {
				for( const auto & str : PrefsFindStrings(i.first) ) {
					fprintf(f, "%s %s\n", key, str.c_str());
				}
				break;
			}
			case TYPE_BOOLEAN:
				fprintf(f, "%s %s\n", key, PrefsFindBool(i.first) ? "true" : "false");
				break;
			case TYPE_INT32:
				fprintf(f, "%s %d\n", key, PrefsFindInt32(i.first));
				break;
			default:
				break;
		}
	}
}

void SavePrefsToStream(FILE *f)
{
	write_prefs(f, common_prefs_items);
	write_prefs(f, platform_prefs_items);
}
