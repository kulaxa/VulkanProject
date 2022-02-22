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
        recreateSwapChain();
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

    bool checkIfOccupided(float xPos, float yPos, std::vector<glm::vec2> positions, float radius, float delta){
   
          for(auto p: positions){
                float distance = sqrt(pow(xPos-p.x, 2.0f) + pow(yPos-p.y, 2.0f));
               
               
                if(distance < 2*radius +delta){
                    // std::cout <<"distance is: "<<distance <<", for: {"<<xPos<< ", "<<yPos<<"} {"
                    // << p.x << ", "<< p.y<< "}, added: "<<positions.size()<<std::endl;
                    
                      return true;
                }
            }
            
            return false;
    }

  
    void FirstApp::loadGameObjects(){
        int numOfBalls = 40;
        std::vector<LveModel::Vertex> vertices{
            {{0.0f, -0.5f},{1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f},{0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f},{0.0f, 0.0f, 1.0f}}
           
        };
        

    //    auto lveModel1 = std::make_shared<LveModel>(lveDevice, vertices);
            srand(time(NULL));
              

              float radiusNotScaled = 0.3f;
              float xSize =0.1f, ySize =0.1f;
              float delta = 0.005f;
               float radius = radiusNotScaled*xSize;
               float speed = 0.1f;
            //very random seed
          
             std::mt19937_64 rng;
              uint64_t timeSeed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
                std::seed_seq ss{uint32_t(timeSeed & 0xffffffff), uint32_t(timeSeed>>32)};
            rng.seed(ss);

            std::uniform_real_distribution<double> unif(-1+radius+delta, 1-radius-delta);
             std::uniform_real_distribution<double> unifSpeed(0, speed);


            std::vector<glm::vec2> positions;
            
         for (int i = 0; i < numOfBalls; i++)
         {
             
            //  float xSpeed =   1.0f/ ((random() % 10));
            //  float ySpeed =  1.0f/ ((random() % 10));

            float xSpeed = unifSpeed(rng);
            float ySpeed = sqrt(pow(speed,2.0f)-pow(xSpeed, 2.0f));

             int signX = random() %2;
            int signY = random() %2;
           

            if(signX ==0){
                    xSpeed *= -1.0f;

                   
            }else if(signY==0)
            {
                ySpeed *= -1.0f;
            }

            

          float xPos, yPos;
          int occuCounter =0;
         // do{
              
            xPos = unif(rng);
             yPos = unif(rng);



            signX = random() %2;
             signY = random() %2;

            
            if(signX ==0){
                    xPos *= -1.0f;

                   
            }else if(signY==0)
            {
                yPos *= -1.0f;
            }
             if(positions.empty()){
                 positions.push_back({xPos, yPos});
             }else{
                 while(checkIfOccupided(xPos, yPos, positions, radius,delta) && occuCounter <= 10){
                     occuCounter++;
                       xPos = unif(rng);
                       yPos = unif(rng);
                 }
                 positions.push_back({xPos, yPos});
             }
             
            // occuCounter++;
         // }while(checkIfOccupided(xPos, yPos, positions, radius) && occuCounter <=10);
            //positions.push_back({xPos, yPos});
           

            // if(xPos >= 1.0f){
            //     xPos -= radius*xSize;
            // }
            // else if(xPos <= -1.0f){
            //     xPos += radius *xSize;
            // }
            // else if(yPos >= 1.0f){
            //     yPos -= radius*ySize;
            // }
            // else if(yPos <= -1.0f){
            //     yPos += radius * ySize;
            // }



            float r = (random() %256)/256.f;
            float g =  (random() %256)/256.f;
            float b =  (random() %256)/256.f;
            
             vertices.clear();
             makeCircle({{0.0f, -0.0f}}, radiusNotScaled, 0.1, &vertices);
             auto lveModel1 = std::make_shared<LveModel>(lveDevice, vertices);
             auto circle = LveGameObject::createGameObject();
             circle.model = lveModel1;
             circle.tranform2d.scale = {xSize, ySize}; 
             circle.color = {r,g,b};
             circle.speedVec = {xSpeed, ySpeed};
             circle.speed = speed;
              circle.tranform2d.translation.x = xPos;
             circle.tranform2d.translation.y = yPos;
              circle.radius = radius;
              if(occuCounter <10){
             gameObjects.push_back(std::move(circle));
              }
              else{
                  std::cout << "cant add this object no space "<< std::endl;
              }
              
         }
            std::cout << "positions size: "<< positions.size()<<std::endl;
        //   vertices.clear();
        //   makeCircle({{0.0f, -0.0f}}, 0.5f, 0.1, &vertices);
        //   auto lveModel2 = std::make_shared<LveModel>(lveDevice, vertices);
          
        // auto triangle = LveGameObject::createGameObject();
        // triangle.model =lveModel1;
        // triangle.color={.1f, .8f, .1f};
        // triangle.tranform2d.translation.x = .2f;
        // triangle.tranform2d.scale={2.f, 0.5f};
        // triangle.tranform2d.rotation = 0.25f*glm::two_pi<float>();
        //   gameObjects.push_back(std::move(triangle));

       // for(int j=0; j<numOfVert ; j++){
      
        

        // auto circle2 = LveGameObject::createGameObject();
        // circle2.model =lveModel2;
        // circle2.tranform2d.scale = {0.1f, 0.1f};
        // circle2.tranform2d.translation.x = -0.5f; //lijevi rub mu je na -0.5-0.5*0.1
        //  circle2.speedVec ={-1.0f, 0.5f};
        // circle2.speed = 0.03f;
        //     circle2.color ={0.0f, 0.5f, 0.2f};
        //     circle2.tranform2d.smjer = true;
        //     gameObjects.push_back(std::move(circle2));
      

      
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

        assert(lveSwapChain != nullptr && "Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "Cannot create pieline before pipeline layout");
        
        PipelineConfiguInfo pipelineConfig{};
       LvePipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = lveSwapChain ->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lvePipeline = std::make_unique<LvePipeline>(
            lveDevice,
            "shaders/simple_shader.vert.spv",
            "shaders/simple_shader.frag.spv",
            pipelineConfig);
       
    }

    void FirstApp::recreateSwapChain(){
        auto extent = lveWindow.getExtent();
        while(extent.width == 0 || extent.height ==0){
            extent = lveWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lveDevice.device());
        if(lveSwapChain == nullptr){
         lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent);
        }else{
            lveSwapChain = std::make_unique<LveSwapChain>(lveDevice, extent, std::move( lveSwapChain));
            if(lveSwapChain -> imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }
        
     
        createPipeline();
    }

    void FirstApp::createCommandBuffers(){
        commandBuffers.resize(lveSwapChain ->imageCount());

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lveDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

        if (vkAllocateCommandBuffers(lveDevice.device(), &allocInfo, commandBuffers.data()) != 
        VK_SUCCESS){
            throw std::runtime_error("faile to allocate command bufferes!");
        }

      
    }

    void FirstApp::freeCommandBuffers(){
        vkFreeCommandBuffers(lveDevice.device(), lveDevice.getCommandPool(), static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());

        commandBuffers.clear();
    }

    void FirstApp::recordCommandBuffer(int imageIndex){
        // static int frame = 0;
        // static bool right=true;
        //static int couter =0;
        //frame = (frame+1) % 100;

         VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            if(vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS){
                throw std::runtime_error("failed to begin recording command buffer!");
            }
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lveSwapChain->getRenderPass();
            renderPassInfo.framebuffer = lveSwapChain ->getFrameBuffer(imageIndex);

            renderPassInfo.renderArea.offset ={0,0};
            renderPassInfo.renderArea.extent = lveSwapChain-> getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f}; //background color

            clearValues[1].depthStencil= {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            VkViewport viewport{};
            viewport.x =0.0f;
            viewport.y = 0.0f;
            viewport.width = static_cast<float>(lveSwapChain->getSwapChainExtent().width);
            viewport.height = static_cast<float>(lveSwapChain->getSwapChainExtent(). height);
            viewport.minDepth =0.0f;
            viewport.maxDepth = 1.0f;
            VkRect2D scissor{{0,0}, lveSwapChain->getSwapChainExtent()};
            vkCmdSetViewport(commandBuffers[imageIndex], 0,1,&viewport);
            vkCmdSetScissor(commandBuffers[imageIndex], 0,1,&scissor);


                    renderGameObjects(commandBuffers[imageIndex]);

        
            vkCmdEndRenderPass(commandBuffers[imageIndex]);
            if(vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS){
                throw std::runtime_error("failes to record command buffer!");
            }
    }

    bool checkIfCollidedWithWall(LveGameObject& object1){

        //problem je ako ide prebrzo i prođe zid i promjeni mu se na jedan frame brzina, ali onda opet sljedeći
        //frame opet je u zidu i promjeni mu se opet
        static float radius = object1.radius;
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
        object1.changeCounter++;

         float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x * object1.speed ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y * object1.speed;

      //if(object1.changeCounter > 0){
         
        if(posX+radius >= 1.0f  || nextPosX+radius >= 1.0f   ) {
            if(object1.lastWallHit != "rightWall" || object1.lastHit != object1.getId()){
            object1.changeCounter =0;
            object1.lastWallHit="rightWall";
            object1.lastHit = object1.getId();
             object1.speedVec.x *= -1.0f;
          
            return true;
            }
        }
        else if(posX-radius <= -1.0f ||  nextPosX-radius <= -1.0f){
            if(object1.lastWallHit != "leftWall" || object1.lastHit != object1.getId()){
                object1.changeCounter =0;
                object1.lastWallHit="leftWall";
                object1.lastHit = object1.getId();
                object1.speedVec.x *= -1.0f;
          
            return true;
            }
        }
       
        else if(posY +radius >= 1.0f ||  nextPosY+radius >=1.0f){
            if(object1.lastWallHit != "upperWall"|| object1.lastHit != object1.getId()){
            object1.changeCounter =0;
            object1.lastWallHit="upperWall";
             object1.lastHit = object1.getId();
           
            object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }
        else if(posY- radius < -1.0f ||  nextPosY-radius <=-1.0f){
            if(object1.lastWallHit != "bottomWall"|| object1.lastHit != object1.getId()){
            object1.changeCounter =0;
            object1.lastWallHit="bottomWall";
             object1.lastHit = object1.getId();
           
            object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }

      //}
       

        return false;
    }

    bool checkIfCollidedAndUpdate(LveGameObject &object1, LveGameObject &object2)
    {
        static float radius = object1.radius;
       
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
        float otherPosX = object2.tranform2d.translation.x;
        float otherPosY = object2.tranform2d.translation.y;

        float distance = sqrt(pow(posX-otherPosX, 2.0f) + pow(posY-otherPosY, 2.0f));


        float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x * object1.speed ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y * object1.speed;

        float otherNextPosX = object2.tranform2d.translation.x +object2.speedVec.x * object2.speed ;
        float otherNextPosY = object2.tranform2d.translation.y +object2.speedVec.y * object2.speed;
  
        float distanceNext  = sqrt(pow(nextPosX-otherNextPosX, 2.0f) + pow(nextPosY-otherNextPosY, 2.0f));
        object1.changeCounter++;
        object2.changeCounter++;

        //if (object1.changeCounter > 0 && object2.changeCounter > 0){
            if(object1.lastHit != object2.getId() || object2.lastHit != object1.getId()){
            if(distance <= 2*radius || distanceNext <= 2*radius){
            
               object1.changeCounter=0;
                object2.changeCounter=0;

                object1.lastHit = object2.getId();
               object2.lastHit = object1.getId();

                glm::vec2 temp = object1.speedVec;
                object1.speedVec = object2.speedVec;
                object2.speedVec = temp;

                float tempspeed = object1.speed;
                object1.speed = object2.speed;
                object2.speed = tempspeed;


                return true;
            }
        }


        return false;
    }

    void adjustForWall(LveGameObject &obj){

                    if (obj.tranform2d.translation.x + obj.radius > 1.0f)
                    {
                        obj.speedVec.x *= -1.0f;

                        obj.tranform2d.translation.x += obj.speedVec.x * obj.speed;
                    }
                    if (obj.tranform2d.translation.x - obj.radius < -1.0f)
                    {
                        obj.speedVec.x *= -1.0f;

                        obj.tranform2d.translation.x += obj.speedVec.x * obj.speed;
                    }
                    if (obj.tranform2d.translation.y + obj.radius > 1.0f)
                    {
                        obj.speedVec.y *= -1.0f;

                        obj.tranform2d.translation.y += obj.speedVec.y * obj.speed;
                    }
                    if (obj.tranform2d.translation.y - obj.radius < -1.0f)
                    {
                        obj.speedVec.y *= -1.0f;

                        obj.tranform2d.translation.y += obj.speedVec.y * obj.speed;
                    }
    }

  

    void FirstApp::renderGameObjects(VkCommandBuffer commandBuffer){
        lvePipeline ->bind(commandBuffer);
  
        
       static std::vector<int> escaped;
    int objectsInFrame=0;
         for(auto& obj: gameObjects){
              
          
             if(checkIfCollidedWithWall(obj)){

               
            //    // obj.speed *= 0.93f;
             }

             for (auto &otherObj : gameObjects)
             {
                 if (obj.getId() != otherObj.getId())
                 {
                     if (checkIfCollidedAndUpdate(obj, otherObj))
                     {

                       //  obj.speed *= 0.99f;
                         //otherObj.speed *= 0.99f;
                     }
                 }
             }
            
                
                    obj.tranform2d.translation.x += obj.speedVec.x * obj.speed;
                    obj.tranform2d.translation.y += obj.speedVec.y * obj.speed;

           
                


                    if (obj.tranform2d.translation.x > 1 || obj.tranform2d.translation.x < -1 ||
                        obj.tranform2d.translation.y > 1 || obj.tranform2d.translation.y < -1)
                    {

                        bool inside = false;
                        if (!escaped.empty())
                        {

                            for (auto &o : escaped)
                            {
                                if (obj.getId() == o)
                                {
                                    inside = true;
                                    break;
                                }
                            }
                        }
                        if (!inside)
                        {
                            escaped.push_back(obj.getId());
                            std::cout << "object [" << obj.getId() << "] escaped, pos: {"
                                      << obj.tranform2d.translation.x << ", "
                                      << obj.tranform2d.translation.y << "}"
                                      << ", speed : " << obj.speed << ", escaped count: " << escaped.size()
                                      << std::endl;
                        }
                    }

        

                    // obj.tranform2d.rotation = glm::mod(obj.tranform2d.rotation + 0.01f, glm::two_pi<float>());

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
    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = lveSwapChain ->acquireNextImage(&imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return;
        }
        if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR){
            throw std::runtime_error("failed to acquire swap chain image!");
        }

        recordCommandBuffer(imageIndex);

        result = lveSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lveWindow.wasWindowResized()){
            lveWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }
        if(result != VK_SUCCESS){
            throw std::runtime_error("failed to present swap chain image!");
        }
    }



}
