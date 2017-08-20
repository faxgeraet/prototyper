#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;

void main()
{
    vec4 color1 = texture(texture_diffuse1, TexCoords);
    vec4 color2 = texture(texture_diffuse1, TexCoords);
    if( length(vec3(color1.xyz))==0)
        color1= vec4(0.5,0.5,0.5,1.0);
    if( length(vec3(color2.xyz))==0)
        color2= vec4(0.5,0.5,0.5,1.0);
    float diffuseAmount = dot(normal, normalize(vec3(2.0, 4.0, 1.0)));
    diffuseAmount = 0.6 * diffuseAmount + 0.4;
    
    FragColor = (color1+color2)*diffuseAmount/2;
}
