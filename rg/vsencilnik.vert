#version 330
// Vertex_shader MORA prejeti nek input, saj bi drugače bil neučinkovit.
// Vertex_shader pridobi input direktno iz vertex_data.
// Da povežemo vertex_data z ustreznim vertex_shader-jem rabimo naznanit vhodno lokacijo. To pomeni, da dobimo podatke
// sem iz glavnega programa v katerem se sklicujemo na ustrezno lokacijo.
// in_Pos je vhodna točka, out je izhodna točka.
// uniform mat4 PVM je globalno definirana matrika.
layout(location=0) in vec3 in_Pos;
layout(location=1) in vec3 barvaOglisca;

uniform mat4 PVM;
out vec3 gPos;
out vec3 barva;

void main(){
    // gl_Position je rezervirana beseda, ki hrani lokacijo našega oglišča.
    // Izhod Vertex_Shader: gl_Position + costum_data(user defined)
    gl_Position=PVM*vec4(in_Pos.xyz, 1);
    gPos=in_Pos;
    barva = barvaOglisca;
}
