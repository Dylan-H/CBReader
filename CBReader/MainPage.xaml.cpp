//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"

using namespace CBReader;
using namespace Concurrency;
using namespace Platform;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::Storage;
using namespace Windows::Storage::Pickers;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

MainPage::MainPage()
{
	InitializeComponent();
}


void CBReader::MainPage::canvasControl_Draw(Microsoft::Graphics::Canvas::UI::Xaml::CanvasControl^ sender, Microsoft::Graphics::Canvas::UI::Xaml::CanvasDrawEventArgs^ args)
{

	args->DrawingSession->DrawEllipse(155, 115, 80, 30, Windows::UI::Colors::Blue, 3);
	args->DrawingSession->DrawText("Hello, world!", 100, 100, Windows::UI::Colors::Yellow);
	//args->DrawingSession->DrawText()
	//args->DrawingSession->DrawRectangle()
}


void CBReader::MainPage::Page_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args)
{
	//ApplicationData^ appd = ApplicationData::Current;

	String^ path =ApplicationData::Current->LocalFolder->Path;
	path += "\\mm.mobi";
	std::wstring textToConvert = std::wstring(path->Data());
	const wchar_t *faceName = textToConvert.c_str();
	//mDoc = Doc::CreateFromFile(faceName);

	Windows::Storage::StorageFolder^ storageFolder =
		Windows::Storage::ApplicationData::Current->LocalFolder;
	//Windows::Storage::StorageFile^ sampleFile =
	create_task(storageFolder->CreateFileAsync("sample.txt", Windows::Storage::CreationCollisionOption::ReplaceExisting)).then([&](Windows::Storage::StorageFile^ File)
	{
		
	});



	auto picker = ref new FileOpenPicker();
	picker->FileTypeFilter->Append(".mobi");
	create_task(picker->PickSingleFileAsync()).then([this](StorageFile^ file)
	{
		if (file != nullptr)
		{
			return  file->Path;
		}
	
	}).then([this](Platform::String^ path )
	{
		Platform::String^ text = path;

		std::wstring textToConvert = std::wstring(text->Data());
		const wchar_t *faceName = textToConvert.c_str();
		mDoc = Doc::CreateFromFile(faceName);
	});
}
