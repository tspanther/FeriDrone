#version 330
out vec4 out_Color;
in vec3 barva;

void main(){
    out_Color = vec4(barva.xyz, 1);
}
