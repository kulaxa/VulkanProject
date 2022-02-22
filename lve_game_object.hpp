#pragma once
#include "lve_model.hpp"
#include <memory>

namespace lve{

    struct Tranform2dComponent{
        glm::vec2 translation{};
        glm::vec2 scale{1.f, 1.f};
 
        bool smjer =false;
        float rotation;

        glm::mat2 mat2() {
            const float s = glm::sin(rotation);
            const float c= glm::cos(rotation);
            glm::mat2 rotMatrix{{c,s}, {-s,c}};
            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return  rotMatrix*scaleMat;    
        }
            };

    class LveGameObject{

        public:
        using id_t = unsigned int;
        glm::vec2 speedVec{0.0f, 0.0f};
        float speed =0.0f;
       // bool changingDirection =false;
        int changeCounter =0;
        int lastHit=id;
        std::string lastWallHit="null";
        float radius;


        static LveGameObject createGameObject(){
            static id_t  currentId =0;
            return LveGameObject{currentId++};
        }

        LveGameObject (const LveGameObject &) = delete;
        LveGameObject &operator=(const LveGameObject &) = delete;
        LveGameObject(LveGameObject &&) = default;
        LveGameObject &operator=(LveGameObject &&) = default;

        std::shared_ptr<LveModel> model{};
        glm::vec3 color{}; 
        Tranform2dComponent tranform2d{};

        id_t getId(){return id;}
        private:
        LveGameObject(id_t objId) :id{objId}{};

        id_t id;
    };
}