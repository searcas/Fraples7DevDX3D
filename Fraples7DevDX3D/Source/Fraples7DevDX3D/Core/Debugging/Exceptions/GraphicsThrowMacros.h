#pragma once
namespace FraplesDev {
#ifdef NDEBUG
#define INFOMAN(GFX) HRESULT hr
#else
#define INFOMAN(gfx) HRESULT hr; DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif // NDEBUG

#ifdef NDEBUG
#define INFOMAN_NOHR(gfx)
#else
#define INFOMAN_NOHR(gfx) DxgiInfoManager& infoManager = GetInfoManager((gfx))
#endif // NDEBUG

#define FPL_GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__,__FILE__,(hr))
#define FPL_GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HrException(__LINE__,__FILE__,hr)

#ifdef _DEBUG
#define FPL_GFX_EXCEPT( hr ) FraplesDev::Graphics::HrException( __LINE__,__FILE__,(hr),infoManager.GetMessages())
#define FPL_GFX_THROW_INFO(hrcall) infoManager.Set(); if(FAILED(hr = (hrcall))) throw FPL_GFX_EXCEPT(hr)
#define FPL_GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__,__FILE__,hr,infoManager.GetMessages())
#define FPL_GFX_THROW_INFO_ONLY(call) infoManager.Set(); (call); {auto v=infoManger.Getmessages(); if(!v.empty()) throw Graphics::InfoException(__LINE__,__FILE__,v);}}

#else
#define FPL_GFX_EXCEPT( hr ) 
#define FPL_GFX_THROW_INFO(hrcall) 
#define FPL_GFX_DEVICE_REMOVED_EXCEPT(hr) 
#define FPL_GFX_THROW_INFO_ONLY(call) 

#endif




}