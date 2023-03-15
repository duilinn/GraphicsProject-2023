#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "camera.h"
#include "shader_s.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "model.h"

#include <iostream>

//adapted from https://learnopengl.com/Getting-started/Camera

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int useWireframe = 0;
int displayGrayscale = 0;

// camera
Camera camera(glm::vec3(0.0f, 1.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(5.0f, 1.0f, -2.0f);
glm::vec3 colouredLightPos(-2.0f, 2.0f, -2.0f);

glm::vec3 snowman1Pos(0.0f, 0.0f, 0.0f);
glm::vec3 snowman1Direction(0.0f, 0.0f, 0.0f);
float snowman1DirectionRadians = 0;

glm::vec3 snowmanStartPositions[] = {
      glm::vec3(-8.0f, 0.0f, -16.0f),
      glm::vec3(8.0f, 0.0f, -16.0f),
      glm::vec3(-8.0f, 0.0f, 16.0f),
      glm::vec3(0.0f, 0.0f, 16.0f),
      glm::vec3(8.0f, 0.0f, 16.0f),
};

glm::vec3 snowmanPositions[] = {
      glm::vec3(-8.0f, 0.0f, -16.0f),
      glm::vec3(8.0f, 0.0f, -16.0f),
      glm::vec3(-8.0f, 0.0f, 16.0f),
      glm::vec3(0.0f, 0.0f, 16.0f),
      glm::vec3(8.0f, 0.0f, 16.0f),
};

//material data adapted from http://devernay.free.fr/cours/opengl/materials.html

float snowmanMaterials[5][10] = {
    {0.1f, 0.18725f, 0.1745f, 0.396f, 0.74151f, 0.69102f, 0.297254f, 0.30829f, 0.306678f, 0.1f}, //turquoise
    {0.329412f, 0.223529f, 0.027451f, 0.780392f, 0.568627f, 0.113725f, 0.992157f,	0.941176f, 0.807843f, 0.21794872f}, //brass
    {0.19225f, 0.19225f, 0.19225f, 0.50754f, 0.50754f, 0.50754f, 0.508273f,	0.508273f, 0.508273f, 0.4f}, //silver
    {0.0f, 0.0f, 0.0f, 0.55f, 0.55f, 0.55f, 0.70f, 0.70f, 0.70f, 0.0025}, //white plastic
    {0.25f, 0.20725f, 0.20725f, 1.0f, 0.829f, 0.829f, 0.296648f, 0.296648f, 0.296648f, 0.988f} //pearl
};

//adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps

unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

float skyboxVertices[] = {
    // positions          
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

int main()
{

    //adapted from https://learnopengl.com/Getting-started/Hello-Window

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Winter Wonderland", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    stbi_set_flip_vertically_on_load(true);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps


    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    vector<std::string> faces
    {
        "right.jpg",
            "left.jpg",
            "bottom.jpg",
            "top.jpg",
            "front.jpg",
            "back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    //adapted from https://learnopengl.com/Getting-started/Shaders

    Shader ourShader("shader.vs", "shader.fs");
    Shader lightShader("lightshader.vs", "lightshader.fs");
    Shader colouredLightShader("colouredlightshader.vs", "colouredlightshader.fs");
    Shader skyboxShader("skyboxshader.vs", "skyboxshader.fs");
    Shader heightMapShader("heightMapShader.vs", "heightMapShader.fs");

    //adapted from https://learnopengl.com/Model-Loading/Model

    Model ourModel("C:/Users/david/source/repos/GraphicsProject/objects/snowman/snowman.obj");
    Model stick1("C:/Users/david/source/repos/GraphicsProject/objects/snowman/stick.obj");

    Model lightball("C:/Users/david/source/repos/GraphicsProject/objects/snowman/stick.obj");

    Model tree("C:/Users/david/source/repos/GraphicsProject/objects/tree/tree.obj");


    float arm_swing = 0.0f;
    bool arm_swinging_forwards = true;

   

    //adapted from https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

    stbi_set_flip_vertically_on_load(true);
    int width, height, nrChannels;
    unsigned char* data = stbi_load("heightmap.png", &width, &height, &nrChannels, 0);
    if (data)
    {
        std::cout << "Loaded heightmap of size " << height << " x " << width << std::endl;
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }



    std::vector<float> vertices;
    
    float yScale = 8.0f / 256.0f, yShift = 5.0f;

    int rez = 1;
    unsigned bytePerPixel = nrChannels;
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            unsigned char* pixelOffset = data + (j + width * i) * bytePerPixel;
            unsigned char y = pixelOffset[0];

            // vertex
            vertices.push_back((-height / 2.0f + height * i / (float)height) / 2.0f);
            vertices.push_back((int)y * yScale - yShift);
            vertices.push_back((-width / 2.0f + width * j / (float)width) / 2.0f);
        }
    }
    std::cout << "Loaded " << vertices.size() / 3 << " vertices" << std::endl;
    stbi_image_free(data);

    std::vector<unsigned> indices;
    for (unsigned i = 0; i < height - 1; i += rez)
    {
        for (unsigned j = 0; j < width; j += rez)
        {
            for (unsigned k = 0; k < 2; k++)
            {
                indices.push_back(j + width * (i + k * rez));
            }
        }
    }
    std::cout << "Loaded " << indices.size() << " indices" << std::endl;

    const int numStrips = (height - 1) / rez;
    const int numTrisPerStrip = (width / rez) * 2 - 2;
    std::cout << "Created lattice of " << numStrips << " strips with " << numTrisPerStrip << " triangles each" << std::endl;
    std::cout << "Created " << numStrips * numTrisPerStrip << " triangles total" << std::endl;

    unsigned int terrainVAO, terrainVBO, terrainIBO;
    glGenVertexArrays(1, &terrainVAO);
    glBindVertexArray(terrainVAO);

    glGenBuffers(1, &terrainVBO);
    glBindBuffer(GL_ARRAY_BUFFER, terrainVBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &terrainIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, terrainIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned), &indices[0], GL_STATIC_DRAW);


    while (!glfwWindowShouldClose(window))
    {
        //adapted from https://learnopengl.com/Getting-started/Camera

        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //adapted from https://learnopengl.com/Getting-started/Shaders

        //light
        ourShader.use();
        ourShader.setVec3("light.ambient", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
        ourShader.setVec3("light.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("light.position", lightPos);

        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("light.constant", 1.0f);
        ourShader.setFloat("light.linear", 0.09f);
        ourShader.setFloat("light.quadratic", 0.032f);

        ourShader.setFloat("alpha", 1.0f);

        //coloured light
        ourShader.setVec3("colouredLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("colouredLight.diffuse", 1.0f, 0.75f, 0.0f);
        ourShader.setVec3("colouredLight.specular", 1.0f, 1.0f, 1.0f);

        ourShader.setVec3("colouredLight.position", colouredLightPos);

        ourShader.setFloat("colouredLight.constant", 1.0f);
        ourShader.setFloat("colouredLight.linear", 0.09f);
        ourShader.setFloat("colouredLight.quadratic", 0.032f);

        //set camera position as uniform to calculate fragment distance for fog
        ourShader.setVec3("userPos", camera.Position);

        //draw skybox

        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();

        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        //adapted from https://learnopengl.com/Model-Loading/Model

        for (int i=0;i<5;i++)
        {
            //set material to material i
            ourShader.setVec3("material.ambient", snowmanMaterials[i][0], snowmanMaterials[i][1], snowmanMaterials[i][2]);
            ourShader.setVec3("material.diffuse", snowmanMaterials[i][3], snowmanMaterials[i][4], snowmanMaterials[i][5]);
            ourShader.setVec3("material.specular", snowmanMaterials[i][6], snowmanMaterials[i][7], snowmanMaterials[i][8]);
            ourShader.setFloat("material.shininess", snowmanMaterials[i][9]);
            
            // render snowman1
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));
            model = glm::translate(model, snowmanPositions[i]); // set to snowman1Pos
            model = glm::translate(model, glm::vec3(0.0f, 0.2f, 0.0f));
            model = glm::rotate(model, snowman1DirectionRadians, glm::vec3(0.0, 1.0, 0.0));

            ourShader.setMat4("model", model);
            ourModel.Draw(ourShader);

            //render stick1
            model = glm::mat4(1.0f);

            //do global then do local tranformations (note: matrices are applied backwards)
            model = glm::translate(model, glm::vec3(0.3f, 0.5f, 0.0f));
            model = glm::translate(model, snowmanPositions[i] * 0.2f); // set to snowman1Pos
            model = glm::rotate(model, glm::radians(arm_swing), glm::vec3(1.0, 0.0, 0.0)); 
            model = glm::rotate(model, glm::radians(45.0f), glm::vec3(0.0, 0.0, 1.0));


            //parent object

            model = glm::rotate(model, snowman1DirectionRadians, glm::vec3(0.0, 1.0, 0.0));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

            ourShader.setMat4("model", model);
            stick1.Draw(ourShader);

            ////render stick2
            model = glm::mat4(1.0f);

            //do global then do local tranformations (note: matrices are applied backwards)
            model = glm::translate(model, glm::vec3(-0.3f, 0.5f, 0.0f));


            //parent object
            model = glm::translate(model, snowmanPositions[i] * 0.2f); // set to snowman1Pos
            model = glm::rotate(model, glm::radians(arm_swing), glm::vec3(1.0, 0.0, 0.0));

            model = glm::rotate(model, glm::radians(-45.0f), glm::vec3(0.0, 0.0, 1.0));
            model = glm::rotate(model, snowman1DirectionRadians, glm::vec3(0.0, 1.0, 0.0));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));

            ourShader.setMat4("model", model);
            stick1.Draw(ourShader);

            //moving each snowman
            if (snowman1DirectionRadians > (3.14 * 2)) snowman1DirectionRadians -= 3.14 * 2;


            if (snowmanPositions[i].x < (0.01 + snowmanStartPositions[i].x) && snowmanPositions[i].z < (1.99 + snowmanStartPositions[i].z)) snowmanPositions[i] += glm::vec3(0.0f, 0.0f, 0.001f);
            else if (snowmanPositions[i].x < (1.99 + snowmanStartPositions[i].x) && snowmanPositions[i].z > (1.99 + snowmanStartPositions[i].z)) snowmanPositions[i] += glm::vec3(0.001f, 0.0f, 0.0f);
            else if (snowmanPositions[i].x > (1.99 + snowmanStartPositions[i].x) && snowmanPositions[i].z > (0.01 + snowmanStartPositions[i].z)) snowmanPositions[i] += glm::vec3(0.0f, 0.0f, -0.001f);
            else snowmanPositions[i] += glm::vec3(-0.001f, 0.0f, 0.0f);
        }

        if (arm_swinging_forwards) {
            arm_swing += 0.02f;

            if (arm_swing > 30.0f) arm_swinging_forwards = false;
        }
        else {
            arm_swing -= 0.02f;

            if (arm_swing < -30.0f) arm_swinging_forwards = true;
        }

        //adapted from https://learnopengl.com/Model-Loading/Model
        
        //draw tree
        
        ourShader.setFloat("alpha", 0.5f);

        // render tree
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        model = glm::translate(model, glm::vec3(0.0f, -0.2f, 0.0f));
        

        ourShader.setMat4("model", model);
        tree.Draw(ourShader);

        ourShader.setFloat("alpha", 1.0f);

        //draw light sources
        lightShader.use();

        lightShader.setMat4("projection", projection);
        lightShader.setMat4("view", view);

        model = glm::mat4(1.0f);

        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(5.0f, 0.5f, 5.0f));	

        lightShader.setMat4("model", model);

        lightball.Draw(lightShader);

        //draw colouredLight
        colouredLightShader.use();

        colouredLightShader.setMat4("projection", projection);
        colouredLightShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        model = glm::translate(model, colouredLightPos); 
        model = glm::scale(model, glm::vec3(3.0f, 0.2f, 3.0f));

        lightShader.setMat4("model", model);

        lightball.Draw(colouredLightShader);

        //
        // heightmap adapted from https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map
        //
        
        heightMapShader.use();

        //set camera position as uniform to calculate fragment distance for fog
        heightMapShader.setVec3("userPos", camera.Position);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100000.0f);
        view = camera.GetViewMatrix();
        heightMapShader.setMat4("projection", projection);
        heightMapShader.setMat4("view", view);

        model = glm::mat4(1.0f);
        heightMapShader.setMat4("model", model);

        glBindVertexArray(terrainVAO);

        for (unsigned strip = 0; strip < numStrips; strip++)
        {
            glDrawElements(GL_TRIANGLE_STRIP,
                numTrisPerStrip + 2,
                GL_UNSIGNED_INT,
                (void*)(sizeof(unsigned) * (numTrisPerStrip + 2) * strip));
        }

        //
        // skybox adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps
        //

        skyboxShader.use();
        skyboxShader.setInt("skybox", 0);

        projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f); 
        glDepthFunc(GL_LEQUAL);
        
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS);


        //adapted from https://learnopengl.com/Getting-started/Hello-Window

        glfwSwapBuffers(window);
        glfwPollEvents();


    }

    glfwTerminate();
    return 0;
}

//adapted from https://learnopengl.com/Getting-started/Camera

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

//adapted from https://learnopengl.com/Getting-started/Camera

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

//adapted from https://learnopengl.com/Getting-started/Camera

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

//adapted from https://learnopengl.com/Getting-started/Camera

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}
