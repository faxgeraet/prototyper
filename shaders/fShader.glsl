#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D texture_diffuse1;

void main()
{
    vec4 color1 = texture(texture_diffuse1, TexCoords);

    //  diffuse light
    
    float diffuseAmount = dot(normal, normalize(vec3(2.0, 4.0, 1.0)));
    diffuseAmount= diffuseAmount*0.5+0.5;
    
    
    //  apply default color if no texture is available
    
//    if(color1.a < 0.2) discard;
    if(length(color1.xyz)==0) color1 = vec4(1.0,1,1,0.5);
    FragColor = vec4 (color1.xyz * diffuseAmount, color1.w);
}
