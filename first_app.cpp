#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "lve_ball_physics.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <random>
#include <chrono>
#include <math.h>
#include <iostream>

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
  
        while (!lveWindow.shouldClose())
        {
            glfwPollEvents(); // gleda sve user evenete

            if (auto commandBuffer = lveRenderer.beginFrame())
            {

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
}
