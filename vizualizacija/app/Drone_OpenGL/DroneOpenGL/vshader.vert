#version 330
layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec2 aTexCoord;
out vec4 ColorFS;
out vec2 TexCoord;
uniform mat4 PVM;
void main(){
    gl_Position = PVM * vec4(in_Pos.xyz,1);
    ColorFS = vec4(in_Color.xyz,1);
    TexCoord = aTexCoord;
}
