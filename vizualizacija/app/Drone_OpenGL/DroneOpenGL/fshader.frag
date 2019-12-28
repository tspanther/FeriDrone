#version 330
out vec4 out_Color;
in vec2 TexCoord;
in vec4 ColorVS;
uniform sampler2D ourTexture;
void main() {
    out_Color=texture(ourTexture, TexCoord);
}
