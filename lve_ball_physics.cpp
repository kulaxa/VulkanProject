#include "lve_ball_physics.hpp"
#include <iostream>
#include <cmath>
namespace lve
{

    PhysicsSystem::PhysicsSystem(std::vector<LveGameObject>& gameObjects):gameObjects(gameObjects){};

    void PhysicsSystem::update()
    {
        static std::vector<int> escaped;
        calcMinMaxSpeed();
       
     //   std::cout << "min speed: "<<minSpeed << ", max speed: "<<maxSpeed << std::endl;
        
        for (auto &obj : gameObjects)
        {
            obj.speedVec *= 0.999f;        
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

            obj.tranform2d.translation.x += obj.speedVec.x;
            obj.tranform2d.translation.y += obj.speedVec.y;
            obj.color = getColorFromSpeed(obj);


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

    // void PhysicsSystem::adjustForWall(LveGameObject &obj){
    //        if (obj.tranform2d.translation.x + obj.radius > 1.0f)
    //                 {
    //                     obj.speedVec.x *= -1.0f;

    //                     obj.tranform2d.translation.x += obj.speedVec.x ;
    //                 }
    //                 if (obj.tranform2d.translation.x - obj.radius < -1.0f)
    //                 {
    //                     obj.speedVec.x *= -1.0f;

    //                     obj.tranform2d.translation.x += obj.speedVec.x ;
    //                 }
    //                 if (obj.tranform2d.translation.y + obj.radius > 1.0f)
    //                 {
    //                     obj.speedVec.y *= -1.0f;

    //                     obj.tranform2d.translation.y += obj.speedVec.y ;
    //                 }
    //                 if (obj.tranform2d.translation.y - obj.radius < -1.0f)
    //                 {
    //                     obj.speedVec.y *= -1.0f;

    //                     obj.tranform2d.translation.y += obj.speedVec.y ;
    //                 }
    // }

    void PhysicsSystem::updateSpeedForWallCollision(LveGameObject& obj, std::string wall){
        float theta;

        float x1 =obj.speedVec.x;
       float y1 =obj.speedVec.y;
     
       float alpha = atan(y1/x1);
       checkQuadrant(x1, y1, alpha);
        if(wall=="rightWall" || wall =="leftWall"){
          theta= 0;
        }else if( wall=="upperWall" || wall=="bottomWall"){
            
              theta = M_PI /2;
        }
        else{
            throw std::runtime_error("That wall doesn't exist : " +wall);
        }


        //std::cout <<"alpha: "<<alpha<< ", theta: "<< theta <<std::endl;
        float vx1 = (((obj.getSpeed()* cos(alpha-theta))*(obj.mass-10000.0f) )
                             / (obj.mass + 10000.0f))* cos(theta) 
                             + obj.getSpeed()*sin(alpha-theta)*cos(theta+(M_PI/2));

        float vy1 = (((obj.getSpeed()* cos(alpha-theta))*(obj.mass-10000.0f)  )
                             / (obj.mass + 10000.0f))* sin(theta) 
                             + obj.getSpeed()*sin(alpha-theta)*sin(theta+(M_PI/2));
       //  std::cout <<"speed before : {" <<x1<< ", "<<y1<< "}, speed after: {"<<vx1<< ", "<<vy1<<"} \n";
        obj.speedVec = {vx1, vy1};
    }
    
   bool PhysicsSystem::checkIfCollidedWithWall(LveGameObject& object1){
 //problem je ako ide prebrzo i prođe zid i promjeni mu se na jedan frame brzina, ali onda opet sljedeći
        //frame opet je u zidu i promjeni mu se opet
         float radius = object1.radius;
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
  

         float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y  ;


         //posX+radius >= 1.0f  ||
        if( nextPosX+radius >= 1.0f   ) {
            if(object1.lastWallHit != "rightWall" || object1.lastHit != object1.getId()){
 
            object1.lastWallHit="rightWall";
            object1.lastHit = object1.getId();
             //object1.speedVec.x *= -1.0f;
            updateSpeedForWallCollision(object1, "rightWall");

          
            return true;
            }
        }//posX-radius <= -1.0f
        else if(   nextPosX-radius <= -1.0f){
            if(object1.lastWallHit != "leftWall" || object1.lastHit != object1.getId()){

                object1.lastWallHit="leftWall";
                object1.lastHit = object1.getId();
               // object1.speedVec.x *= -1.0f;
            updateSpeedForWallCollision(object1, "leftWall");
            return true;
            }
        }
       //posY +radius >= 1.0f || 
        else if( nextPosY+radius >=1.0f){
            if(object1.lastWallHit != "upperWall"|| object1.lastHit != object1.getId()){

            object1.lastWallHit="upperWall";
             object1.lastHit = object1.getId();
            updateSpeedForWallCollision(object1, "upperWall");
          //  object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }//posY- radius < -1.0f ||
        else if(  nextPosY-radius <=-1.0f){
            if(object1.lastWallHit != "bottomWall"|| object1.lastHit != object1.getId()){

            object1.lastWallHit="bottomWall";
             object1.lastHit = object1.getId();
            updateSpeedForWallCollision(object1, "bottomWall");
           // object1.speedVec.y *= -1.0f;
              return true;
              
            }
        }

      //}
       

        return false;
   }

   void PhysicsSystem::checkQuadrant(float x, float y, float& angle){
    //    if(x <0 || y >0){
    //        if(!(x<0 && y>0)){
    //            angle = M_PI -angle;
    //        }
    //    }

    if(x <0){
        angle = M_PI +angle;
    }
   }

float roundoff(float value, unsigned char prec)
{
  float pow_10 = pow(10.0f, (float)prec);
  return round(value * pow_10) / pow_10;
}

    float calcAngleOfImpact(std::pair<float,float> obj1, std::pair<float,float> obj2){
       
        float x = obj2.first - obj1.first;
        float y = obj2.second - obj1.second;
       // std::cout <<"x: "<<x << ", y : "<<y<<"\n";
        if(fabs(x) < 0.00001f && y >= 0 ){
            return M_PI/2;
        }
        else if(fabs(x) < 0.00001f && y<0){
            return (3*M_PI)/2;
        }
        return atan(y/x);
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
      //  std::cout <<"speed angle 1 : "<<alpha1 << "{"<<x1 <<", "<< y1<<"}"<< std::endl;
      //  std::cout <<"speed angle 2 : "<<alpha2 << "{"<<x2 <<", "<< y2<<"}"<< std::endl;
       
   
      float theta = calcAngleOfImpact({object1.tranform2d.translation.x,object1.tranform2d.translation.y },
       {object2.tranform2d.translation.x,object2.tranform2d.translation.y});
       // std::cout <<" angle of impact : "<<theta  << std::endl;

      //  std::cout <<"object 1 speed "<< object1.getSpeed() << ", object 2 speed "<<object2.getSpeed() <<"\n";
       // std::cout << "object 1  mass : "<<object1.mass << ", object 2 mass : " << object2.mass << "\n";
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

   // object1.speedVec = {roundoff(vx1, 5),roundoff(vy1, 5) };
    //object2.speedVec = {roundoff(vx2, 5), roundoff(vy2, 5)};


    //std::cout << "vec 1: {"<<vx1 << ", "<<vy1<< "} , it would be: {"<<object2.speedVec.x <<", "<<object2.speedVec.y << "}"<< std::endl;
   //  std::cout << "vec 2: {"<<vx2 << ", "<<vy2<< "}, it would be: {"<<object1.speedVec.x <<", "<<object1.speedVec.y << "}"<< std::endl;


     object1.speedVec = {vx1,vy1};
   object2.speedVec = {vx2, vy2};

    
//    glm::vec2 temp = object1.speedVec;
//    object1.speedVec = object2.speedVec;
//     object2.speedVec = temp;
        
   }
   bool PhysicsSystem::checkIfCollidedAndUpdate(LveGameObject &object1, LveGameObject &object2){
         float radiusFirst = object1.radius;
         float radiusSecond = object2.radius;
       
        float posX = object1.tranform2d.translation.x;
        float posY = object1.tranform2d.translation.y;
        float otherPosX = object2.tranform2d.translation.x;
        float otherPosY = object2.tranform2d.translation.y;

        float distance = sqrt(pow(posX-otherPosX, 2.0f) + pow(posY-otherPosY, 2.0f));


        float nextPosX = object1.tranform2d.translation.x +object1.speedVec.x ;
        float nextPosY = object1.tranform2d.translation.y +object1.speedVec.y ;

        float otherNextPosX = object2.tranform2d.translation.x +object2.speedVec.x  ;
        float otherNextPosY = object2.tranform2d.translation.y +object2.speedVec.y ;
  
        float distanceNext  = sqrt(pow(nextPosX-otherNextPosX, 2.0f) + pow(nextPosY-otherNextPosY, 2.0f));


        //if (object1.changeCounter > 0 && object2.changeCounter > 0){
            //distance <= 2*radius ||
            if(object1.lastHit != object2.getId() || object2.lastHit != object1.getId()){
            if( distanceNext <= radiusFirst + radiusSecond){
    

                object1.lastHit = object2.getId();
               object2.lastHit = object1.getId();

               updateVecSpeed(object1,object2);

            


                return true;
            }
        }


        return false;
   }


    glm::vec3 PhysicsSystem::getColorFromSpeed(LveGameObject& obj){
        glm::vec3 result;
     
        

        // if(maxSpeed - obj.getSpeed()  < obj.getSpeed() - minSpeed){
        //     result.r = (obj.getSpeed() / maxSpeed);
        //     result.b = 0.1f;
        // }
        // else{
        //         result.r = 0.1f;
        //        result.b = (obj.getSpeed() / minSpeed);
        // }

        result.r = (obj.getSpeed() / maxSpeed);
         result.b = (obj.getSpeed() / minSpeed);
        
        return result;
    }

    void PhysicsSystem::calcMinMaxSpeed(){
      //  minSpeed = 100.f;
        //maxSpeed = -1.0f;
        for(auto& obj: gameObjects){
           
            if(obj.getSpeed() > maxSpeed){
                maxSpeed = obj.getSpeed();
            }
            if(obj.getSpeed() < minSpeed){
                minSpeed = obj.getSpeed();
            }
        }

       
    }

   }