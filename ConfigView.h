#ifndef CONFIGVIEW_H
#define CONFIGVIEW_H
#include <View.h>
#include <StringList.h>
#include <ListView.h>
#include <SupportDefs.h>

const int32 kAddPathMessage = 'aPth';

class ConfigView : public BView
{
	public:
		ConfigView(BRect configRect, BStringList* searchPathStrings);
		~ConfigView();
		virtual void MessageReceived(BMessage* message);
		virtual void AttachedToWindow();
	private:
		BStringList* fSearchPathStrings;
		BListView* fPathsList;
		BTextControl* fNewPath;
		BRect fConfigRect;
};

#endif
