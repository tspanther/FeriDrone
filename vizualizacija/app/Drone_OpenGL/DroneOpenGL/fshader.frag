#version 330

out vec4 out_Color;

in vec2 TexCoord;
in vec3 normalVS;
in vec3 posVS;
in float Alpha;

uniform sampler2D ourTexture;
uniform vec3 Ra;
uniform vec3 Rd;
uniform vec3 Rs;
uniform vec3 lightColor;
uniform vec3 camPos;
uniform vec3 lightPos;
uniform int ns;
uniform bool activateShading;

void main() {
    vec4 color;
    
    if (activateShading == false) {
        color = texture(ourTexture, TexCoord);
    } else {
        vec3 ambient = lightColor * Ra;

        vec3 N = normalize(normalVS);
        vec3 L = normalize(lightPos - posVS);
        vec3 V = normalize(camPos - posVS);
        vec3 H = (L + V) / length(L + V);

        vec3 diffuse = lightColor * Rd * clamp(dot(N, L), 0, 1);

        vec3 spc = vec3(0.0, 0.0, 0.0);
        /*
        if (diffuse.x > 0.0 || diffuse.z > 0.0 || diffuse.y > 0.0) {
            spc = lightColor * Rs * pow(clamp(dot(N, H), 0, 1), ns);
        }
        */

        color = (vec4(diffuse.xyz, 1) + vec4(ambient.xyz, 1)) * texture(ourTexture, TexCoord) + clamp(vec4(spc.xyz, 1.0), 0, 1);
    }

    if (Alpha < 0.01) {
        color.a = 1.0;
    } else {
        color.a = Alpha;
    }
        
    out_Color = color;
}