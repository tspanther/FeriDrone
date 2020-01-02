#version 330

out vec4 out_Color;

in vec2 TexCoord;
in vec4 normalVS;
in vec3 posVS;

uniform sampler2D ourTexture;
uniform vec3 Ra;
uniform vec3 Rd;
uniform vec3 Rs;
uniform vec3 lightColor;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform int ns;

void main() {
    vec3 ambient = lightColor * Ra;

    vec3 n_e = vec3(normalize(normalVS));
    vec3 l_e = vec3(normalize(lightPos - posVS));
    vec3 diffuse = lightColor * Rd * dot(n_e, l_e);

    vec3 v_e = vec3(camPos - posVS);
    vec3 h_e = normalize(l_e + v_e);
    vec3 spc = lightColor * Rs * pow(dot(n_e, h_e), ns);

    vec3 I_ = ambient + diffuse + spc;
    vec4 I = vec4(I_.xyz, 1.0);

    out_Color = I * texture(ourTexture, TexCoord);
}
