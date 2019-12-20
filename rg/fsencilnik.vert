#version 330
out vec4 out_Color;
in vec3 gPos;
uniform vec4 DodajBarvo;
void main(){
    out_Color=vec4(gPos.x, gPos.y, 0, 1) + DodajBarvo;
}
