#include <pch.h>
#include <base.h>

//Helper Functions Declarations

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam);
HWND GetProcessWindow();
bool GetD3D11Data(void** pSwapchainTable, size_t Size_Swapchain, void** pDeviceTable, size_t Size_Device, void** pContextTable, size_t Size_Context);

//Functions

bool Base::Hooks::Init()
{
	if (GetD3D11Data(Data::pSwapChainTable, sizeof(Data::pSwapChainTable), Data::pDeviceTable, sizeof(Data::pDeviceTable), Data::pContextTable, sizeof(Data::pContextTable)))
	{
#		if defined(MEM_86)
		mem::detour_t detour_method = mem::MEM_DT_M1;
#		elif defined(MEM_64)
		mem::detour_t detour_method = mem::MEM_DT_M0;
#		endif

		Data::pPresent = Data::pSwapChainTable[8];
		Data::oPresent = (Present_t)mem::in::detour_trampoline((mem::voidptr_t)Data::pPresent, (mem::voidptr_t)Hooks::Present, Data::szPresent, detour_method);
		//Data::oWndProc  = (WndProc_t)SetWindowLongPtr(Data::hWindow, WNDPROC_INDEX, (LONG_PTR)Hooks::WndProc); //This hook will be set in Base::Hooks::Present

		return true;
	}

	return false;
}

bool Base::Hooks::Shutdown()
{
	if (Data::InitImGui)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	mem::in::detour_restore(Data::pPresent, (mem::byte_t*)Data::oPresent, Data::szPresent);
	SetWindowLongPtr(Data::hWindow, WNDPROC_INDEX, (LONG_PTR)Data::oWndProc);

	return true;
}

//Helper Functions Definitions

BOOL CALLBACK EnumWindowsCallback(HWND handle, LPARAM lParam)
{
	DWORD wndProcId = 0;
	GetWindowThreadProcessId(handle, &wndProcId);

	if (GetCurrentProcessId() != wndProcId)
		return TRUE;

	Base::Data::hWindow = handle;
	return FALSE;
}

HWND GetProcessWindow()
{
	Base::Data::hWindow = (HWND)NULL;
	EnumWindows(EnumWindowsCallback, NULL);
	return Base::Data::hWindow;
}

bool GetD3D11Data(void** pSwapchainTable, size_t Size_Swapchain, void** pDeviceTable, size_t Size_Device, void** pContextTable, size_t Size_Context)
{
	WNDCLASSEX wc{ 0 };
	wc.cbSize = sizeof(wc);
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = TEXT("Class");

	if (!RegisterClassEx(&wc))
	{
		return false;
	}

	HWND hWnd = CreateWindow(wc.lpszClassName, TEXT(""), WS_DISABLED, 0, 0, 0, 0, NULL, NULL, NULL, nullptr);

	DXGI_SWAP_CHAIN_DESC swapChainDesc{ 0 };
	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.OutputWindow = hWnd;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevel;

	IDXGISwapChain* pDummySwapChain = nullptr;
	ID3D11Device* pDummyDevice = nullptr;
	ID3D11DeviceContext* pDummyContext = nullptr;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &swapChainDesc, &pDummySwapChain, &pDummyDevice, &featureLevel, nullptr);
	if (FAILED(hr))
	{
		DestroyWindow(swapChainDesc.OutputWindow);
		UnregisterClass(wc.lpszClassName, GetModuleHandle(nullptr));
		return false;
	}

	if (pSwapchainTable && pDummySwapChain)
	{
		memcpy(pSwapchainTable, *reinterpret_cast<void***>(pDummySwapChain), Size_Swapchain);
	}

	if (pDeviceTable && pDummyDevice)
	{
		memcpy(pDeviceTable, *reinterpret_cast<void***>(pDummyDevice), Size_Device);
	}

	if (pContextTable && pDummyContext)
	{
		memcpy(pContextTable, *reinterpret_cast<void***>(pDummyContext), Size_Context);
	}

	if (pDummySwapChain)
	{
		pDummySwapChain->Release();
	}

	if (pDummyDevice)
	{
		pDummyDevice->Release();
	}

	if (pDummyContext)
	{
		pDummyContext->Release();
	}

	DestroyWindow(swapChainDesc.OutputWindow);
	UnregisterClass(wc.lpszClassName, GetModuleHandle(nullptr));

	return true;
}