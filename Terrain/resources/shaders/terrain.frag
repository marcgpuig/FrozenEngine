#version 330 core
out vec4 color;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

#define MAX_POINT_LIGHTS 25

struct SunLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform SunLight sun;

struct pointLight {
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
	
    float constant;
    float linear;
    float quadratic;
};

uniform int N_of_lights;
uniform pointLight light[MAX_POINT_LIGHTS];

uniform vec3 viewPos;

uniform sampler2D texture_0;
uniform sampler2D texture_0_h;
uniform sampler2D texture_0_s;

uniform sampler2D texture_1;
uniform sampler2D texture_1_h;
uniform sampler2D texture_1_s;

uniform sampler2D texture_2;
uniform sampler2D texture_2_h;
uniform sampler2D texture_2_s;

uniform sampler2D texture_3;
uniform sampler2D texture_3_h;
uniform sampler2D texture_3_s;

uniform sampler2D alpha;

vec3 calcSunLight(SunLight sun, vec3 normal, vec3 viewDir, vec3 tex, float specText)
{    
    vec3 lightDir = normalize(sun.direction);
	// Diffuse 
    float diff = max(dot(normal, lightDir), 0.0);
	// Specular
	vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);

	vec3 ambient  = sun.ambient			* tex;
    vec3 diffuse  = sun.diffuse  * diff * tex;
    vec3 specular = sun.specular * spec * specText;

    return (ambient + diffuse + specular);
}

vec3 calcPointLight(pointLight light, vec3 normal, vec3 viewDir, vec3 tex, float specText)
{
    vec3 lightDir = normalize(light.position - FragPos);
    // Diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // Specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 16);
    // Attenuation
    float distance = length(light.position - FragPos);
    float attenuation = 1.0f / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    // Combine results
    vec3 ambient  = light.ambient		  * tex;
    vec3 diffuse  = light.diffuse  * diff * tex;
    vec3 specular = light.specular * spec * specText;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

void main()
{
	vec3 norm = normalize(Normal);
	vec3 viewDir = normalize(viewPos - FragPos);

	float scale0 = 0.2;
	float scale1 = 0.2;
	float scale2 = 0.2;
	float scale3 = 0.5;

	float alphamap1 = texture(alpha, TexCoord/8.0).x;
	float alphamap2 = texture(alpha, TexCoord/4.0).x;
	float alphamap3 = texture(alpha, TexCoord/2.0).x;
	
	vec3 finaltexture = texture(texture_0, TexCoord/scale0).xyz;
	float specularMap = texture(texture_0_s, TexCoord/scale0).w;

	if(alphamap3 >= (1.0f - texture(texture_3_h, TexCoord/scale3).w))
	{
		finaltexture = texture(texture_3, TexCoord/scale3).xyz;
		specularMap = texture(texture_3_s, TexCoord/scale3).w;
	}
	else if(alphamap2 >= (1.0f - texture(texture_2_h, TexCoord/scale2).w))
	{
		finaltexture = texture(texture_2, TexCoord/scale2).xyz;
		specularMap = texture(texture_2_s, TexCoord/scale2).w;
	}
	else if(alphamap1 >= (1.0f - texture(texture_1_h, TexCoord/scale1).w))
	{
		finaltexture = texture(texture_1, TexCoord/scale1).xyz;
		specularMap = texture(texture_1_s, TexCoord/scale1).w;
	}

	vec3 result = calcSunLight(sun, norm, viewDir, finaltexture, specularMap);
	for(int i = 0; i < N_of_lights; i++)
		result += calcPointLight(light[i], norm, viewDir, finaltexture, specularMap);

	color = vec4(result, 1.0f);
}