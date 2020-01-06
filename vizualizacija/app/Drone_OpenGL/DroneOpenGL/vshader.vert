#version 330

layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in float in_Alfa;

out vec2 TexCoord;
out vec3 normalVS;
out vec3 posVS;
out float Alpha;

uniform mat4 PVM;
uniform mat4 M;
uniform mat4 NormalMatrix;

void main() {
    gl_Position = PVM * vec4(in_Pos.xyz, 1.0);

    TexCoord = aTexCoord;
    normalVS = normalize(NormalMatrix * vec4(normal.xyz, 1.0)).xyz; 
    posVS = (M * vec4(in_Pos.xyz, 1.0)).xyz;
    Alpha = in_Alfa;
}
