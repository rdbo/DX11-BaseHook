#include <pch.h>
#include <base.h>

//Data
HMODULE                 Base::Data::hModule     = (HMODULE)NULL;
void*                   Base::Data::pDeviceTable   [D3D11DEV_LEN];
void*                   Base::Data::pSwapChainTable[D3D11SC_LEN];
void*                   Base::Data::pContextTable  [D3D11CTX_LEN];
ID3D11Device*           Base::Data::pDxDevice11 = (ID3D11Device*)NULL;
IDXGISwapChain*         Base::Data::pSwapChain  = (IDXGISwapChain*)NULL;
ID3D11DeviceContext*    Base::Data::pContext    = (ID3D11DeviceContext*)NULL;
ID3D11RenderTargetView* Base::Data::pMainRenderTargetView = (ID3D11RenderTargetView*)NULL;
HWND                    Base::Data::hWindow     = (HWND)NULL;
mem::voidptr_t          Base::Data::pPresent    = (mem::voidptr_t)NULL;
Present_t               Base::Data::oPresent    = (Present_t)NULL;
WndProc_t               Base::Data::oWndProc    = (WndProc_t)NULL;
#if defined(MEM_86)
mem::size_t             Base::Data::szPresent = 5;
#elif defined(MEM_64)
mem::size_t             Base::Data::szPresent = 19;
#endif
UINT                    Base::Data::WmKeys[0xFF];
bool                    Base::Data::Detached   = false;
bool                    Base::Data::ShowMenu   = true;
bool                    Base::Data::InitImGui  = false;


//Functions

bool Base::Init()
{
	Hooks::Init();
	return true;
}

bool Base::Shutdown()
{
	Hooks::Shutdown();
	return true;
}

bool Base::Detach()
{
	Base::Shutdown();
	CreateThread(nullptr, 0, ExitThread, Data::hModule, 0, nullptr);
	return true;
}