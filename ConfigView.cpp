#include <ListView.h>
#include <ScrollView.h>
#include <TextControl.h>
#include <Font.h>
#include <Button.h>
#include <Alert.h>
#include <Entry.h>
#include <File.h>
#include <cstdio>

#include "ConfigView.h"

ConfigView::ConfigView(BRect configRect, BStringList* searchPathStrings)
	:	BView(configRect, "configView", B_FOLLOW_ALL, B_WILL_DRAW)
{
	fSearchPathStrings = searchPathStrings;
	fConfigRect = configRect;
}

void ConfigView::AttachedToWindow()
{
	AdoptParentColors();

	fConfigRect.InsetBy(10, 10);
	fPathsList = new BListView(BRect(fConfigRect.left, fConfigRect.top, fConfigRect.right, fConfigRect.bottom - fConfigRect.Height() / 5), "searchListView", B_SINGLE_SELECTION_LIST);

	for (int pathNo = 0; pathNo < fSearchPathStrings->CountStrings(); pathNo++)
	{
		BStringItem* item = new BStringItem(fSearchPathStrings->StringAt(pathNo));
		fPathsList->AddItem(item);
	}

	AddChild(new BScrollView("scrollView", fPathsList, B_FOLLOW_LEFT | B_FOLLOW_TOP, 0, true, true));

	fConfigRect = fPathsList->Bounds();
	BRect textRect = BRect(fConfigRect.left, fConfigRect.bottom + fConfigRect.Height() / 10.0f, fConfigRect.right, fConfigRect.bottom + fConfigRect.Height() / 2.5f);
	fNewPath = new BTextControl(textRect, "textView", "Path: ", "Insert Path here...", NULL);
	AddChild(fNewPath);

	AddChild(new BButton(BRect(fConfigRect.left, fConfigRect.bottom + fConfigRect.Height() / 5.0f, fConfigRect.right, fConfigRect.bottom + fConfigRect.Height() / 4.5f), "addPathButton", "Add Path", new BMessage(kAddPathMessage)));
}

ConfigView::~ConfigView()
{
	delete fPathsList;
}

void ConfigView::MessageReceived(BMessage* message)
{
	printf("Message %d!\n", message->what == kAddPathMessage);

	switch(message->what)
	{
		case kAddPathMessage:
		{
			BEntry errorsEntry = BEntry(fNewPath->Text());
			entry_ref ref;
			errorsEntry.GetRef(&ref);
			BFile errorsFile = BFile(&ref, B_READ_ONLY);

			if (errorsFile.InitCheck() != B_OK)
			{
				printf("Invalid!\n");
				BAlert* alert = new BAlert("Error", "Path is invalid.", "Ok");
				alert->Go();
				delete alert;
				break;
			}

			fSearchPathStrings->Add(fNewPath->Text());
			BStringItem* item = new BStringItem(fNewPath->Text());
			fPathsList->AddItem(item);
			break;
		}

		case B_QUIT_REQUESTED:
		{
			BMessage archive = BMessage('SvrA');

			archive.AddInt32("pathCount", fSearchPathStrings->CountStrings());
			for (int pathNo = 0; pathNo < fSearchPathStrings->CountStrings(); pathNo++)
			{
				BString nameString = "Path";
				nameString << pathNo;
				archive.AddString(nameString.String(), fSearchPathStrings->StringAt(pathNo));
			}

			BFile file = BFile("~/config/settings/CrashExcuseSaver_settings", B_READ_WRITE);
			archive.Flatten(&file);
		}

		default:
			BView::MessageReceived(message);
			break;
	}
}
