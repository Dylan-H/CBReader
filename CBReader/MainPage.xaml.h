//
// MainPage.xaml.h
// Declaration of the MainPage class.
//

#pragma once

#include "MainPage.g.h"
#include "Doc.h"
using namespace Microsoft::Graphics::Canvas::UI::Xaml;
namespace CBReader
{
	/// <summary>
	/// An empty page that can be used on its own or navigated to within a Frame.
	/// </summary>
	public ref class MainPage sealed
	{
	public:
		MainPage();

	private:
		void canvasControl_Draw(CanvasControl^ sender, CanvasDrawEventArgs^ args);

	private:
		CanvasControl mCanvasControl;
		Doc  mDoc;
		void Page_Loading(Windows::UI::Xaml::FrameworkElement^ sender, Platform::Object^ args);
	};
}
