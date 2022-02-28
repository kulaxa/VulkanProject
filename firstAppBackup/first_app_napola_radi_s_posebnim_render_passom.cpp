#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_ball_physics.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <random>
#include <chrono>
#include <math.h>
#include <iostream>
//######################################################################
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"
#include <stdio.h>          // printf, fprintf
#include <stdlib.h>         // abort
#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

//#define IMGUI_UNLIMITED_FRAME_RATE
#ifdef _DEBUG
#define IMGUI_VULKAN_DEBUG_REPORT
#endif

static VkAllocationCallbacks*   g_Allocator = NULL;
static VkInstance               g_Instance = VK_NULL_HANDLE;
static VkPhysicalDevice         g_PhysicalDevice = VK_NULL_HANDLE;
static VkDevice                 g_Device = VK_NULL_HANDLE;
static uint32_t                 g_QueueFamily = (uint32_t)-1;
static VkQueue                  g_Queue = VK_NULL_HANDLE;
static VkDebugReportCallbackEXT g_DebugReport = VK_NULL_HANDLE;
static VkPipelineCache          g_PipelineCache = VK_NULL_HANDLE;
static VkDescriptorPool         g_DescriptorPool = VK_NULL_HANDLE;

static ImGui_ImplVulkanH_Window g_MainWindowData;
static int                      g_MinImageCount = 2;
static bool                     g_SwapChainRebuild = false;

static void check_vk_result(VkResult err)
{
    if (err == 0)
        return;
    fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    if (err < 0)
        abort();
}

#ifdef IMGUI_VULKAN_DEBUG_REPORT
static VKAPI_ATTR VkBool32 VKAPI_CALL debug_report(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode, const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
    (void)flags; (void)object; (void)location; (void)messageCode; (void)pUserData; (void)pLayerPrefix; // Unused arguments
    fprintf(stderr, "[vulkan] Debug report from ObjectType: %i\nMessage: %s\n\n", objectType, pMessage);
    return VK_FALSE;
}
#endif // IMGUI_VULKAN_DEBUG_REPORT

//#######################################################################
namespace lve
{void createImguiRenderpass(VkDevice device, VkRenderPass *imGuiRenderPass, VkFormat imageFormat, VkFormat depthFormat)
    {
            VkAttachmentDescription attachment = {};
        attachment.format = imageFormat;
        attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR ;
        attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        VkAttachmentReference color_attachment = {};
        color_attachment.attachment = 0;
        color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment;
        VkSubpassDependency dependency = {};
        dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass = 0;
        dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        VkRenderPassCreateInfo info = {};
        info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        info.attachmentCount = 1;
        info.pAttachments = &attachment;
        info.subpassCount = 1;
        info.pSubpasses = &subpass;
        info.dependencyCount = 1;
        info.pDependencies = &dependency;
              if (vkCreateRenderPass(device, &info, nullptr, imGuiRenderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("Could not create Dear ImGui's render pass");
        }

//         VkAttachmentDescription attachment = {};
//         attachment.format = imageFormat;
//         attachment.samples = VK_SAMPLE_COUNT_1_BIT;
//         attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
//         attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
//         attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
//         attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
//         attachment.initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;//VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
//         attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

// //           VkAttachmentDescription depthAttachment{};
// //   depthAttachment.format = depthFormat;
// //   depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
// //   depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
// //   depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// //   depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
// //   depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
// //   depthAttachment.initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
// //   depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

// //   VkAttachmentReference depthAttachmentRef{};
// //   depthAttachmentRef.attachment = 1;
// //   depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

//         VkAttachmentReference color_attachment = {};
//         color_attachment.attachment = 0;
//         color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

//         VkSubpassDescription subpass = {};
//         subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
//         subpass.colorAttachmentCount = 1;
//         subpass.pColorAttachments = &color_attachment;  
//        // subpass.pDepthStencilAttachment = &depthAttachmentRef;

//         VkSubpassDependency dependency = {};
//         dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
//         dependency.dstSubpass = 0;
//         dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//         dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
//         dependency.srcAccessMask =   VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT; //0
//         dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

//         VkRenderPassCreateInfo info = {};
//         info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
//         info.attachmentCount = 1;
//         info.pAttachments = &attachment;
//         info.subpassCount = 1;
//         info.pSubpasses = &subpass;
//         info.dependencyCount = 1;
//         info.pDependencies = &dependency;

// //         std::array<VkAttachmentDescription, 2> attachments = {attachment, depthAttachment};
// //   VkRenderPassCreateInfo info = {};
// //   info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
// //   info.attachmentCount = static_cast<uint32_t>(attachments.size());
// //   info.pAttachments = attachments.data();
// //   info.subpassCount = 1;
// //   info.pSubpasses = &subpass;
// //   info.dependencyCount = 1;
// //   info.pDependencies = &dependency;
//         if (vkCreateRenderPass(device, &info, nullptr, imGuiRenderPass) != VK_SUCCESS)
//         {
//             throw std::runtime_error("Could not create Dear ImGui's render pass");
//         }
    }


    

    FirstApp::FirstApp()
    {
        loadGameObjects();
    }

    FirstApp::~FirstApp()
    {
    }


     void FirstApp::run()
    {

        SimpleRendererSystem simpleRendererSystem{lveDevice, lveRenderer.getSwapChainRenderPass()};
         VkRenderPass imGuiRenderPass ;
    createImguiRenderpass(lveDevice.device(), &imGuiRenderPass, lveRenderer.getSwapChain()->getSwapChainImageFormat(), lveRenderer.getSwapChain()->findDepthFormat());
        
       PhysicsSystem ballPhyisicsSystem(gameObjects);
        //###################################################################################################################

    GLFWwindow* window = lveWindow.getWindow();
 
    g_Instance = lveDevice.getInstance();
     g_PhysicalDevice = lveDevice.getPhysicalDevice();
    g_Device = lveDevice.device();
    g_Queue =lveDevice.graphicsQueue();
     g_QueueFamily = lveDevice.findPhysicalQueueFamilies().graphicsFamily;


// Create Descriptor Pool
    //ovo bi trebal copy pastati 
    { 
        VkDescriptorPoolSize pool_sizes[] =
        {
            { VK_DESCRIPTOR_TYPE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 },
            { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1000 },
            { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC, 1000 },
            { VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, 1000 }
        };
        VkDescriptorPoolCreateInfo pool_info = {};
        pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        pool_info.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
        pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
        pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
        pool_info.pPoolSizes = pool_sizes;
        VkResult err = vkCreateDescriptorPool(g_Device, &pool_info, g_Allocator, &g_DescriptorPool);
        check_vk_result(err);
    }

    // Create Window Surface
    VkSurfaceKHR surface = lveDevice.surface();

    // Create Framebuffers
    int w, h;
    glfwGetFramebufferSize(window, &w, &h); 
    ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
      wd->Surface = surface;


         const VkFormat requestSurfaceImageFormat[] = { VK_FORMAT_B8G8R8A8_UNORM, VK_FORMAT_R8G8B8A8_UNORM, VK_FORMAT_B8G8R8_UNORM, VK_FORMAT_R8G8B8_UNORM };
    const VkColorSpaceKHR requestSurfaceColorSpace = VK_COLORSPACE_SRGB_NONLINEAR_KHR;
  
    wd->SurfaceFormat = ImGui_ImplVulkanH_SelectSurfaceFormat(g_PhysicalDevice, wd->Surface, requestSurfaceImageFormat, (size_t)IM_ARRAYSIZE(requestSurfaceImageFormat), requestSurfaceColorSpace);
  
    wd->PresentMode = VK_PRESENT_MODE_FIFO_KHR;

     wd->ImageCount = lveRenderer.getImageCount();

      wd->Swapchain = lveRenderer.getSwapChain()->getSwapChainMYFUNCTION();
              //  wd->RenderPass = lveRenderer.getSwapChain() -> getRenderPass();
              wd -> RenderPass = imGuiRenderPass;
                 wd->ImageCount = lveRenderer.getSwapChain() -> imageCount();


    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsClassic();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForVulkan(window, true);
    ImGui_ImplVulkan_InitInfo init_info = {};
    init_info.Instance = g_Instance; //jep
    init_info.PhysicalDevice = g_PhysicalDevice;//jep
    init_info.Device = g_Device;//jep
    init_info.QueueFamily = g_QueueFamily;//jep
    init_info.Queue = g_Queue;//jep
    init_info.PipelineCache = g_PipelineCache;
    init_info.DescriptorPool = g_DescriptorPool; //jep
    init_info.Subpass = 0; //jep
    init_info.MinImageCount = g_MinImageCount; //jep
    init_info.ImageCount = wd->ImageCount; //jep
    init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
    init_info.Allocator = g_Allocator;
    init_info.CheckVkResultFn = check_vk_result;
  

    ImGui_ImplVulkan_Init(&init_info, wd->RenderPass);


    // Upload Fonts
    {
        // Use any command queue
        VkCommandPool command_pool = lveDevice.getCommandPool();//wd->Frames[wd->FrameIndex].CommandPool;
        VkCommandBuffer command_buffer =lveDevice.beginSingleTimeCommands();//wd->Frames[wd->FrameIndex].CommandBuffer;

        VkResult err = vkResetCommandPool(g_Device, command_pool, 0);
        check_vk_result(err);
        VkCommandBufferBeginInfo begin_info = {};
        begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        begin_info.flags |= VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
        err = vkBeginCommandBuffer(command_buffer, &begin_info);
        check_vk_result(err);

        ImGui_ImplVulkan_CreateFontsTexture(command_buffer);

        VkSubmitInfo end_info = {};
        end_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        end_info.commandBufferCount = 1;
        end_info.pCommandBuffers = &command_buffer;
        err = vkEndCommandBuffer(command_buffer);
        check_vk_result(err);
        err = vkQueueSubmit(g_Queue, 1, &end_info, VK_NULL_HANDLE);
        check_vk_result(err);

        err = vkDeviceWaitIdle(g_Device);
        check_vk_result(err);
        ImGui_ImplVulkan_DestroyFontUploadObjects();
    }

    // Our state
    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    // Main loop

        //####################################################################################################################    

        while (!lveWindow.shouldClose())
        {

            glfwPollEvents(); // gleda sve user evenete
   

            if (auto bufferimgui = lveRenderer.beginFrame())
            {

                ballPhyisicsSystem.update();
                 VkRenderPass imGuiRenderPass ;
    createImguiRenderpass(lveDevice.device(), &imGuiRenderPass, lveRenderer.getSwapChain()->getSwapChainImageFormat(), lveRenderer.getSwapChain()->findDepthFormat());
                // render system
               lveRenderer.beginSwapChainRenderPass(bufferimgui);
              simpleRendererSystem.renderGameObjects(bufferimgui, gameObjects);

             if (g_SwapChainRebuild)
        {
            int width, height;
            glfwGetFramebufferSize(window, &width, &height);
            if (width > 0 && height > 0)
            {
                ImGui_ImplVulkan_SetMinImageCount(g_MinImageCount);
                g_MainWindowData.FrameIndex = 0;
                g_SwapChainRebuild = false;
            }
        }

                 wd->Swapchain = lveRenderer.getSwapChain()->getSwapChainMYFUNCTION();
              //  wd->RenderPass = lveRenderer.getSwapChain() -> getRenderPass();
              wd->RenderPass =imGuiRenderPass;
                 wd->ImageCount = lveRenderer.getSwapChain() -> imageCount();

        // Start the Dear ImGui frame
       
        ImGui_ImplVulkan_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
     

     if (show_demo_window)
           // ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
            ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
            ImGui::Checkbox("Another Window", &show_another_window);

            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
            ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
                counter++;
            ImGui::SameLine();
            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
        
        // Rendering
        
        ImGui::Render();
     
        ImDrawData* main_draw_data = ImGui::GetDrawData();
        const bool main_is_minimized = (main_draw_data->DisplaySize.x <= 0.0f || main_draw_data->DisplaySize.y <= 0.0f);
        wd->ClearValue.color.float32[0] = clear_color.x * clear_color.w;
        wd->ClearValue.color.float32[1] = clear_color.y * clear_color.w;
        wd->ClearValue.color.float32[2] = clear_color.z * clear_color.w;
        wd->ClearValue.color.float32[3] = clear_color.w;

           if (!main_is_minimized)
           {
               simpleRendererSystem.renderImgui(bufferimgui,main_draw_data );
           }
           // Update and Render additional Platform Windows
           if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
           {
          
               ImGui::UpdatePlatformWindows();
              ImGui::RenderPlatformWindowsDefault();
        
           }
     
           // Present Main Platform Window
         

        lveRenderer.endSwapChainRenderPass(bufferimgui);
        vkDestroyRenderPass(lveDevice.device(), imGuiRenderPass,nullptr);
           lveRenderer.endFrame();
            }

        }

        vkDeviceWaitIdle(lveDevice.device());
        vkDestroyRenderPass(lveDevice.device(), imGuiRenderPass,nullptr);
        ImGui_ImplVulkan_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

     vkDestroyDescriptorPool(g_Device, g_DescriptorPool, g_Allocator);

        glfwDestroyWindow(window);
        glfwTerminate();
    }
        


        


    bool checkIfOccupided(float xPos, float yPos, std::vector<glm::vec2> &positions, float radius, float delta)
    {
        float minDistance = 10.0f;
        glm::vec2 point;
        for (auto p : positions)
        {
            float distance = sqrt(pow(xPos - p.x, 2.0f) + pow(yPos - p.y, 2.0f));
            if (distance < minDistance)
            {
                minDistance = distance;
                point.x = p.x;
                point.y = p.y;
            }
            if (distance < 2 * radius + delta)
            {
                return true;
            }
        }
        // std::cout << "{" <<xPos << ", "<<yPos<<"}, "<<"{"<<point.x <<", "
        //  << point.y << "}, distance :"<<minDistance - 2*radius << ", radius: "<<radius<<  std::endl;
        return false;
    }



    void FirstApp::loadBalls(int numOfBalls, float maxRadius,
                             float delta, float maxSpeed, std::vector<LveModel::Vertex> &vertices)
    {
        // very random seed
        srand(time(NULL));
        std::mt19937_64 rng;
        uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
        rng.seed(ss);

        std::uniform_real_distribution<double> unif(-1 + maxRadius + delta, 1 - maxRadius - delta);
        std::uniform_real_distribution<double> unifSpeed(-maxSpeed, maxSpeed);
        std::uniform_real_distribution<double> unifColor(0, 1);
        std::vector<glm::vec2> positions;
        std::uniform_real_distribution<double> unifRadius(0.3 * maxRadius, maxRadius);

        for (int i = 0; i < numOfBalls; i++)
        {

            float xSpeed = unifSpeed(rng);
            float ySpeed = unifSpeed(rng);

            float xPos, yPos;
            int occuCounter = 0;

            xPos = unif(rng);
            yPos = unif(rng);

            if (positions.empty())
            {
                positions.push_back({xPos, yPos});
            }
            else
            {
                while (checkIfOccupided(xPos, yPos, positions, maxRadius, delta) && occuCounter <= 10)
                {
                    occuCounter++;
                    xPos = unif(rng);
                    yPos = unif(rng);
                }
                positions.push_back({xPos, yPos});
            }

            float r = unifColor(rng);
            float g = unifColor(rng);
            float b = unifColor(rng);
            //    float r = 1.0f;
            //    float g = 1.0f;
            //    float b = 1.0f;
            float radius = unifRadius(rng);

            vertices.clear();
            makeCircle({{0.0f, 0.0f}}, radius, 0.1, &vertices);
            auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
            auto circle = LveGameObject::createGameObject();

            circle.model = lveModel;
            circle.color = {r, g, b};
            circle.speedVec = {xSpeed, ySpeed};
            circle.tranform2d.translation.x = xPos;
            circle.tranform2d.translation.y = yPos;
            //  circle.tranform2d.scale = {1.0f, WIDTH/HEIGHT};
            circle.mass = radius;
            circle.radius = radius;

            if (occuCounter < 10)
            {
                gameObjects.push_back(std::move(circle));
            }
            else
            {
                std::cout << "cant add this object no space " << std::endl;
            }
        }
    }

    void FirstApp::loadGameObjects()
    {
        //
        std::vector<LveModel::Vertex> vertices{

        };
        //     auto lveModel1 = std::make_shared<LveModel>(lveDevice, vertices);
        //     auto triangle = LveGameObject::createGameObject();
        //     triangle.model =lveModel1;
        //     triangle.color={.1f, .8f, .1f};
        //     triangle.tranform2d.translation.x = .2f;
        //     triangle.tranform2d.scale={2.f, 0.5f};
        //     triangle.tranform2d.rotation = 0.25f*glm::two_pi<float>();
        //       gameObjects.push_back(std::move(triangle));
        int numOfBalls = 15;
        float maxRadius = 0.1f;
        float delta = 0.005f;
        float maxSpeed = 0.02f;
        loadBalls(numOfBalls, maxRadius, delta, maxSpeed, vertices);

        //   vertices.clear();
        // makeCircle({{0.0f, 0.0f}}, radius, 0.1, &vertices);
        //     auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        //     auto circle = LveGameObject::createGameObject();

        //     circle.model = lveModel;
        //     circle.speedVec = {-0.02f, -0.04f};
        //     circle.tranform2d.translation.x = -0.5f;
        //     circle.tranform2d.translation.y = 0;
        //     circle.tranform2d.scale = {1.0f, WIDTH/HEIGHT};
        //     circle.mass = radius;
        //     circle.radius = radius;
        //     gameObjects.push_back(std::move(circle));
        //       vertices.clear();
        //    makeCircle({{0.0f, 0.0f}}, radius, 0.1, &vertices);
        //  lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        //  circle = LveGameObject::createGameObject();

        // circle.model = lveModel;
        // circle.speedVec = {0.03f, -0.01f};
        // circle.tranform2d.translation.x = 0.50f;
        // circle.tranform2d.translation.y = 0.0;
        // circle.tranform2d.scale = {1.0f, WIDTH/HEIGHT};
        // circle.mass = radius;
        // circle.radius = radius;
        // gameObjects.push_back(std::move(circle));
    }

}


//     GLFWwindow* window = lveWindow.getWindow();
    // g_Instance = lveDevice.getInstance();
    // g_PhysicalDevice = lveDevice.getPhysicalDevice();
    // g_Device = lveDevice.device();
    // g_Queue =lveDevice.graphicsQueue();
    // g_QueueFamily = lveDevice.findPhysicalQueueFamilies().graphicsFamily;
    // q_image_count = lveRenderer.getSwapChain().get()->imageCount();
