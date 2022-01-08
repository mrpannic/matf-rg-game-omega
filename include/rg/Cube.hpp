#ifndef MATF_RG_GAME_OMEGA_CUBE_HPP
#define MATF_RG_GAME_OMEGA_CUBE_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ctime>
#include <cstdlib>
#include <iostream>

#define Z_DEFAULT -10.0f


class Cube {
public:
    Cube(){
        float lane = randomLane();
        setModel(lane, 0.0f, Z_DEFAULT);
    };

    Cube(float xCurrent){
        setModel(x, 0.0f, Z_DEFAULT);
    };

    Cube(float x, float y, float z) {
        setModel(x, y, z);
    };

    glm::mat4 translate(float x, float y, float z){
        setModel(x, y, z);
        return model;
    }

    glm::mat4 getModel(){
        return model;
    }

    float xPos(){
        return x;
    }

    float zPos(){
        return z;
    }
private:

    // tells us if the object is in the middle, left or right
    float x;
    // tells us current position of the cube
    float z;
    glm::mat4 model;

    void setModel(float x, float y, float z) {
        this->x = x;
        this->z = z;
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(x, y, z));
        model = glm::scale(model, glm::vec3(0.4f, 0.4f,0.4f));
    }

    float spawnOnDifferentLane(float xCurrent){
        float xNew = randomLane();
        while(xCurrent - xNew <= 0.01f){
            xNew = randomLane();
        }
        return xNew;
    }

    float randomLane(){
        const float lanes[] = {-0.66, 0.0, 0.66};
        srand(time(nullptr));
        int lane = rand() % 3;
        return lanes[lane];
    }
};
#endif //MATF_RG_GAME_OMEGA_CUBE_HPP
