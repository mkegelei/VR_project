#version 330 core
layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec4 BrightColor;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;    
    sampler2D texture_normal1;
    sampler2D texture_height1;
    sampler2D texture_emission1;
    sampler2D texture_reflection1;
    float shininess;
    float refraction;
}; 

struct DirLight {
    vec3 direction;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    samplerCube pointShadowMap;
};

struct FlashLight {
    vec3 position;
    vec3 direction;
    float cutOff;
    float outerCutOff;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    samplerCube flashShadowMap;
};

in VERT_OUT {
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
    mat3 TBN;
} frag_in;

uniform float far_plane;
uniform float heightScale;
uniform bool parallax;
uniform bool reflection;
uniform bool refraction;
uniform float worldRefraction;
uniform sampler2D dirShadowMap; 

#define NR_POINT_LIGHTS 4
#define NR_FLASH_LIGHTS 2

uniform vec3 viewPos;
uniform DirLight dirLight;
uniform PointLight pointLights[NR_POINT_LIGHTS];
uniform FlashLight flashLights[NR_FLASH_LIGHTS];
uniform Material material;
uniform samplerCube skybox;

// array of offset direction for sampling
vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

// function prototypes
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float kEnergyConservation, vec4 fragPosLightSpace, vec2 texCoords);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float kEnergyConservation, vec2 texCoords);
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float kEnergyConservation, vec2 texCoords);
float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, DirLight light);
float PointShadowCalculation(vec3 fragPos, PointLight light);
float FlashShadowCalculation(vec3 fragPos, FlashLight light);
vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir);

void main()
{
    const float kPi = 3.14159265;

    // properties
    vec3 viewDir = normalize(frag_in.TBN * (viewPos - frag_in.FragPos));

    vec2 texCoords = frag_in.TexCoords;
    if(parallax)
    {
        texCoords = ParallaxMapping(frag_in.TexCoords, viewDir);       
        if(texCoords.x > 1.0 || texCoords.y > 1.0 || texCoords.x < 0.0 || texCoords.y < 0.0)
            discard;
    }

    // obtain normal from normal map in range [0,1]
    //vec3 norm = normalize(frag_in.Normal);
    vec3 norm = texture(material.texture_normal1, texCoords).rgb;
    // transform normal vector to range [-1,1]
    norm = normalize(norm * 2.0 - 1.0);  // this normal is in tangent space

    float kEnergyConservation = ( 8.0 + material.shininess ) / ( 8.0 * kPi ); 
    
    // phase 1: directional lighting
    vec3 result = CalcDirLight(dirLight, norm, viewDir, kEnergyConservation, frag_in.FragPosLightSpace, texCoords);
    //vec3 result = vec3(0.0f, 0.0f, 0.0f);
    // phase 2: point lights
    for(int i = 0; i < NR_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, frag_in.FragPos, viewDir, kEnergyConservation, texCoords);    
    // phase 3: flash light
    for(int j = 0; j < NR_FLASH_LIGHTS; j++)
        result += CalcFlashLight(flashLights[j], norm, frag_in.FragPos, viewDir, kEnergyConservation, texCoords);
    
    if(reflection)
    {
        vec3 I = normalize(frag_in.TBN*(frag_in.FragPos - viewPos));
        vec3 R = reflect(I, norm);
        result += texture(skybox, R).rgb * vec3(texture(material.texture_reflection1, texCoords));
    }

    if(refraction)
    {
        float ratio = worldRefraction / material.refraction;
        vec3 I = normalize(frag_in.TBN*(frag_in.FragPos - viewPos));
        vec3 R = refract(I, norm, ratio);
        result += texture(skybox, R).rgb;
    }

    result *= 0.2f;

    vec3 emission = 2*texture(material.texture_emission1, texCoords).rgb;
    result += emission;


    FragColor = vec4(result, 1.0);
    float gamma = 1.0;
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
    
    // check whether result is higher than some threshold, if so, output as bloom threshold color
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    BrightColor = vec4(0.0, 0.0, 0.0, 1.0);
    if(brightness > 1.0f)
        BrightColor = vec4(result, 1.0);
    
}

// calculates the color when using a directional light.
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir, float kEnergyConservation, vec4 fragPosLightSpace, vec2 texCoords)
{
    vec3 lightDir = normalize(frag_in.TBN * (-light.direction));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = kEnergyConservation * pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    // calculate shadow
    float shadow = DirShadowCalculation(fragPosLightSpace, normal, light); 

    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a point light.
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float kEnergyConservation, vec2 texCoords)
{
    vec3 lightDir = normalize(frag_in.TBN * (light.position - fragPos));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = kEnergyConservation * pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(frag_in.TBN * (light.position - fragPos));
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    ambient *= attenuation;
    diffuse *= attenuation;
    specular *= attenuation;
    // calculate shadow
    float shadow = PointShadowCalculation(fragPos, light);
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

// calculates the color when using a flash light.
vec3 CalcFlashLight(FlashLight light, vec3 normal, vec3 fragPos, vec3 viewDir, float kEnergyConservation, vec2 texCoords)
{
    vec3 lightDir = normalize(frag_in.TBN * (light.position - fragPos));
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 halfwayDir = normalize(lightDir + viewDir);  
    float spec = kEnergyConservation * pow(max(dot(normal, halfwayDir), 0.0), material.shininess);
    // attenuation
    float distance = length(frag_in.TBN * (light.position - fragPos));
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // flashlight intensity
    float theta = dot(lightDir, normalize(frag_in.TBN * (-light.direction))); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 diffuse = light.diffuse * diff * vec3(texture(material.texture_diffuse1, texCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.texture_specular1, texCoords));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    float shadow = FlashShadowCalculation(fragPos, light);
    //float shadow = 0.0f;
    return (ambient + (1.0 - shadow) * (diffuse + specular));
}

float DirShadowCalculation(vec4 fragPosLightSpace, vec3 normal, DirLight light)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(dirShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 lightDir = normalize(frag_in.TBN * (-light.direction));
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);
    // check whether current frag pos is in shadow
    // float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

float PointShadowCalculation(vec3 fragPos, PointLight light)
{
    // get vector between fragment position and light position
    vec3 fragToLight = frag_in.TBN * (fragPos - light.position);
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(frag_in.TBN * (viewPos - fragPos));
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(light.pointShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

float FlashShadowCalculation(vec3 fragPos, FlashLight light)
{
    // get vector between fragment position and light position
    vec3 fragToLight = frag_in.TBN * (fragPos - light.position);
    // now get current linear depth as the length between the fragment and light position
    float currentDepth = length(fragToLight);
    
    float shadow = 0.0;
    float bias = 0.15;
    int samples = 20;
    float viewDistance = length(frag_in.TBN * (viewPos - fragPos));
    float diskRadius = (1.0 + (viewDistance / far_plane)) / 25.0;
    for(int i = 0; i < samples; ++i)
    {
        float closestDepth = texture(light.flashShadowMap, fragToLight + gridSamplingDisk[i] * diskRadius).r;
        closestDepth *= far_plane;   // undo mapping [0;1]
        if(currentDepth - bias > closestDepth)
            shadow += 1.0;
    }
    shadow /= float(samples);
        
    // display closestDepth as debug (to visualize depth cubemap)
    // FragColor = vec4(vec3(closestDepth / far_plane), 1.0);    
        
    return shadow;
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{ 
    // number of depth layers
    const float minLayers = 8;
    const float maxLayers = 32;
    float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0.0, 0.0, 1.0), viewDir)));  
    // calculate the size of each layer
    float layerDepth = 1.0 / numLayers;
    // depth of current layer
    float currentLayerDepth = 0.0;
    // the amount to shift the texture coordinates per layer (from vector P)
    vec2 P = viewDir.xy / viewDir.z * heightScale; 
    vec2 deltaTexCoords = P / numLayers;
  
    // get initial values
    vec2  currentTexCoords     = texCoords;
    float currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;
      
    while(currentLayerDepth < currentDepthMapValue)
    {
        // shift texture coordinates along direction of P
        currentTexCoords -= deltaTexCoords;
        // get depthmap value at current texture coordinates
        currentDepthMapValue = texture(material.texture_height1, currentTexCoords).r;  
        // get depth of next layer
        currentLayerDepth += layerDepth;  
    }
    
    // get texture coordinates before collision (reverse operations)
    vec2 prevTexCoords = currentTexCoords + deltaTexCoords;

    // get depth after and before collision for linear interpolation
    float afterDepth  = currentDepthMapValue - currentLayerDepth;
    float beforeDepth = texture(material.texture_height1, prevTexCoords).r - currentLayerDepth + layerDepth;
 
    // interpolation of texture coordinates
    float weight = afterDepth / (afterDepth - beforeDepth);
    vec2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0 - weight);

    return finalTexCoords;
}