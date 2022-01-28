#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "rg/Cube.hpp"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>

#include <iostream>
#include <unordered_set>

#define CUBE_VELOCITY 2.5f
#define CUBE_Z_SPAWN_FACTOR -6.0f

void framebuffer_size_callback(GLFWwindow *window, int width, int height);

void mouse_callback(GLFWwindow *window, double xpos, double ypos);

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);

void processInput(GLFWwindow *window);

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

void drawImGui();

void setUpShaderLights(Shader shader);

int comparableFloat(float val);

void resetGame();

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;


float xModelPos = 0.0f;
bool collided = false;

struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff;
    float outerCutOff;
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

//global light
struct DirLight {
    glm::vec3 direction;

    glm::vec3 ambient;
    glm::vec3 specular;
    glm::vec3 diffuse;
};



struct ProgramState {

    ProgramState() { setUpLights();
        clearColor = glm::vec3(0.604575, 0.65498, 0.906863);
        ImGuiEnabled = false;
    }
    glm::vec3 clearColor;
    bool ImGuiEnabled;
    DirLight dirLight;
    SpotLight spotLight;
    PointLight pointLight;

    void SaveToFile(std::string filename);

    void LoadFromFile(std::string filename);

    void setUpLights();
};

void ProgramState::SaveToFile(std::string filename) {
    std::ofstream out(filename);
    out << clearColor.r << '\n'
        << clearColor.g << '\n'
        << clearColor.b << '\n'
        << ImGuiEnabled << '\n'
        << spotLight.position.x << '\n'
        << spotLight.position.y << '\n'
        << spotLight.position.z << '\n'
        << spotLight.direction.x << '\n'
        << spotLight.direction.y << '\n'
        << spotLight.direction.z << '\n'
        << spotLight.ambient.x << '\n'
        << spotLight.ambient.y << '\n'
        << spotLight.ambient.z << '\n'
        << spotLight.diffuse.x << '\n'
        << spotLight.diffuse.y << '\n'
        << spotLight.diffuse.z << '\n'
        << spotLight.specular.x << '\n'
        << spotLight.specular.y << '\n'
        << spotLight.specular.z << '\n'
        << pointLight.position.x << '\n'
        << pointLight.position.y << '\n'
        << pointLight.position.z << '\n'
        << pointLight.direction.x << '\n'
        << pointLight.direction.y << '\n'
        << pointLight.direction.z << '\n'
        << pointLight.ambient.x << '\n'
        << pointLight.ambient.y << '\n'
        << pointLight.ambient.z << '\n'
        << pointLight.diffuse.x << '\n'
        << pointLight.diffuse.y << '\n'
        << pointLight.diffuse.z << '\n'
        << pointLight.specular.x << '\n'
        << pointLight.specular.y << '\n'
        << pointLight.specular.z << '\n'
        << dirLight.direction.x << '\n'
        << dirLight.direction.y << '\n'
        << dirLight.direction.z << '\n'
        << dirLight.ambient.x << '\n'
        << dirLight.ambient.y << '\n'
        << dirLight.ambient.z << '\n'
        << dirLight.diffuse.x << '\n'
        << dirLight.diffuse.y << '\n'
        << dirLight.diffuse.z << '\n'
        << dirLight.specular.x << '\n'
        << dirLight.specular.y << '\n'
        << dirLight.specular.z << '\n';
}

void ProgramState::LoadFromFile(std::string filename) {
    std::ifstream in(filename);
    if (in) {
        in >> clearColor.r
           >> clearColor.g
           >> clearColor.b
           >> ImGuiEnabled
           >> spotLight.position.x
           >> spotLight.position.y
           >> spotLight.position.z
           >> spotLight.direction.x
           >> spotLight.direction.y
           >> spotLight.direction.z
           >> spotLight.ambient.x
           >> spotLight.ambient.y
           >> spotLight.ambient.z
           >> spotLight.diffuse.x
           >> spotLight.diffuse.y
           >> spotLight.diffuse.z
           >> spotLight.specular.x
           >> spotLight.specular.y
           >> spotLight.specular.z
           >> pointLight.position.x
           >> pointLight.position.y
           >> pointLight.position.z
           >> pointLight.direction.x
           >> pointLight.direction.y
           >> pointLight.direction.z
           >> pointLight.ambient.x
           >> pointLight.ambient.y
           >> pointLight.ambient.z
           >> pointLight.diffuse.x
           >> pointLight.diffuse.y
           >> pointLight.diffuse.z
           >> pointLight.specular.x
           >> pointLight.specular.y
           >> pointLight.specular.z
           >> dirLight.direction.x
           >> dirLight.direction.y
           >> dirLight.direction.z
           >> dirLight.ambient.x
           >> dirLight.ambient.y
           >> dirLight.ambient.z
           >> dirLight.diffuse.x
           >> dirLight.diffuse.y
           >> dirLight.diffuse.z
           >> dirLight.specular.x
           >> dirLight.specular.y
           >> dirLight.specular.z;
    }
}

void ProgramState::setUpLights()
{
    dirLight.direction = glm::vec3(0.0f,-10.0f, 0.0f);
    dirLight.ambient = glm::vec3(0.15);
    dirLight.diffuse = glm::vec3(0.4);
    dirLight.specular = glm::vec3(0.5);

    spotLight.position = glm::vec3(camera.Position);
    spotLight.direction = glm::vec3(0.0f, -1.0f, -3);
    spotLight.ambient = glm::vec3( 0.04f, 0.04f, 0.04f);
    spotLight.diffuse = glm::vec3(0.4f, 0.85f, 0.4f);
    spotLight.specular = glm::vec3(0.4f, 0.85f, 0.4f);
    spotLight.constant = 1.0f;
    spotLight.linear = 0.09;
    spotLight.quadratic = 0.032;
    spotLight.cutOff = glm::cos(glm::radians(5.0f));
    spotLight.outerCutOff = glm::cos(glm::radians(7.5f));

    pointLight.position = glm::vec3(camera.Position);
    pointLight.direction = glm::vec3(0.0f, -1.0f, -3);
    pointLight.ambient = glm::vec3( 0.04f, 0.04f, 0.04f);
    pointLight.diffuse = glm::vec3(0.4f, 0.85f, 0.4f);
    pointLight.specular = glm::vec3(0.4f, 0.85f, 0.4f);
    pointLight.constant = 1.0f;
    pointLight.linear = 0.09;
    pointLight.quadratic = 0.032;

}

ProgramState *programState;
std::unordered_set<Cube* > cubes;

DirLight dirLight;
SpotLight spotLight;

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "matf_rg_game_omega", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    programState = new ProgramState();
    programState->LoadFromFile("resources/program_state.txt");
    if (programState->ImGuiEnabled) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    // Init Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    (void) io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Shader planeShader("resources/shaders/plane.vs", "resources/shaders/plane.fs");
    Shader cubeShader("resources/shaders/cube.vs", "resources/shaders/cube.fs");
    Shader modelShader("resources/shaders/model.vs", "resources/shaders/model.fs");

    Model objectModel("resources/objects/gazelle_model/10020_Gazelle_v04.obj");

    float planeVertices[] = {
            //positions - 3f                   //normals - 3f                      //texture coords - 2f
            1.0f,  0.0f, 1.0f, 0.0f, 1.0f, 0.0f,       1.0f, 0.0f,
            1.0f,  0.0f, -1.0f, 0.0f, 1.0f, 0.0f,  1.0f, 1.0f,
            -1.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,0.0f, 1.0f,
            -1.0f, 0.0f,1.0f, 0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
    };

    unsigned int planeIndices[] = {
            0, 1, 3,
            1, 2, 3
    };

    float cubeVertices[] = {
            //back face
            // positions                       // normals                         // texture coords
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
            -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

            //front face
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,

            //left face
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

            //right face
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

            //bottom face
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
            0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
            -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

            //top face
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
            0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
            -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
            -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    //plane data
    unsigned int planeVAO, planeVBO, planeEBO;

    //gen buffers
    glGenVertexArrays(1, &planeVAO);
    glGenBuffers(1, &planeVBO);
    glGenBuffers(1, &planeEBO);

    //bind buffers - first VAO then set buffer data and then set attributes
    glBindVertexArray(planeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(planeIndices), planeIndices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    //cube data
    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // tell stb_image.h to flip loaded texture's on the y-axis (before loading model).
    stbi_set_flip_vertically_on_load(true);

    //Gen Textures
    unsigned int planeTexture, cubeTexture;
    glGenTextures(1, &planeTexture);
    //Gen plane texture

    glBindTexture(GL_TEXTURE_2D, planeTexture);
    //Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    unsigned char* data = stbi_load("resources/textures/plane.JPG", &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glGenTextures(1, &cubeTexture);
    //Gen plane texture

    glBindTexture(GL_TEXTURE_2D, cubeTexture);
    //Wrapping
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //Filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    data = stbi_load("resources/textures/container.jpg", &width, &height, &nrChannels, 0);

    if(data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    Cube* firstCube = new Cube();
    cubes.insert(firstCube);
    cubes.insert(firstCube->additionalXLaneCube());
    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(programState->clearColor.r, programState->clearColor.g, programState->clearColor.b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindVertexArray(planeVAO);
        planeShader.use();
        planeShader.setInt("planeTexture", 0);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, planeTexture);

        glm::mat4 view = camera.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 100.0f);
        planeShader.setMat4("projection", projection);
        planeShader.setMat4("view", view);
        setUpShaderLights(planeShader);

        for(unsigned int i = 0; i< 10; i++){
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f,0.0f,-2.0f * i - 1.0f));
            planeShader.setMat4("model", model);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        glBindVertexArray(cubeVAO);
        cubeShader.use();
        cubeShader.setInt("cubeTexture", 1);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);


        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CW);

        cubeShader.setMat4("view", view);
        cubeShader.setMat4("projection", projection);
        setUpShaderLights(cubeShader);

        float xPos = 0.0f;
        float deltaZ = 0.0f;
        for(auto cubeIt = cubes.begin(); cubeIt != cubes.end();){
            Cube* cube = *cubeIt;
            xPos = cube->xPos();
            float zPos = cube->zPos();

            float zPosition = zPos + deltaTime * CUBE_VELOCITY;

            if(deltaZ - zPosition >= 0.01f) {
                deltaZ = zPosition;
            }

            if(zPosition + 0.3f >= 0.01f){
                cubeIt = cubes.erase(cubeIt);
                continue;
            }

            if(comparableFloat(zPosition) >= comparableFloat(-1.2f) && comparableFloat(xModelPos) == comparableFloat(xPos)){
                cubes.clear();
                collided = true;
                break;
            }


            glm::mat4 model = cube->translate(xPos, 0.0f, zPosition);
            cubeShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++cubeIt;
        }
        if((deltaZ > CUBE_Z_SPAWN_FACTOR) && !collided) {
            Cube* newCube = new Cube();
            cubes.insert(newCube);
            cubes.insert(newCube->additionalXLaneCube());
        }

        modelShader.use();

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(xModelPos, 0.0f, -0.7f));
        model = glm::rotate(model, glm::radians(270.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(-180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::scale(model, glm::vec3(0.006f));

        modelShader.setMat4("projection", projection);
        modelShader.setMat4("view", view);
        modelShader.setMat4("model", model);
        setUpShaderLights(modelShader);


        objectModel.Draw(modelShader);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        if(programState->ImGuiEnabled){
            drawImGui();
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    programState->SaveToFile("resources/program_state.txt");
    cubes.clear();
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods){
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && xModelPos > -0.66f){
        xModelPos -= 0.66f;
    }

    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && xModelPos < 0.66f){
        xModelPos += 0.66f;
    }

    if(key == GLFW_KEY_R && action == GLFW_PRESS){
        resetGame();
    }

    if(key == GLFW_KEY_F1 && action == GLFW_PRESS){
        programState->ImGuiEnabled = !programState->ImGuiEnabled;
    }
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

}

void scroll_callback(GLFWwindow *window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void setUpShaderLights(Shader shader){
    shader.setVec3("dirLight.direction",programState->dirLight.direction);
    shader.setVec3("dirLight.ambient", programState->dirLight.ambient);
    shader.setVec3("dirLight.diffuse", programState->dirLight.diffuse);
    shader.setVec3("dirLight.specular", programState->dirLight.specular);

    shader.setVec3("spotLight.position",programState->spotLight.position);
    shader.setVec3("spotLight.direction",programState->spotLight.direction);
    shader.setVec3("spotLight.ambient", programState->spotLight.ambient);
    shader.setVec3("spotLight.diffuse", programState->spotLight.diffuse);
    shader.setVec3("spotLight.specular", programState->spotLight.specular);
    shader.setFloat("spotLight.constant", programState->spotLight.constant);
    shader.setFloat("spotLight.linear", programState->spotLight.linear);
    shader.setFloat("spotLight.quadratic", programState->spotLight.quadratic);
    shader.setFloat("spotLight.cutOff", programState->spotLight.cutOff);
    shader.setFloat("spotLight.outerCutOff", programState->spotLight.outerCutOff);

    shader.setVec3("pointLight.position",programState->pointLight.position);
    shader.setVec3("pointLight.direction",programState->pointLight.direction);
    shader.setVec3("pointLight.ambient", programState->pointLight.ambient);
    shader.setVec3("pointLight.diffuse", programState->pointLight.diffuse);
    shader.setVec3("pointLight.specular", programState->pointLight.specular);
    shader.setFloat("pointLight.constant", programState->pointLight.constant);
    shader.setFloat("pointLight.linear", programState->pointLight.linear);
    shader.setFloat("pointLight.quadratic", programState->pointLight.quadratic);

    shader.setVec3("viewPos", camera.Position);
}

void drawImGui()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    {
        ImGui::Begin("Camera info");
        const Camera& c = camera;
        ImGui::Text("Camera position: (%f, %f, %f)", c.Position.x, c.Position.y, c.Position.z);
        ImGui::Text("(Yaw, Pitch): (%f, %f)", c.Yaw, c.Pitch);
        ImGui::Text("Camera front: (%f, %f, %f)", c.Front.x, c.Front.y, c.Front.z);
//        ImGui::Checkbox("Camera mouse update", &programState->CameraMouseMovementUpdateEnabled);
        ImGui::End();
    }

    {
        ImGui::Begin("Settings");
        ImGui::ColorEdit3("Background color", (float *) &programState->clearColor);

        ImGui::DragFloat3("dirLight.direction", (float*) &(programState->dirLight.direction));
        ImGui::DragFloat3("dirLight.ambient", (float*) &(programState->dirLight.ambient), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("dirLight.diffuse", (float*) &(programState->dirLight.diffuse), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("dirLight.specular", (float*) &(programState->dirLight.specular), 0.05, 0.0, 1.0);

        ImGui::DragFloat3("spotlight.position", (float*)&(programState->spotLight.position));
        ImGui::DragFloat3("spotlight.direction", (float*)&(programState->spotLight.direction));
        ImGui::DragFloat3("spotLight.ambient", (float*) &(programState->spotLight.ambient), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("spotLight.diffuse", (float*) &(programState->spotLight.diffuse), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("spotLight.specular", (float*) &(programState->spotLight.specular), 0.05, 0.0, 1.0);
        ImGui::DragFloat("spotLight.constant", (float*) &programState->spotLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("spotLight.linear", (float*) &programState->spotLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("spotLight.quadratic", (float *) &programState->spotLight.quadratic, 0.05, 0.0, 1.0);

        ImGui::DragFloat3("pointLight.position", (float*)&(programState->pointLight.position));
        ImGui::DragFloat3("pointLight.direction", (float*)&(programState->pointLight.direction));
        ImGui::DragFloat3("pointLight.ambient", (float*) &(programState->pointLight.ambient), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("pointLight.diffuse", (float*) &(programState->pointLight.diffuse), 0.05, 0.0, 1.0);
        ImGui::DragFloat3("pointLight.specular", (float*) &(programState->pointLight.specular), 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.constant", (float*) &programState->pointLight.constant, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.linear", (float*) &programState->pointLight.linear, 0.05, 0.0, 1.0);
        ImGui::DragFloat("pointLight.quadratic", (float *) &programState->pointLight.quadratic, 0.05, 0.0, 1.0);

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

int comparableFloat(float val){
    return (int)(val * 100);
}

void resetGame(){
    collided = false;
}