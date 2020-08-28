#pragma once
namespace FraplesDev {

#define FPL_EXCEPT( hr ) FraplesDev::Window::HrException( __LINE__,__FILE__,(hr) )
#define FPL_LAST_EXCEPT() FraplesDev::Window::HrException( __LINE__,__FILE__,GetLastError() )
#define FPL_NOGFX_EXCEPT() FraplesDev::Window::NoGfxException( __LINE__,__FILE__ )

}