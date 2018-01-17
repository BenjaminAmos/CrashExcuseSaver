#include <ScreenSaver.h>
#include <String.h>
#include <Entry.h>
#include <Path.h>
#include <File.h>
#include <FindDirectory.h>
#include <ListView.h>
#include <StringItem.h>
#include <ScrollView.h>
#include <StringView.h>
#include <cstdlib>
#include <ctime>

#include "CrashExcuseSaver.h"
#include "ConfigView.h"

bool IsInteger(const char integerCharacter)
{
	switch(integerCharacter)
	{
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			return true;
		default:
			return false;
	}
}

void SetFontScale(BView* view, int scale)
{
	BFont font;
	view->GetFont(&font);
	font.SetSize(view->Bounds().Height() / scale);
	view->SetFont(&font);
}

const char GetHexSymbol(int32 integer)
{
	switch(integer)
	{
		case 10:
			return 'A';
		case 11:
			return 'B';
		case 12:
			return 'C';
		case 13:
			return 'D';
		case 14:
			return 'E';
		case 15:
			return 'F';
		default:
			return integer;
	}
}

CrashExcuseSaver::CrashExcuseSaver(BMessage* message, image_id id)
	: BScreenSaver(message, id)
{
	fErrors = new BStringList();
	fSearchPaths = new BStringList();

	srand(time(NULL));

	BMessage archive = BMessage('SvrA');
	BFile file = BFile("~/config/settings/CrashExcuseSaver_settings", B_READ_ONLY);
	if (file.InitCheck())
	{
		archive.Unflatten(&file);

		int32 pathCount;
		archive.FindInt32("pathCount", &pathCount);
		for (int pathNo = 0; pathNo < pathCount; pathNo++)
		{
			BString nameString = "Path";
			nameString << pathNo;
			BString path;
			archive.FindString(nameString.String(), &path);
			fSearchPaths->Add(path);
		}
	}

	if (fErrors->CountStrings() == 0)
	{
		BPath fortunesPath;
		find_directory(B_SYSTEM_DATA_DIRECTORY, &fortunesPath);
		fortunesPath.Append("fortunes");
		fortunesPath.Append("Tech Support Excuses");
		fSearchPaths->Add(fortunesPath.Path());
	}

	for (int fileNo = 0; fileNo < fSearchPaths->CountStrings(); fileNo++)
	{
		BEntry errorEntry = BEntry(fSearchPaths->StringAt(fileNo));
		entry_ref ref;
		errorEntry.GetRef(&ref);
		BFile errorsFile = BFile(&ref, B_READ_ONLY);

		if (errorsFile.InitCheck() != B_OK)
		{
			return;
		}

		off_t fileSize = 0;
		errorsFile.GetSize(&fileSize);

		BString fileData;
		char* dataBuffer = fileData.LockBuffer(fileSize + 10);
		errorsFile.Read(dataBuffer, fileSize);
		fileData.UnlockBuffer();

		fileData.Split("%\n", true, *fErrors);
	}
}

CrashExcuseSaver::~CrashExcuseSaver()
{
	delete fErrors;
}

status_t
CrashExcuseSaver::StartSaver(BView* screenView, bool isPreview)
{
	int32 randomErrorIndex = rand() % fErrors->CountStrings();
	BString error = fErrors->StringAt(randomErrorIndex).Trim();

	BStringList splitList;
	error.Split("#", true, splitList);
	BString rawErrorNoString = splitList.StringAt(1).String();
	BString errorNoString;
	int index = 0;
	for (; IsInteger(rawErrorNoString[index]); index++)
	{
		errorNoString << rawErrorNoString[index];
	}

	fErrorCode.SetToFormat("Code: 0x%X", atoi(errorNoString));

	error.CopyCharsInto(fErrorMessage, error.FindFirst("#") + index + 2, error.Length());
	fErrorMessage.Prepend("Message: ");

	SetTickSize(5000000);
	return B_OK;
}

void CrashExcuseSaver::StartConfig(BView* configView)
{
	ConfigView* view = new ConfigView(configView->Bounds(), fSearchPaths);
	configView->AddChild(view);
}

void
CrashExcuseSaver::Draw(BView* screenView, int32 frame)
{
	screenView->SetLowColor(0, 0, 100);
	screenView->FillRect(screenView->Bounds(), B_SOLID_LOW);

	screenView->SetLowColor(64, 128, 128);
	BRect messageBackgroundRect = BRect(screenView->Bounds().left + (screenView->Bounds().Width() / 10), screenView->Bounds().top + (screenView->Bounds().Height() / 10), screenView->Bounds().right - (screenView->Bounds().Width() / 10), screenView->Bounds().bottom - (screenView->Bounds().Height() / 10));
	screenView->FillRect(messageBackgroundRect, B_SOLID_LOW);

	screenView->SetHighColor(255, 255, 255);

	SetFontScale(screenView, 8);
	screenView->DrawString("ERROR!", messageBackgroundRect.LeftTop() + BPoint(screenView->Bounds().Width() / 4, screenView->Bounds().Height() / 10));

	SetFontScale(screenView, 16);
	screenView->DrawString("YOUR COMPUTER IS OFFLINE!", messageBackgroundRect.LeftTop() + BPoint(screenView->Bounds().Width() / 12, (screenView->Bounds().Height() / 10) * 2));

	SetFontScale(screenView, 32);
	screenView->DrawString(fErrorCode, messageBackgroundRect.LeftTop() + BPoint(screenView->Bounds().Width() / 12, (screenView->Bounds().Height() / 10) * 3));

	BPoint stringPosition = messageBackgroundRect.LeftTop() + BPoint(screenView->Bounds().Width() / 12, (screenView->Bounds().Height() / 10) * 3.5f);
	if (screenView->StringWidth(fErrorMessage) < messageBackgroundRect.Width())
	{
		screenView->DrawString(fErrorMessage, stringPosition);
	}
	else
	{
		BString checkString;
		int stringLine = 1;
		int characterNo = 0;
		while (characterNo < fErrorMessage.Length())
		{
			checkString << fErrorMessage[characterNo];
			if (screenView->StringWidth(checkString) + messageBackgroundRect.LeftTop().x + screenView->Bounds().Width() / 12 >= messageBackgroundRect.Width())
			{
				screenView->DrawString(checkString, stringPosition + BPoint(0, screenView->Bounds().Height() / 20 * stringLine));
				stringLine++;
				checkString = "";
			}

			characterNo++;
		}

		if (!checkString.IsEmpty())
		{
			screenView->DrawString(checkString, stringPosition + BPoint(0, screenView->Bounds().Height() / 20 * stringLine));
		}
	}

	screenView->Sync();
}
