#include "SDL.h"
/*
 *  Display alert
 */



/*
 *  Display error alert
 */

void ErrorAlert(const char *text)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "ERROR", text, nullptr);
}


/*
 *  Display warning alert
 */

void WarningAlert(const char *text)
{
	SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_WARNING, "WARNING", text, nullptr);
}


/*
 *  Display choice alert
 */

bool ChoiceAlert(const char *text, const char *pos, const char *neg)
{
	SDL_MessageBoxButtonData btn[] = {
		{ SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, pos},
		{ SDL_MESSAGEBOX_BUTTON_ESCAPEKEY_DEFAULT, 0, neg},
	} ;
	SDL_MessageBoxData data = {
		SDL_MESSAGEBOX_INFORMATION,
		nullptr,
		"CHOICE",
		text,
		2, 
		btn,
		nullptr
	};
	int ret;
	SDL_ShowMessageBox(&data, &ret);
	return ret;
}
