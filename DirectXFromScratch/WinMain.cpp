#include "win.h"
#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	Window janela( 640, 480, "Janela Principal" );

	MSG msg;
	BOOL gResult;
	while ( (gResult = GetMessage( &msg, nullptr, 0, 0 )) > 0 )
	{
		TranslateMessage( &msg );
		DispatchMessage( &msg );
	}
	if ( gResult == -1 )
	{
		return -1;
	}
	else {
		return msg.wParam;
	}
}