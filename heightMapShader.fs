#version 330 core

out vec4 FragColor;
in float distance;

in float Height;

// adapted from https://learnopengl.com/Guest-Articles/2021/Tessellation/Height-map

void main()
{
    float h = (((Height + 1) * 0.05) + 0.85);	// shift and scale the height in to a grayscale value

    //add fog
    vec4 fogColour = vec4(0.8, 0.8, 0.8, 1.0);
    float fogWeighting = 1 - (1/(distance / 10));

    vec4 addedFog = (vec4(h,h,h,1.0) * (1 - fogWeighting)) + (fogColour * fogWeighting);
    FragColor = addedFog;//vec4(h,h,h,1.0);

    //FragColor = vec4(h, h, h, 1.0);
}