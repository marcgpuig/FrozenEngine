#version 330 core
out vec4 color;
  
in vec3 Normal;  
in vec3 FragPos;  

uniform vec3 viewPos;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

void main()
{
    color = vec4(normalize(Normal), 1.0f);
}