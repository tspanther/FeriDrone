#version 330
out vec4 out_Color;
in vec4 ColorVS;

void main(){
    out_Color = vec4(ColorVS.rgba);
}
