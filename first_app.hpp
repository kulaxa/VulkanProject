#pragma once

#include "lve_window.hpp"
#include "lve_pipeline.hpp"
#include "lve_device.hpp"
#include "lve_model.hpp"
#include "lve_game_object.hpp"
#include "lve_renderer.hpp"


#include <memory>
#include <vector>
namespace lve{
    class FirstApp{
        public:
        static constexpr int WIDTH = 800;
        static constexpr int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp &operator=(const FirstApp &) = delete;
        void run();
        private:
            void loadGameObjects();
            void createPipelineLayout();
            void createPipeline();
        
           
   
    
      
            void renderGameObjects(VkCommandBuffer commandBuffer);
            //my code:
            void makeVertices(int num, std::vector<LveModel::Vertex> *vertices);
             uint32_t makeCircle(LveModel::Vertex center, float radius, float angle, std::vector<LveModel::Vertex> *vertices);
             void FillVert(LveModel::Vertex center, float size, std::vector<LveModel::Vertex> *vertices, int depth);
           

            LveWindow lveWindow{WIDTH, HEIGHT, "Vulkan tutorial!"};
            LveDevice lveDevice{lveWindow};
            LveRenderer lveRenderer{lveWindow, lveDevice};
            std::unique_ptr<LvePipeline> lvePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<LveGameObject> gameObjects;
    };
}
