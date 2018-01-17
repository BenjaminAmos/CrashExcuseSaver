#ifndef CRASHEXCUSESAVER_H
#define CRASHEXCUSESAVER_H
#include <ScreenSaver.h>
#include <StringList.h>
#include <TextControl.h>

class CrashExcuseSaver : public BScreenSaver
{
	public:
		CrashExcuseSaver(BMessage* message, image_id id);
		~CrashExcuseSaver();
		virtual status_t StartSaver(BView* screenView, bool isPreview);
		virtual void StartConfig(BView* configView);
		virtual void Draw(BView* screenView, int32 frame);
	private:
		BStringList* fErrors;
		BString fErrorMessage;
		BString fErrorCode;
		BStringList* fSearchPaths;
};

#endif
