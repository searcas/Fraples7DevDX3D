#include "DxgiInfoManager.h"
#include "../../Platform/Windows/Window.h"
#include "../Debugging/Exceptions/Macros/GraphicsThrowMacros.h"
#include "../Debugging/Exceptions/Macros/WindowsThrowMacros.h"
#include <memory>
FraplesDev::DxgiInfoManager::DxgiInfoManager()
{
	//define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	//load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
	{
		throw FPL_LAST_EXCEPT();
	}
	//get adress of DXGIGetDebugInterface in dll
	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	
	if (DxgiGetDebugInterface == nullptr)
	{
		throw FPL_LAST_EXCEPT();
	}
	HRESULT hr;
	FPL_GFX_THROW_NOINFO(DxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), &pDxgiInfoQueue));
}

void FraplesDev::DxgiInfoManager::Set() noexcept
{
	//set the index (next) so that the next all to GetMessages()
	// will only get errors generated after this call
	next = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> FraplesDev::DxgiInfoManager::GetMessages() const
{
	std::vector<std::string>messages;
	const auto end = pDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (int i = next; i < end; i++)
	{
		HRESULT hr;
		SIZE_T messageLength;
		//get the size of message i in bytes
		FPL_GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		//alocate memory for message
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		//get the message and push its description into the vector
		FPL_GFX_THROW_NOINFO(pDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, pMessage, &messageLength));
		messages.emplace_back(pMessage->pDescription);
	}
	return messages;
}
