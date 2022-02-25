#include "simple_render_system.hpp"


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
         glm::mat4 transform{1.f};
         alignas(16) glm::vec3 color{};
    };

    SimpleRendererSystem::SimpleRendererSystem(LveDevice& device, VkRenderPass renderPass) : lveDevice{device}{

        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRendererSystem::~SimpleRendererSystem(){
        vkDestroyPipelineLayout(lveDevice.device(), pipelineLayout, nullptr);
    }

    void SimpleRendererSystem::createPipelineLayout(){

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

    void SimpleRendererSystem::createPipeline(VkRenderPass renderPass){

        assert(pipelineLayout != nullptr && "Cannot create pieline before pipeline layout");
        
        PipelineConfiguInfo pipelineConfig{};
       LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
       
    }


    void SimpleRendererSystem::renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &gameObjects, const LveCamera& camera){
        lvePipeline ->bind(commandBuffer);
  
        
    
         for(auto& obj: gameObjects){
              
                   obj.transform.rotation.y = glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
                obj.transform.rotation.x = glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());
                   //std::cout <<"rotation x : "<< obj.transform.rotation.x<< "\n";
                   // std::cout <<"{x,y,z}: {"<< obj.transform.x << ", "<< obj.transform.y << ", "<<obj.transform.z<<"} \n";    
                   obj.transform.translation.z += 0.001f;
               
                    SimplePushConstantData push{};

                    // my code

                    push.color = obj.color;
                   push.transform = camera.getProjection() *obj.transform.mat4();

                  

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
