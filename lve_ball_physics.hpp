// #include "lve_game_object.hpp"

// namespace lve{

//     class PhysicsSystem{
//         public:
//             PhysicsSystem(std::vector<LveGameObject>& gameObjects);

//             void update();
//             void calcMinMaxSpeed();


//         private:
//             std::vector<LveGameObject>& gameObjects;
//             float minSpeed{10.0}, maxSpeed{-1.0};


//           //  void adjustForWall(LveGameObject &obj);
//             bool checkIfCollidedWithWall(LveGameObject& object1);
//             bool checkIfCollidedAndUpdate(LveGameObject &object1, LveGameObject &object2);
//             void updateVecSpeed(LveGameObject &object1, LveGameObject &object2);
//             void updateSpeedForWallCollision(LveGameObject& obj, std::string wall);
//             void checkQuadrant(float x, float y, float& angle);
//             glm::vec3 getColorFromSpeed(LveGameObject& obj);
//     };



// }