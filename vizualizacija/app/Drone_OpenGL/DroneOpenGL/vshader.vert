#version 330

layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 normal;
layout(location=2) in vec2 aTexCoord;

out vec2 TexCoord;
out vec4 normalVS;
out vec3 posVS;

uniform mat4 PVM;
uniform mat4 M;

void main(){
    gl_Position = PVM * vec4(in_Pos.xyz, 1.0);

    TexCoord = aTexCoord;
    normalVS =  mat4(transpose(inverse(M))) * vec4(normal.xyz,1);
    posVS = vec3(M * vec4(in_Pos.xyz, 1.0));
}
