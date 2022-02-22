#include "lve_ball_physics.hpp"
#include <iostream>
#include <cmath>
namespace lve
{

    PhysicsSystem::PhysicsSystem(std::vector<LveGameObject>& gameObjects):gameObjects(gameObjects){};

    void PhysicsSystem::update()
    {
        static std::vector<int> escaped;

        for (auto &obj : gameObjects)
        {

            if (checkIfCollidedWithWall(obj))
            {

                //    // obj.speed *= 0.93f;
            }

            for (auto &otherObj : gameObjects)
            {
                if (obj.getId() != otherObj.getId())
                {
                    if (checkIfCollidedAndUpdate(obj, otherObj))
                    {
                    }
                }
            }

            obj.tranform2d.translation.x += obj.speedVec.x * obj.getSpeed();
            obj.tranform2d.translation.y += obj.speedVec.y * obj.getSpeed();


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
                              << ", speed : " << obj.getSpeed() << ", escaped count: " << escaped.size()
                              << std::endl;
                }
            }
        }
    }

    void PhysicsSystem::adjustForWall(LveGameObject &obj){
           if (obj.tranform2d.translation.x + obj.radius > 1.0f)
                    {
                        obj.speedVec.x *= -1.0f;

                        obj.tranform2d.translation.x += obj.speedVec.x * obj.getSpeed();
                    }
                    if (obj.tranform2d.translation.x - obj.radius < -1.0f)
                    {
                        obj.speedVec.x *= -1.0f;

                        obj.tranform2d.translation.x += obj.speedVec.x * obj.getSpeed();
                    }
                    if (obj.tranform2d.translation.y + obj.radius > 1.0f)
                    {
                        obj.speedVec.y *= -1.0f;

                        obj.tranform2d.translation.y += obj.speedVec.y * obj.getSpeed();
                    }
                    if (obj.tranform2d.translation.y - obj.radius < -1.0f)
                    {
                        obj.speedVec.y *= -1.0f;

                        obj.tranform2d.translation.y += obj.speedVec.y * obj.getSpeed();
                    }
    }

    
   bool PhysicsSystem::checkIfCollidedWithWall(LveGameObject& object1){
 //problem je ako ide prebrzo i prođe zid i promjeni mu se na jedan frame brzina, ali onda opet sljedeći
        //frame opet je u zidu i promjeni mu se opet
        static float radius = object1.radius;
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
   

         float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x * object1.getSpeed() ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y * object1.getSpeed() ;


         
        if(posX+radius >= 1.0f  || nextPosX+radius >= 1.0f   ) {
            if(object1.lastWallHit != "rightWall" || object1.lastHit != object1.getId()){
 
            object1.lastWallHit="rightWall";
            object1.lastHit = object1.getId();
             object1.speedVec.x *= -1.0f;
          
            return true;
            }
        }
        else if(posX-radius <= -1.0f ||  nextPosX-radius <= -1.0f){
            if(object1.lastWallHit != "leftWall" || object1.lastHit != object1.getId()){

                object1.lastWallHit="leftWall";
                object1.lastHit = object1.getId();
                object1.speedVec.x *= -1.0f;
          
            return true;
            }
        }
       
        else if(posY +radius >= 1.0f ||  nextPosY+radius >=1.0f){
            if(object1.lastWallHit != "upperWall"|| object1.lastHit != object1.getId()){

            object1.lastWallHit="upperWall";
             object1.lastHit = object1.getId();
           
            object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }
        else if(posY- radius < -1.0f ||  nextPosY-radius <=-1.0f){
            if(object1.lastWallHit != "bottomWall"|| object1.lastHit != object1.getId()){

            object1.lastWallHit="bottomWall";
             object1.lastHit = object1.getId();
           
            object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }

      //}
       

        return false;
   }

   void checkQuadrant(float x, float y, float& angle){
       if(x <0 || y <0){
           if(!(x<0 && y<0)){
               angle = M_PI -angle;
           }
       }
   }

   void PhysicsSystem::updateVecSpeed(LveGameObject &object1, LveGameObject &object2){
       float x1 =object1.speedVec.x;
       float y1 =object1.speedVec.y;
       float x2 =object2.speedVec.x;
       float y2 =object2.speedVec.y;
       float alpha1 = atan(y1/x1);
       checkQuadrant(x1, y1, alpha1);
       float alpha2 = atan(y2/x2);
        checkQuadrant(x2, y2, alpha2);
        std::cout <<"speed angle 1 : "<<alpha1 << "{"<<x1 <<", "<< y1<<"}"<< std::endl;
        std::cout <<"speed angle 2 : "<<alpha2 << "{"<<x2 <<", "<< y2<<"}"<< std::endl;
        float mags = (sqrt(pow(x1, 2.0f)+ pow(y1,2.0f))) * sqrt(pow(x2, 2.0f)+ pow(y2,2.0f));
       float theta = acos((x1*x2 + y1*y2)/ mags);
      
        std::cout <<" angle of impact : "<<theta  << std::endl;

       float vx1 = (((object1.getSpeed()* cos(alpha1-theta))*(object1.mass-object2.mass) + 
                                            2*object2.mass*object2.getSpeed()*cos(alpha2-theta))
                             / (object1.mass + object2.mass))* cos(theta) 
                             + object1.getSpeed()*sin(alpha1-theta)*cos(theta+(M_PI/2));

        float vy1 = (((object1.getSpeed()* cos(alpha1-theta))*(object1.mass-object2.mass) + 
                                            2*object2.mass*object2.getSpeed()*cos(alpha2-theta))
                             / (object1.mass + object2.mass))* sin(theta) 
                             + object1.getSpeed()*sin(alpha1-theta)*sin(theta+(M_PI/2));

        float vx2 = (((object2.getSpeed()* cos(alpha2-theta))*(object2.mass-object1.mass) + 
                                            2*object1.mass*object1.getSpeed()*cos(alpha1-theta))
                             / (object2.mass + object1.mass))* cos(theta) 
                             + object2.getSpeed()*sin(alpha2-theta)*cos(theta+(M_PI/2));

        float vy2 = (((object2.getSpeed()* cos(alpha2-theta))*(object2.mass-object1.mass) + 
                                            2*object1.mass*object1.getSpeed()*cos(alpha1-theta))
                             / (object2.mass + object1.mass))* sin(theta) 
                             + object2.getSpeed()*sin(alpha2-theta)*sin(theta+(M_PI/2));

       object1.speedVec = {vx1, vy1};
       object2.speedVec = {vx2, vy2};
     // glm::vec2 temp = object1.speedVec;
     // object1.speedVec = object2.speedVec;
      //object2.speedVec = temp;

   }
   bool PhysicsSystem::checkIfCollidedAndUpdate(LveGameObject &object1, LveGameObject &object2){
        static float radius = object1.radius;
       
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
        float otherPosX = object2.tranform2d.translation.x;
        float otherPosY = object2.tranform2d.translation.y;

        float distance = sqrt(pow(posX-otherPosX, 2.0f) + pow(posY-otherPosY, 2.0f));


        float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x * object1.getSpeed() ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y * object1.getSpeed();

        float otherNextPosX = object2.tranform2d.translation.x +object2.speedVec.x * object2.getSpeed() ;
        float otherNextPosY = object2.tranform2d.translation.y +object2.speedVec.y * object2.getSpeed();
  
        float distanceNext  = sqrt(pow(nextPosX-otherNextPosX, 2.0f) + pow(nextPosY-otherNextPosY, 2.0f));


        //if (object1.changeCounter > 0 && object2.changeCounter > 0){
            if(object1.lastHit != object2.getId() || object2.lastHit != object1.getId()){
            if(distance <= 2*radius || distanceNext <= 2*radius){
    

                object1.lastHit = object2.getId();
               object2.lastHit = object1.getId();

               updateVecSpeed( object1,  object2);

            


                return true;
            }
        }


        return false;
   }

   }