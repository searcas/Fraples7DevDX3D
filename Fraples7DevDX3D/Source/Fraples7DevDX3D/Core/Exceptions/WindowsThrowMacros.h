#pragma once

#define FPL_EXCEPT( hr ) Window::HrException( __LINE__,__FILE__,(hr) )
#define FPL_LAST_EXCEPT() Window::HrException( __LINE__,__FILE__,GetLastError() )
#define FPL_NOGFX_EXCEPT() Window::NoGfxException( __LINE__,__FILE__ )
