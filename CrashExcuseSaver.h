#ifndef CRASHEXCUSESAVER_H
#define CRASHEXCUSESAVER_H
#include <ScreenSaver.h>
#include <StringList.h>

class CrashExcuseSaver : public BScreenSaver
{
	public:
		CrashExcuseSaver(BMessage* message, image_id id);
		~CrashExcuseSaver();
		status_t StartSaver(BView* screenView, bool isPreview);
		virtual void Draw(BView* screenView, int32 frame);
	private:
		BStringList* fErrors;
		BString fErrorMessage;
		BString fErrorCode;
};

#endif
