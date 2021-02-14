#pragma once
#ifndef BASE_H
#define BASE_H

#include "pch.h"

#if defined(MEM_86)
#define WNDPROC_INDEX GWL_WNDPROC
#define PRESENT_CALL __stdcall
#elif defined(MEM_64)
#define WNDPROC_INDEX GWLP_WNDPROC
#define PRESENT_CALL __fastcall
#endif
#define D3D11DEV_LEN 40
#define D3D11SC_LEN  18
#define D3D11CTX_LEN 108

typedef HRESULT(PRESENT_CALL* Present_t)(IDXGISwapChain*, UINT, UINT);
typedef LRESULT(CALLBACK*  WndProc_t) (HWND, UINT, WPARAM, LPARAM);

DWORD WINAPI MainThread(LPVOID lpThreadParameter);
DWORD WINAPI ExitThread(LPVOID lpThreadParameter);

namespace Base
{
	bool Init();
	bool Shutdown();
	bool Detach();

	namespace Data
	{
		extern HMODULE                 hModule;
		extern ID3D11Device*           pDxDevice11;
		extern IDXGISwapChain*         pSwapChain;
		extern ID3D11DeviceContext*    pContext;
		extern ID3D11RenderTargetView* pMainRenderTargetView;
		extern void*                   pDeviceTable   [D3D11DEV_LEN];
		extern void*                   pSwapChainTable[D3D11SC_LEN];
		extern void*                   pContextTable  [D3D11CTX_LEN];
		extern HWND                    hWindow;
		extern mem::voidptr_t          pPresent;
		extern Present_t               oPresent;
		extern WndProc_t               oWndProc;
		extern mem::size_t             szPresent;
		extern UINT                    WmKeys[0xFF];
		extern bool                    Detached;
		extern bool                    InitImGui;
		extern bool                    ShowMenu;

		namespace Keys
		{
			const UINT ToggleMenu = VK_INSERT;
			const UINT DetachDll  = VK_END;
		}
	}

	namespace Hooks
	{
		bool Init();
		bool Shutdown();
		HRESULT PRESENT_CALL Present(IDXGISwapChain* thisptr, UINT SyncInterval, UINT Flags);
		LRESULT CALLBACK  WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	}
}

#endif