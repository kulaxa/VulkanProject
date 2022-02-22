#include "first_app.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <stdexcept>
#include <array>
#include <iostream>
#include <math.h>

#include <random>
#include <chrono>

#include <glm/gtc/constants.hpp>
namespace lve{

    struct SimplePushConstantData{
         glm::mat2 transform{1.f};
    
        glm::vec2 offset;
       alignas(16)  glm::vec3 color;
    };

    FirstApp::FirstApp(){
        loadGameObjects();
        createPipelineLayout();
        createPipeline();
    }

    FirstApp::~FirstApp(){
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }
    void FirstApp::run(){
        while(!lveWindow.shouldClose()){
            glfwPollEvents(); //gleda sve user evenete
           if(auto commandBuffer = lveRenderer.beginFrame()){
               lveRenderer.beginSwapChainRenderPass(commandBuffer);
               renderGameObjects(commandBuffer);
               lveRenderer.endSwapChainRenderPass(commandBuffer);
               lveRenderer.endFrame();
           }
            
        }

        vkDeviceWaitIdle(lveDevice.device());
    }



  
    void FirstApp::loadGameObjects(){
        int numOfBalls = 40;
        std::vector<LveModel::Vertex> vertices{
            {{0.0f, -0.5f},{1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f},{0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f},{0.0f, 0.0f, 1.0f}}
           
        };
        

        auto lveModel1 = std::make_shared<LveModel>(lveDevice, vertices);
          
              

        
         
          
        auto triangle = LveGameObject::createGameObject();
        triangle.model =lveModel1;
        triangle.color={.1f, .8f, .1f};
        triangle.tranform2d.translation.x = .2f;
        triangle.tranform2d.scale={2.f, 0.5f};
        triangle.tranform2d.rotation = 0.25f*glm::two_pi<float>();
          gameObjects.push_back(std::move(triangle));


      

      
    }

    void FirstApp::createPipelineLayout(){

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType =VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount =0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount =1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;
        if(vkCreatePipelineLayout(lveDevice.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) !=
        VK_SUCCESS){
            throw std::runtime_error("failed to create pipeline layout!");
        }
    }

    void FirstApp::createPipeline(){

        assert(pipelineLayout != nullptr && "Cannot create pieline before pipeline layout");
        
        PipelineConfiguInfo pipelineConfig{};
       LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveRenderer.getSwapChainRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
       
    }

    
     

  

    

    


  

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer){
        lvePipeline ->bind(commandBuffer);
  
        
       static std::vector<int> escaped;
    
         for(auto& obj: gameObjects){
              
          

        
                 
            
                

        

                    obj.tranform2d.rotation = glm::mod(obj.tranform2d.rotation + 0.01f, glm::two_pi<float>());

                        
                    SimplePushConstantData push{};

                    // my code

                    push.offset = obj.tranform2d.translation;

                    push.color = obj.color;
                    push.transform = obj.tranform2d.mat2();

                    // }

                    vkCmdPushConstants(commandBuffer,
                                       pipelineLayout,
                                       VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                                       0,
                                       sizeof(SimplePushConstantData),
                                       &push);

                    obj.model->bind(commandBuffer);
                    obj.model->draw(commandBuffer);
                }
         
    }
  



}
