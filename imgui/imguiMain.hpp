#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
namespace lve{

    
class ImGuiEverything{
    public:
static VkAllocationCallbacks* g_Allocator ;  
static VkInstance            g_Instance;   
static VkPhysicalDevice     g_PhysicalDevice;
static VkDevice                g_Device;
static uint32_t                 g_QueueFamily;
static VkQueue                  g_Queue;
static VkDebugReportCallbackEXT g_DebugReport;
static VkPipelineCache         g_PipelineCache;
static VkDescriptorPool         g_DescriptorPool;
static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount ;
static bool                     g_SwapChainRebuild;
GLFWwindow* window;
bool show_demo_window;
bool show_another_window;
VkResult err;
ImGuiIO& io;
ImGui_ImplVulkanH_Window* wd;
ImVec4 clear_color;


public: 
    //ImGuiEverything(){};
static void SetupVulkan(const char** extensions, uint32_t extensions_count);
static void check_vk_result(VkResult err);
static void SetupVulkanWindow(ImGui_ImplVulkanH_Window* wd, VkSurfaceKHR surface, int width, int height);
static void FrameRender(ImGui_ImplVulkanH_Window* wd, ImDrawData* draw_data);
static void FramePresent(ImGui_ImplVulkanH_Window* wd);
static void CleanupVulkanWindow();
static void CleanupVulkan();
void imgui_init();



};

}