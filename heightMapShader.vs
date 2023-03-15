#version 330 core
layout (location = 0) in vec3 aPos;



out float Height;
out vec3 Position;
out float distance;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 userPos;

// adapted from https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

void main()
{
    Height = aPos.y;
    Position = (view * model * vec4(aPos, 1.0)).xyz;
    gl_Position = projection * view * model * vec4(aPos, 1.0);

    distance = length(userPos - aPos);
}