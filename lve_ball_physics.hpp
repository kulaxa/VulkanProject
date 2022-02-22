#include "lve_game_object.hpp"

namespace lve{

    class PhysicsSystem{
        public:
            PhysicsSystem(std::vector<LveGameObject>& gameObjects);

            void update();

        private:
            std::vector<LveGameObject>& gameObjects;

            void adjustForWall(LveGameObject &obj);
            bool checkIfCollidedWithWall(LveGameObject& object1);
            bool checkIfCollidedAndUpdate(LveGameObject &object1, LveGameObject &object2);
            void updateVecSpeed(LveGameObject &object1, LveGameObject &object2);
    };



}