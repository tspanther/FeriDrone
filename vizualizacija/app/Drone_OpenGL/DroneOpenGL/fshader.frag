#version 330
out vec4 out_Color;
in vec2 TexCoord;
in vec4 ColorVS;
in float Alfa;
uniform sampler2D ourTexture;
void main() {
    out_Color=texture(ourTexture, TexCoord);

    // TODO: this impl. of Alpha ch. is in 'debug mode'.
    if(Alfa == 0.0f)
        out_Color.a = 1.0f;
    else
        out_Color.a = Alfa;
}
