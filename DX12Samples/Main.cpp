#include <windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
// #include "include/directx/d3dx12.h"
#include <string>
#include <vector>
#include <iostream>
using namespace Microsoft::WRL;
using namespace DirectX;

// â ����
const wchar_t* windowTitle = L"DirectX12 �⺻ ���ø����̼�";

// ������ ũ��
const int WindowWidth = 1920;
const int WindowHeight = 1080;

// DX12 ���� ��ü��
ComPtr<ID3D12Device> device;
ComPtr<IDXGIFactory4> factory;
ComPtr<IDXGISwapChain3> swapChain;
ComPtr<ID3D12CommandQueue> commandQueue;
ComPtr<ID3D12CommandAllocator> commandAllocator;
ComPtr<ID3D12GraphicsCommandList> commandList;
ComPtr<ID3D12DescriptorHeap> rtvHeap;
ComPtr<ID3D12Resource> renderTargets[2];
ComPtr<ID3D12Fence> fence;
UINT frameIndex;
UINT rtvDescriptorSize;
HANDLE fenceEvent;
UINT64 fenceValue;

// ������ ���ν���
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

// �ʱ�ȭ �Լ���
bool InitWindow(HINSTANCE hInstance, int nCmdShow, HWND& hwnd) {
    // ������ Ŭ���� ���
    WNDCLASSEX windowClass = {};
    windowClass.cbSize = sizeof(WNDCLASSEX);
    windowClass.style = CS_HREDRAW | CS_VREDRAW;
    windowClass.lpfnWndProc = WindowProc;
    windowClass.hInstance = hInstance;
    windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    windowClass.lpszClassName = L"DX12WindowClass";
    RegisterClassEx(&windowClass);

    // ������ ����
    RECT windowRect = { 0, 0, WindowWidth, WindowHeight };
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    hwnd = CreateWindow(
        windowClass.lpszClassName,
        windowTitle,
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) {
        return false;
    }

    ShowWindow(hwnd, nCmdShow);
    return true;
}

bool InitD3D(HWND hwnd) {
    UINT dxgiFactoryFlags = 0;

#if defined(_DEBUG)
    // ����� ���̾� Ȱ��ȭ
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
        debugController->EnableDebugLayer();
        dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
#endif

    // DXGI ���丮 ����
    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, IID_PPV_ARGS(&factory)))) {
        return false;
    }

    // �ϵ���� ����� ã��
    ComPtr<IDXGIAdapter1> hardwareAdapter;
    for (UINT adapterIndex = 0; DXGI_ERROR_NOT_FOUND != factory->EnumAdapters1(adapterIndex, &hardwareAdapter); ++adapterIndex) {
        DXGI_ADAPTER_DESC1 desc;
        hardwareAdapter->GetDesc1(&desc);

        if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) {
            continue;
        }

        // DirectX12�� �����ϴ� �ϵ���� ����� ã��
        if (SUCCEEDED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr))) {
            break;
        }
    }

    // ����̽� ����
    if (FAILED(D3D12CreateDevice(hardwareAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)))) {
        return false;
    }

    // Ŀ�ǵ� ť ����
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

    if (FAILED(device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue)))) {
        return false;
    }

    // ���� ü�� ����
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = 2;
    swapChainDesc.Width = WindowWidth;
    swapChainDesc.Height = WindowHeight;
    swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.SampleDesc.Count = 1;

    ComPtr<IDXGISwapChain1> swapChain1;
    if (FAILED(factory->CreateSwapChainForHwnd(
        commandQueue.Get(),
        hwnd,
        &swapChainDesc,
        nullptr,
        nullptr,
        &swapChain1))) {
        return false;
    }

    if (FAILED(swapChain1.As(&swapChain))) {
        return false;
    }

    // Alt+Enter ��üȭ�� ����
    factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

    frameIndex = swapChain->GetCurrentBackBufferIndex();

    // ���� Ÿ�� �� �� ����
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.NumDescriptors = 2;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    if (FAILED(device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)))) {
        return false;
    }

    rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    // ���� Ÿ�� �� ����
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = rtvHeap->GetCPUDescriptorHandleForHeapStart();
    for (UINT i = 0; i < 2; i++) {
        if (FAILED(swapChain->GetBuffer(i, IID_PPV_ARGS(&renderTargets[i])))) {
            return false;
        }
        device->CreateRenderTargetView(renderTargets[i].Get(), nullptr, rtvHandle);
        rtvHandle.ptr += rtvDescriptorSize;
    }

    // Ŀ�ǵ� �Ҵ��� ����
    if (FAILED(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)))) {
        return false;
    }

    // Ŀ�ǵ� ����Ʈ ����
    if (FAILED(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator.Get(), nullptr, IID_PPV_ARGS(&commandList)))) {
        return false;
    }

    // Ŀ�ǵ� ����Ʈ�� ���� ���·� ���۵ǹǷ� �ݾ���
    commandList->Close();

    // ����ȭ�� ���� �潺 ����
    if (FAILED(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)))) {
        return false;
    }

    fenceValue = 1;
    fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    if (fenceEvent == nullptr) {
        return false;
    }

    return true;
}

// ȭ�� �׸��� �Լ�
void Render() {
    // Ŀ�ǵ� �Ҵ��� ����
    commandAllocator->Reset();

    // Ŀ�ǵ� ����Ʈ ����
    commandList->Reset(commandAllocator.Get(), nullptr);

    // ���ҽ� �踮�� - ���� Ÿ�� ���� ����
    D3D12_RESOURCE_BARRIER transition;
    transition.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    transition.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
    transition.Transition.pResource = renderTargets[frameIndex].Get();
    transition.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
    transition.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    commandList->ResourceBarrier(1, &transition);

    // ���� Ÿ�� ����
    D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle;
    rtvHandle.ptr = rtvHeap->GetCPUDescriptorHandleForHeapStart().ptr + frameIndex * rtvDescriptorSize;
    commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);

    // ȭ�� ����� (Ǫ��������)
    const float clearColor[] = { 0.0f, 0.2f, 0.4f, 1.0f };
    commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

    // ���ҽ� �踮�� - ���� Ÿ�ٿ��� ��� ���·� ����
    transition.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    transition.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
    commandList->ResourceBarrier(1, &transition);

    // Ŀ�ǵ� ����Ʈ �ݱ�
    commandList->Close();

    // Ŀ�ǵ� ����Ʈ ����
    ID3D12CommandList* ppCommandLists[] = { commandList.Get() };
    commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // ����ü�� ȭ�� ��ü
    swapChain->Present(1, 0);

    // ����ȭ (CPU�� GPU �۾� ����ȭ)
    const UINT64 currentFenceValue = fenceValue;
    commandQueue->Signal(fence.Get(), currentFenceValue);
    fenceValue++;

    // GPU�� ��� ����� �Ϸ��� ������ ��ٸ�
    if (fence->GetCompletedValue() < currentFenceValue) {
        fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    // ���� ������ �ε��� ����
    frameIndex = swapChain->GetCurrentBackBufferIndex();
}

// �޸� ���� �Լ�
void CleanupD3D() {
    // GPU�� �۾��� �Ϸ�� ������ ���
    const UINT64 currentFenceValue = fenceValue;
    commandQueue->Signal(fence.Get(), currentFenceValue);
    fenceValue++;

    if (fence->GetCompletedValue() < currentFenceValue) {
        fence->SetEventOnCompletion(currentFenceValue, fenceEvent);
        WaitForSingleObject(fenceEvent, INFINITE);
    }

    // �̺�Ʈ �ڵ� �ݱ�
    CloseHandle(fenceEvent);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    HWND hwnd;

    // ������ �ʱ�ȭ
    if (!InitWindow(hInstance, nCmdShow, hwnd)) {
        MessageBox(0, L"������ �ʱ�ȭ ����", L"����", MB_OK);
        return 1;
    }

    // DirectX �ʱ�ȭ
    if (!InitD3D(hwnd)) {
        MessageBox(0, L"DirectX �ʱ�ȭ ����", L"����", MB_OK);
        return 1;
    }

    // �޽��� ����
    MSG msg = {};
    while (msg.message != WM_QUIT) {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            // ������ ������
            Render();
        }
    }

    // ����
    CleanupD3D();

    return (int)msg.wParam;
}