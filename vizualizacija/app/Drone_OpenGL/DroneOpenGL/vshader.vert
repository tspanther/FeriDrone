#version 330

layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec3 normalVS; // od modela (blender..)
out vec3 posVS; // od modela (blender..)

/*
uniform mat4 P;
uniform mat4 VM;
uniform mat4 NormalMatrix;
*/

uniform mat4 PVM;
uniform mat4 M;
uniform mat4 NormalMatrix; // transpose(inverse(M))

void main(){
    /*
    gl_Position = P * VM * vec4(in_Pos.xyz, 1.0);

    TexCoord = aTexCoord;
    normalVS = normalize(NormalMatrix * vec4(normal.xyz, 1.0)).xyz; 
    posVS = (VM * vec4(in_Pos.xyz, 1.0)).xyz;
    */

    gl_Position = PVM * vec4(in_Pos.xyz, 1.0);

    TexCoord = aTexCoord;
    normalVS = normalize(NormalMatrix * vec4(normal.xyz, 1.0)).xyz; 
    posVS = (M * vec4(in_Pos.xyz, 1.0)).xyz;
}

