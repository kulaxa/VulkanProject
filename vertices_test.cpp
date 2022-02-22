#include "first_app.hpp"
#include <stdexcept>
#include <iostream>

namespace  lve
{
    
     void FirstApp::makeVertices(int num, std::vector<LveModel::Vertex>* vertices){
        float x1, x2, x3, y1, y2, y3;
        float xprev=-1.0f, yprev = -1.0f;
        for(int j=1; j<=num; j++){
            if(j%2==0){

            }
        for(int i=1; i<= num; i++){
            x1 =xprev +(2.0f/num);
            x2 = xprev;
            x3=(x1+ x2)/2;
            xprev = x1;
            y1 = yprev;
            y2 = y1;
            y3 = yprev + (2.0f/num);
            if(j%2==0){
               vertices ->push_back({{x1,y3}});
              vertices ->push_back({{x2,y3}});
              vertices ->push_back({{x3,y1}});

            }
            else{
           vertices ->push_back({{x1,y1}});
              vertices ->push_back({{x2,y2}});
              vertices ->push_back({{x3,y3}});

            }
              

        }   
        xprev=-1.0f;
        yprev += (2.0f/num);

        }
    }

    uint32_t FirstApp::makeCircle(LveModel::Vertex center, float radius, float angle, std::vector<LveModel::Vertex> *vertices)
    {
        // if (radius < 0 || radius > 1)
        //     throw std::runtime_error("Wrong radius");
        if (angle < 0 || angle > 2 * M_PI)
            throw std::runtime_error("Wrong angle");
        float xc = center.position[0];
        float yc = center.position[1];
        float xprev = xc + radius;
        float yprev = yc;
        float times = (2 * M_PI) / angle;
        float color = 1.0f;
        glm::vec3 colorVec = {color,color,color};
        for (int i = 0; i < times; i++)
        {

            float x1 = sqrt(pow(radius, 2.0f) / (1 + pow(tan(i * angle), 2.0f)));
            if (i * angle > M_PI / 2.0f && i * angle < M_PI * (3.0f / 2))
            {
                x1 *= -1.0f;

            }
            float y1 = -1.0f * tan(i * angle) * x1;
            if(i <= times/2)
            color -= (1/(times+2));
            else 
            color += (1/(times+2));
            colorVec = {color,0.5f, 0.3f};
            vertices->push_back({{xc, yc},colorVec});
            vertices->push_back({{x1 + xc, y1 + yc},colorVec});
            if (i == 0)
            {
                vertices->push_back({{radius + xc, yc},colorVec});
   
            }
            else
            {
                vertices->push_back({{xprev + xc, yprev + yc},colorVec});
             
            }
          
            xprev = x1;
            yprev = y1;
        }

        vertices->push_back({{xc, yc}, colorVec});
        vertices->push_back({{xprev + xc, yprev + yc},colorVec});
        vertices->push_back({{xc + radius, yc},colorVec});
        return times+1;
    }

   
    void recFillVert(LveModel::Vertex point1, LveModel::Vertex point2,std::vector<LveModel::Vertex> *vertices, int depth , int currentDepth){
        if(currentDepth <= depth){
        float x1 = (point2.position[0] - point1.position[0])/3 + point1.position[0];
        float x2 = 2*((point2.position[0] - point1.position[0])/3) + point1.position[0];
        float y1  =(point2.position[1] - point1.position[1])/3 + point1.position[1];
        float y2 =  2*((point2.position[1] - point1.position[1])/3) + point1.position[1];
        
        float L = sqrt(pow(x2-x1, 2.0f) + pow(y2-y1, 2.0f));
        float angle = atan((y2-y1)/(x2-x1)) + 1.0472;

        float x = x1 + L*cos(angle);
        float y = y1 + L*sin(angle);

        vertices ->push_back({{x1,y1}});
        vertices ->push_back({{x2, y2}});
        vertices ->push_back({{x,y}});
        std::cout << "("<<x1 << ", "<< y1<< ")"<<std::endl;
        std::cout << "("<<x2 << ", "<< y2<< ")"<<std::endl;
        std::cout << "("<<x << ", "<< y<< ")"<<std::endl;

        recFillVert({{x1,y1}}, {{x,y}}, vertices, depth, currentDepth+1);
        recFillVert({{x2,y2}}, {{x,y}}, vertices, depth, currentDepth+1);
        }
        else return;
    }
     void FirstApp::FillVert(LveModel::Vertex center, float size,std::vector<LveModel::Vertex> *vertices, int depth){
       recFillVert(center, {{center.position[0] +size, center.position[1]}}, vertices, depth, 0);
    }

} // namespace  lve
