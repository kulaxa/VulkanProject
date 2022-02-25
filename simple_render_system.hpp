#pragma once


#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_camera.hpp"


#include <memory>
#include <vector>
namespace lve{
    class SimpleRendererSystem{
        public:


        SimpleRendererSystem(LveDevice& device, VkRenderPass renderPass);
        ~SimpleRendererSystem();

        SimpleRendererSystem(const SimpleRendererSystem&) = delete;
        SimpleRendererSystem &operator=(const SimpleRendererSystem &) = delete;
        

        void renderGameObjects(VkCommandBuffer commandBuffer, std::vector<LveGameObject> &GameObjects, const LveCamera &camera);
        private:
          
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            
            //my code:


        
            LveDevice &lveDevice;
           
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
           
    };
}
