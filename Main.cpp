#include <iostream>
#include <ScreenSaver.h>

#include "CrashExcuseSaver.h"

extern "C" BScreenSaver*
instantiate_screen_saver(BMessage* message, image_id id)
{
	return new CrashExcuseSaver(message, id);
}

int main()
{
	std::cout << "This application cannot be run directly. Please place it in the \"~/config/non-packaged/add-ons/Screen Savers\" directory and run it from the \"Screen Savers\" application." << "\n";
}
