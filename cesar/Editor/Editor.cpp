#include "Editor.h"
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_win32.h>
#include <ImGui/imgui_impl_dx12.h>

#include <d3d12.h>

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

using namespace cesar::render_graph;

namespace cesar
{
    static void Alloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* out_cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE* out_gpu_handle)
    {
        DescriptorAllocator* pool = static_cast<DescriptorAllocator*>(info->UserData);
        Descriptor descriptor = pool->Allocate();
        *out_cpu_handle = GetCPUHandle(descriptor);
        *out_gpu_handle = GetGPUHandle(descriptor);
        return;
    };

    static void Dealloc(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu_handle, D3D12_GPU_DESCRIPTOR_HANDLE gpu_handle) {
        DescriptorAllocator* pool = static_cast<DescriptorAllocator*>(info->UserData);
        Uint32 index = pool->GetHeap()->GetDescriptorIndex(cpu_handle);
        pool->FreeDescriptor(index);
        return;
    };

    Editor::Editor(Window* window, Engine* engine, GPUContext* gpu_context)
        :window(window), is_visible(true), engine(engine),
        viewport(this)
	{
        DescriptorHeapDesc heap_desc{};
        heap_desc.descriptor_count = 1024;
        heap_desc.is_shader_visible = true;
        heap_desc.type = DescriptorType::CBV_SRV_UAV;
        descriptor_pool = std::make_unique<DescriptorAllocator>(std::make_unique<DescriptorHeap>(gpu_context, heap_desc));
		
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO(); (void)io;
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

        ImGui::StyleColorsDark();

        ImGuiStyle& style = ImGui::GetStyle();
        io.ConfigDpiScaleFonts = true;          
        io.ConfigDpiScaleViewports = true;      

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            style.WindowRounding = 0.0f;
            style.Colors[ImGuiCol_WindowBg].w = 1.0f;
        }

        ImGui_ImplWin32_Init(window->GetNative());

        ImGui_ImplDX12_InitInfo init_info = {};
        init_info.Device = gpu_context->GetDevice();
        init_info.CommandQueue = gpu_context->GetGraphicsCommandQueue()->GetCommandQueue();
        init_info.NumFramesInFlight = FRAMES_IN_FLIGHT;
        init_info.RTVFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;

        init_info.SrvDescriptorHeap = descriptor_pool->GetHeap()->GetDescriptorHeap();

        init_info.SrvDescriptorAllocFn = Alloc;
        init_info.SrvDescriptorFreeFn = Dealloc;
        init_info.UserData = static_cast<DescriptorAllocator*>(descriptor_pool.get());

        ImGui_ImplDX12_Init(&init_info);
	}
    Editor::~Editor()
    {
        ImGui_ImplDX12_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }
    void Editor::ImGuiBegin()
    {
        ImGui_ImplDX12_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        BeginDocking();
    }
    void Editor::ImGuiEnd(CommandList& cmd_list)
    {
        EndDocking();

        ImGui::Render();
        
        ID3D12GraphicsCommandList* d3d12_cmd_list = (ID3D12GraphicsCommandList*)cmd_list.GetCommandList();
        ID3D12DescriptorHeap* imgui_heap[] = { (ID3D12DescriptorHeap*)descriptor_pool->GetHeap()->GetDescriptorHeap() };
        d3d12_cmd_list->SetDescriptorHeaps(ARRAYSIZE(imgui_heap), imgui_heap);
        ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), d3d12_cmd_list);
        
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }
    }

    void Editor::BeginDocking()
    {
        ImGuiIO& io = ImGui::GetIO();

        ImGuiWindowFlags flags =
            ImGuiWindowFlags_MenuBar |
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_NoTitleBar |
            ImGuiWindowFlags_NoCollapse |
            ImGuiWindowFlags_NoResize |
            ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoBringToFrontOnFocus |
            ImGuiWindowFlags_NoNavFocus;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);

        ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
        ImGui::Begin("MainDockSpace", nullptr, flags);
        ImGui::PopStyleVar(2);

        ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
        ImGui::DockSpace(dockspace_id, ImVec2(0, 0), ImGuiDockNodeFlags_PassthruCentralNode);
    }

    void Editor::EndDocking()
    {
        ImGui::End();
    }

    void Editor::AddPass(render_graph::RenderGraph& render_graph)
    {
        using namespace render_graph;

        struct PassData {
            TextureReadOnly viewport_texture; //final_texture;
        };
        render_graph.AddPass<PassData>("Editor Pass", RGPassType::Graphics, RGPassFlags::None,
            [&](PassData& data, RenderGraphBuilder& builder)
            {
                TextureViewDesc desc{};
                desc.array_size = 1;
                desc.mip = 0;
                desc.mip_levels = 1;
                desc.slice = 0;

                builder.WriteRenderTarget(RG_NAME(Backbuffer), ResourceLoadStoreFlags::ClearPreserve, desc);
                data.viewport_texture = builder.ReadTexture(RG_NAME(FinalTexture), ReadAccessType::AllShader, desc);
            },
            [&](PassData& data, RenderGraphContext& context)
            {
                ImGuiBegin();
                
                Descriptor viewport_texture_desc = context.GetTextureReadOnlyDescriptor(data.viewport_texture);
                viewport.DrawViewport(GetGPUHandle(viewport_texture_desc).ptr);

                CommandList& cmd_list = context.GetCommandList();

                ImGuiEnd(cmd_list);
            });

    }

    Bool Editor::IsVisible() const
    {
        return is_visible;
    }

    void Editor::ToggleVisibilty()
    {
        is_visible = !is_visible;
    }
    
    void Editor::OnWindowEvent(const WindowEventInfo& info)
    {
        ImGui_ImplWin32_WndProcHandler(static_cast<HWND>(info.hwnd),
            info.msg, info.wparam, info.lparam);
    }
}
