#version 330 core
out vec4 FragColor;

in vec3 TexCoords;

uniform samplerCube skybox;

// adapted from https://learnopengl.com/Advanced-OpenGL/Cubemaps

void main()
{    
    FragColor = texture(skybox, TexCoords);
}