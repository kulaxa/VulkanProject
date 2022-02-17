#include "first_app.hpp"
#include <stdexcept>
#include <array>
#include <iostream>
#include <math.h>
namespace lve{

    FirstApp::FirstApp(){
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp(){
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }
    void FirstApp::run(){
        while(!lveWindow.shouldClose()){
            glfwPollEvents(); //gleda sve user evenete
            drawFrame();
            
        }

        vkDeviceWaitIdle(lveDevice.device());
    }
    void makeVertices(int num, std::vector<LveModel::Vertex>* vertices){
        float x1, x2, x3, y1, y2, y3;
        float xprev=-1.0f, yprev = -1.0f;
        for(int j=1; j<=num; j++){
            if(j%2==0){

            }
        for(int i=1; i<= num; i++){
            x1 =xprev +(2.0f/num);
            x2 = xprev;
            x3=(x1+ x2)/2;
            xprev = x1;
            y1 = yprev;
            y2 = y1;
            y3 = yprev + (2.0f/num);
            if(j%2==0){
               vertices ->push_back({{x1,y3}});
              vertices ->push_back({{x2,y3}});
              vertices ->push_back({{x3,y1}});

            }
            else{
           vertices ->push_back({{x1,y1}});
              vertices ->push_back({{x2,y2}});
              vertices ->push_back({{x3,y3}});

            }
              

        }   
        xprev=-1.0f;
        yprev += (2.0f/num);

        }
    }

    void makeCircle(LveModel::Vertex center, float radius, float angle, std::vector<LveModel::Vertex> *vertices)
    {
        if (radius < 0 || radius > 1)
            throw std::runtime_error("Wrong radius");
        if (angle < 0 || angle > 2 * M_PI)
            throw std::runtime_error("Wrong angle");
        float xc = center.postion[0];
        float yc = center.postion[1];
        float xprev = xc + radius;
        float yprev = yc;
        float times = (2 * M_PI) / angle;


        for (int i = 0; i < times; i++)
        {

            float x1 = sqrt(pow(radius, 2.0f) / (1 + pow(tan(i * angle), 2.0f)));
            if (i * angle > M_PI / 2.0f && i * angle < M_PI * (3.0f / 2))
            {
                x1 *= -1.0f;

            }
            float y1 = -1.0f * tan(i * angle) * x1;

            vertices->push_back({{xc, yc}});
            vertices->push_back({{x1 + xc, y1 + yc}});
            if (i == 0)
            {
                vertices->push_back({{radius + xc, yc}});
   
            }
            else
            {
                vertices->push_back({{xprev + xc, yprev + yc}});
             
            }
            xprev = x1;
            yprev = y1;
        }

        vertices->push_back({{xc, yc}});
        vertices->push_back({{xprev + xc, yprev + yc}});
        vertices->push_back({{xc + radius, yc}});
     
    }
    void FirstApp::loadModels(){
        std::vector<LveModel::Vertex> vertices{
            // {{0.0f, -0.5f}},
            // {{0.5f, 0.5f}},
            // {{-0.5f, 0.5f}},
            // {{-1.0f, -1.0f}},
            // {{-1.0f, -0.9f}},
            // {{-0.9f, -1.0f}},
            //  {{1.0f, 1.0f}},
            // {{1.0f, 0.9f}},
            // {{0.9f, 1.0f}},
            // {{-1.0f, 1.0f}},
            // {{-1.0f, 0.9f}},
            // {{-0.9f, 1.0f}}
        };
        

        //makeVertices(200, &vertices);
        makeCircle({{0.4f, -0.4f}}, 0.5f, 0.5, &vertices);
        makeCircle({{0.75f, 0.75f}}, 0.2f, 0.1, &vertices);
        lveModel = std::make_unique<LveModel>(lveDevice, vertices);
    }

    void FirstApp::createPipelineLayout(){
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount =0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount =0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        if(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline(){
        auto pipelineConfig = LvePipeline::defaultPipelineConfigInfo(lveSwapChain.width(), lveSwapChain.height());
        pipelineConfig.renderPass = lveSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
       
    }

    void FirstApp::createCommandBuffers(){
        commandBuffers.resize(lveSwapChain.imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != 
        VK_SUCCESS){
            throw std::runtime_error("faile to allocate command bufferes!");
        }

        for(int i=0; i< commandBuffers.size(); i++){
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if(vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS){
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset ={0,0};
            renderPassInfo.renderArea.extent = lveSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.1f, 0.1f, 0.1f, 1.0f};

            clearValues[1].depthStencil= {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lvePipeline ->bind(commandBuffers[i]);
            lveModel ->bind(commandBuffers[i]);
            lveModel -> draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);
            if(vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS){
                throw std::runtime_error("failes to record command buffer!");
            }
        }
    }
    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = lveSwapChain.acquireNextImage(&imageIndex);
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        result = lveSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image!");
        }
    }



}