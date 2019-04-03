#include "Window.h"






Window::Window( int width, int height, const char * name )
	:
	width( width ),
	height( height )
{
	RECT wr;
	wr.left = 100;
	wr.right = width + wr.left;
	wr.top = 100;
	wr.bottom = height + wr.top;

	AdjustWindowRect( &wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE );

	hWnd = CreateWindow(
		WindowClass::GetName(), name,
		WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
		CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
		nullptr, nullptr, WindowClass::GetInstance(), this
	);

	ShowWindow( hWnd, SW_SHOWDEFAULT );
}

Window::~Window()
{
	DestroyWindow( hWnd );
}

LRESULT WINAPI Window::HandleMsgSetup( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( msg )
	{
	case WM_NCCREATE:
		const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>( lParam );
		Window* const pWnd = reinterpret_cast<Window*>( pCreate->lpCreateParams );
		SetWindowLongPtr( hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>( pWnd ) );
		SetWindowLongPtr( hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>( HandleMsgThunk ) );
		return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

LRESULT WINAPI Window::HandleMsgThunk( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	Window* const pWnd = reinterpret_cast<Window*>( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
	return pWnd->HandleMsg( hWnd, msg, wParam, lParam );
}

LRESULT Window::HandleMsg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) noexcept
{
	switch ( msg )
	{
	case WM_CLOSE:
		PostQuitMessage( 0 );
		return 0;

	case WM_KILLFOCUS:
		kbd.ClearState();
		break;

	case WM_MOUSEMOVE:
		const POINTS pt = MAKEPOINTS( lParam );
		if ( pt.x > 0 && pt.x < width && pt.y >= 0 && pt.y < height )
		{
			mouse.OnMouseMove( pt.x, pt.y );
			if ( !mouse.IsInWindow() )
			{
				SetCapture( hWnd );
				mouse.OnMouseEnter();
			}
		}
		else
		{
			if ( wParam & ( MK_LBUTTON | MK_RBUTTON ) )
			{
				mouse.OnMouseMove( pt.x, pt.y );
			}
			else
			{
				ReleaseCapture();
				mouse.OnMouseLeave();
			}
		}

		break;
	case WM_LBUTTONDOWN:
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftPresssed( pt.x, pt.y );
		break;
	case WM_LBUTTONUP:
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnLeftReleased( pt.x, pt.y );
		break;
	case WM_RBUTTONDOWN:
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightPressed( pt.x, pt.y );
		break;
	case WM_RBUTTONUP:
		const POINTS pt = MAKEPOINTS( lParam );
		mouse.OnRightReleased( pt.x, pt.y );
		break;
	case WM_MOUSEWHEEL:
		const POINTS pt = MAKEPOINTS( lParam );
		if ( GET_WHEEL_DELTA_WPARAM( wParam ) > 0 )
		{
			mouse.OnWheelUp( pt.x, pt.y );
		}
		else if ( GET_WHEEL_DELTA_WPARAM( wParam ) < 0 )
		{
			mouse.OnWheelDown( pt.x, pt.y );
		}

	case WM_KEYDOWN:
	case WM_SYSKEYDOWN:
		if ( !( lParam & 0x40000000 ) || kbd.AutorepeatIsEnabled() )
		{
			kbd.OnKeyPressed( static_cast<unsigned char>( wParam ) );
		}
		break;

	case WM_KEYUP:
	case WM_SYSKEYUP:
		kbd.OnKeyRelease( static_cast<unsigned char>( wParam ) );
		break;
	case WM_CHAR:
		kbd.OnChar( static_cast<unsigned char>( wParam ) );
		break;
	}
	return DefWindowProc( hWnd, msg, wParam, lParam );
}

const char * Window::WindowClass::GetName() noexcept
{
	return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
	return wndClass.hInst;
}

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept
	:
	hInst( GetModuleHandle( nullptr ) )
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof( wc );
	wc.hInstance = GetInstance();
	wc.style = CS_OWNDC;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = nullptr;
	wc.hCursor = nullptr;
	wc.hIcon = nullptr;
	wc.hIconSm = nullptr;
	wc.lpfnWndProc = HandleMsgSetup;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = GetName();
	RegisterClassEx( &wc );
}

Window::WindowClass::~WindowClass()
{
	UnregisterClass( wndClassName, hInst );
}
