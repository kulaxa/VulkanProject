#include "first_app.hpp"
#include "simple_render_system.hpp"
//#include "lve_ball_physics.hpp"


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

       // PhysicsSystem ballPhyisicsSystem(gameObjects);
        

       
        while (!lveWindow.shouldClose())
        {

            glfwPollEvents(); // gleda sve user evenete
           
            if (auto commandBuffer = lveRenderer.beginFrame())
            {

                // imgui commands

                // your draw function

                // my system update fucntions
               // ballPhyisicsSystem.update();
                // render system
                lveRenderer.beginSwapChainRenderPass(commandBuffer);
                simpleRendererSystem.renderGameObjects(commandBuffer, gameObjects);
                lveRenderer.endSwapChainRenderPass(commandBuffer);
                lveRenderer.endFrame();
            }
        }


        vkDeviceWaitIdle(lveDevice.device());
    }

    // bool checkIfOccupided(float xPos, float yPos, std::vector<glm::vec2> &positions, float radius, float delta)
    // {
    //     float minDistance = 10.0f;
    //     glm::vec2 point;
    //     for (auto p : positions)
    //     {
    //         float distance = sqrt(pow(xPos - p.x, 2.0f) + pow(yPos - p.y, 2.0f));
    //         if (distance < minDistance)
    //         {
    //             minDistance = distance;
    //             point.x = p.x;
    //             point.y = p.y;
    //         }
    //         if (distance < 2 * radius + delta)
    //         {
    //             return true;
    //         }
    //     }
    //     // std::cout << "{" <<xPos << ", "<<yPos<<"}, "<<"{"<<point.x <<", "
    //     //  << point.y << "}, distance :"<<minDistance - 2*radius << ", radius: "<<radius<<  std::endl;
    //     return false;
    // }

    void FirstApp::loadBalls(int numOfBalls, float maxRadius,
                             float delta, float maxSpeed, std::vector<LveModel::Vertex> &vertices)
    {
        // // very random seed
        // srand(time(NULL));
        // std::mt19937_64 rng;
        // uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
        // std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed >> 32)};
        // rng.seed(ss);

        // std::uniform_real_distribution<double> unif(-1 + maxRadius + delta, 1 - maxRadius - delta);
        // std::uniform_real_distribution<double> unifSpeed(-maxSpeed, maxSpeed);
        // std::uniform_real_distribution<double> unifColor(0, 1);
        // std::vector<glm::vec2> positions;
        // std::uniform_real_distribution<double> unifRadius(0.3 * maxRadius, maxRadius);

        // for (int i = 0; i < numOfBalls; i++)
        // {

        //     float xSpeed = unifSpeed(rng);
        //     float ySpeed = unifSpeed(rng);

        //     float xPos, yPos;
        //     int occuCounter = 0;

        //     xPos = unif(rng);
        //     yPos = unif(rng);

        //     if (positions.empty())
        //     {
        //         positions.push_back({xPos, yPos});
        //     }
        //     else
        //     {
        //         while (checkIfOccupided(xPos, yPos, positions, maxRadius, delta) && occuCounter <= 10)
        //         {
        //             occuCounter++;
        //             xPos = unif(rng);
        //             yPos = unif(rng);
        //         }
        //         positions.push_back({xPos, yPos});
        //     }

        //     float r = unifColor(rng);
        //     float g = unifColor(rng);
        //     float b = unifColor(rng);
        //     //    float r = 1.0f;
        //     //    float g = 1.0f;
        //     //    float b = 1.0f;
        //     float radius = unifRadius(rng);

        //     vertices.clear();
        //     makeCircle({{0.0f, 0.0f}}, radius, 0.1, &vertices);
        //     auto lveModel = std::make_shared<LveModel>(lveDevice, vertices);
        //     auto circle = LveGameObject::createGameObject();

        //     circle.model = lveModel;
        //     circle.color = {r, g, b};
        //     circle.speedVec = {xSpeed, ySpeed};
        //     circle.tranform2d.translation.x = xPos;
        //     circle.tranform2d.translation.y = yPos;
        //     //  circle.tranform2d.scale = {1.0f, WIDTH/HEIGHT};
        //     circle.mass = radius;
        //     circle.radius = radius;

        //     if (occuCounter < 10)
        //     {
        //         gameObjects.push_back(std::move(circle));
        //     }
        //     else
        //     {
        //         std::cout << "cant add this object no space " << std::endl;
        //     }
        // }
    }

std::unique_ptr<LveModel> createCubeModel(LveDevice& device, glm::vec3 offset) {
  std::vector<LveModel::Vertex> vertices{
 
      // left face (white)
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
      {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
      {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
 
      // right face (yellow)
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
      {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
 
      // top face (orange, remember y axis points down)
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
      {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
      {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
 
      // bottom face (red)
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
      {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
      {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
 
      // nose face (blue)
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
      {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
      {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
 
      // tail face (green)
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
      {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
      {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
 
  };
  for (auto& v : vertices) {
    v.position += offset;
  }
  return std::make_unique<LveModel>(device, vertices);
}

    void FirstApp::loadGameObjects()
    {
        //
        std::vector<LveModel::Vertex> vertices;
        // int numOfBalls = 15;
        // float maxRadius = 0.1f;
        // float delta = 0.005f;
        // float maxSpeed = 0.02f;
        // loadBalls(numOfBalls, maxRadius, delta, maxSpeed, vertices);
       
//         std::shared_ptr<LveModel> lveModel = createCubeModel(lveDevice, {.0f, .0f, .0f});
//   auto cube = LveGameObject::createGameObject();
//   cube.model = lveModel;
//   cube.transform.translation = {.0f, .0f, .5f};
//   cube.transform.scale = {.5f, .5f, .5f};
  //gameObjects.push_back(std::move(cube));

  // makeCircle({{0.f, 0.f ,0.0f}}, 0.5f, 0.1, &vertices);
   makeAlmostSpehere({{0.f, 0.f ,-1.f}}, 0.5f, 0.1, &vertices);
   std::shared_ptr<LveModel> lveModelCircle = std::make_unique<LveModel>(lveDevice, vertices);
   auto circle = LveGameObject::createGameObject();
   circle.model = lveModelCircle;
   gameObjects.push_back(std::move(circle));

    }

}
