#version 330
layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 in_Color;
layout(location=2) in vec2 aTexCoord;
layout(location=3) in float in_Alfa;
out vec4 ColorVS;
out vec2 TexCoord;
out float Alfa;
uniform mat4 PVM;
void main(){
    gl_Position = PVM * vec4(in_Pos.xyz,1);
    ColorVS = vec4(in_Color.xyz,1);
    TexCoord = aTexCoord;
    Alfa = in_Alfa;
}
