#pragma once


#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "imgui.h"
#include "imgui_impl_vulkan.h"

#include <memory>
#include <vector>
namespace lve{
    class SimpleRendererSystem{
        public:


        SimpleRendererSystem(LveDevice& device, VkRenderPass renderPass);
        ~SimpleRendererSystem();

        SimpleRendererSystem(const SimpleRendererSystem&) = delete;
        SimpleRendererSystem &operator=(const SimpleRendererSystem &) = delete;
        
        void renderImgui(VkCommandBuffer commandBuffer, ImDrawData* drawData);
        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &GameObjects);
        private:
          
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);
    
            
            //my code:


        
            LveDevice &lveDevice;
           
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
           
    };
}
