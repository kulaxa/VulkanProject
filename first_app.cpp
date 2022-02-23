#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_ball_physics.hpp"


//imgui
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

static ImGui_ImplVulkanH_Window g_MainWindowData;
static void check_vk_result(VkResult err)
{
    // if (err == 0)
    //     return;
    // fprintf(stderr, "[vulkan] Error: VkResult = %d\n", err);
    // if (err < 0)
    //     abort();
    return;
}

namespace lve
{

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

        PhysicsSystem ballPhyisicsSystem(gameObjects);

        init_imgui();

        while (!lveWindow.shouldClose())
        {

           
              
            

            glfwPollEvents(); // gleda sve user evenete

            if (auto commandBuffer = lveRenderer.beginFrame())
            {

           
                 ImGui_ImplVulkan_NewFrame();
		        ImGui_ImplGlfw_NewFrame();

		        ImGui::NewFrame();


        //imgui commands
        ImGui::ShowDemoWindow();

        //your draw function
		
                // my system update fucntions
                ballPhyisicsSystem.update();
                // render system
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRendererSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }

        vkDeviceWaitIdle(lveDevice.device());
    }

     bool checkIfOccupided(float xPos, float yPos, std::vector<glm::vec2>& positions, float radius, float delta){
            float minDistance = 10.0f;
            glm::vec2 point;
            for(auto p: positions){
               float distance = sqrt(pow(xPos-p.x, 2.0f) + pow(yPos-p.y, 2.0f));
               if(distance < minDistance){
                   minDistance = distance;
                   point.x = p.x;
                   point.y = p.y;
               }
               if(distance < 2*radius +delta){
                    return true;
                }
            }
            //std::cout << "{" <<xPos << ", "<<yPos<<"}, "<<"{"<<point.x <<", "
          //  << point.y << "}, distance :"<<minDistance - 2*radius << ", radius: "<<radius<<  std::endl;
            return false;
    }

    void FirstApp::loadBalls(int numOfBalls, float maxRadius, 
    float delta, float maxSpeed, std::vector<LveModel::Vertex> &vertices){
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
        std::uniform_real_distribution<double> unifRadius(0.3*maxRadius, maxRadius);
       
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

    void FirstApp::init_imgui()
    {

        //1: create descriptor pool for IMGUI
	// the size of the pool is very oversize, but it's copied from imgui demo itself.
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
	pool_info.maxSets = 1000;
	pool_info.poolSizeCount = std::size(pool_sizes);
	pool_info.pPoolSizes = pool_sizes;

	VkDescriptorPool imguiPool;
	//VK_CHECK(vkCreateDescriptorPool(lveDevice.device(), &pool_info, nullptr, &imguiPool));


	// 2: initialize imgui library

	//this initializes the core structures of imgui
	IMGUI_CHECKVERSION();
    ImGui::CreateContext(nullptr);

    //this initializes imgui for FWLG
    ImGui_ImplGlfw_InitForVulkan(lveWindow.getWindow(), true);

    // setting up imgui
        //  Setup Dear ImGui context
       // IMGUI_CHECKVERSION();
        

       
        VkDescriptorPool g_DescriptorPool;
        
        
        // Setup Platform/Renderer bindings
        ImGui_ImplGlfw_InitForVulkan(lveWindow.getWindow(), true);


        ImGui_ImplVulkan_InitInfo init_info = {};
        init_info.Instance = lveDevice.getInstance();
        init_info.PhysicalDevice = lveDevice.getPhysicalDevice();
        init_info.Device = lveDevice.device();
        init_info.QueueFamily = lveDevice.findPhysicalQueueFamilies().graphicsFamily;
        init_info.Queue = lveDevice.graphicsQueue();
        init_info.PipelineCache = VK_NULL_HANDLE;
        init_info.DescriptorPool = g_DescriptorPool;
        init_info.Allocator = nullptr;
        init_info.MinImageCount = LveSwapChain::MAX_FRAMES_IN_FLIGHT;
        init_info.ImageCount = lveRenderer.getSwapChain() ->imageCount();
        init_info.CheckVkResultFn = check_vk_result;

        //ImGui_ImplVulkan_Init(&init_info, lveRenderer.getSwapChainRenderPass());


      
        // VkAttachmentDescription attachment = {}; 
        // attachment.format = lveRenderer.getSwapChain() -> findDepthFormat();
        // attachment.samples = VK_SAMPLE_COUNT_1_BIT;
        // attachment.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        // attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        // attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        // attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        // attachment.initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        // attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        // VkAttachmentReference color_attachment = {};
        // color_attachment.attachment = 0;
        // color_attachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        // VkSubpassDescription subpass = {};
        // subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        // subpass.colorAttachmentCount = 1;
        // subpass.pColorAttachments = &color_attachment;

        // VkSubpassDependency dependency = {};
        // dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
        // dependency.dstSubpass = 0;
        // dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        // dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        // dependency.srcAccessMask = 0; // or VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        // dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        // VkRenderPassCreateInfo info = {};
        // info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        // info.attachmentCount = 1;
        // info.pAttachments = &attachment;
        // info.subpassCount = 1;
        // info.pSubpasses = &subpass;
        // info.dependencyCount = 1;
        // info.pDependencies = &dependency;
        // VkRenderPass imGuiRenderPass;
        // if (vkCreateRenderPass(lveDevice.device(), &info, nullptr, &imGuiRenderPass) != VK_SUCCESS)
        // {
        //     throw std::runtime_error("Could not create Dear ImGui's render pass");
        // }

        // ImGui_ImplVulkanH_Window* wd = &g_MainWindowData;
        // ImGui_ImplVulkan_Init(&init_info, imGuiRenderPass);

    }
}
