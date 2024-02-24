varying vec2 vUV;
varying vec3 vP;
varying vec3 vT;
varying vec3 vB;
varying vec3 vN;
varying vec3 vE;

uniform sampler2D texture0; //baseColorTexture;
uniform sampler2D texture1; //aoTexture;
uniform sampler2D texture2; //metalRoughnessTexture;
uniform sampler2D texture3; //normalTexture;
uniform sampler2D texture4; //emissiveTexture;

uniform float metallicOffset;
uniform float roughnessOffset;
uniform float emissiveFactor;

float Diffuse(float NdotL, float LdotH, float NdotV, float roughness)
{
    float energyBias        = mix(0.0, 0.5, roughness);
    float energyFactor      = mix(1.0, 1.0 / 1.51, roughness);
    float fd90              = energyBias + 2.0 * (LdotH * LdotH) * roughness;

    float lightScatter      = 1.0 + (fd90 - 1.0) * pow(max(1.0 - NdotL, 0.1), 5.0);
    float viewScatter       = 1.0 + (fd90 - 1.0) * pow(max(1.0 - NdotV, 0.1), 5.0);

    return lightScatter * viewScatter * energyFactor;
}

float Specular(float NdotL, float LdotH, float NdotV, float NdotH, float roughness, float specular)
{
    float rough2            = max(roughness * roughness, 0.002);
    float rough4            = rough2 * rough2;

    float d                 = (NdotH * rough4 - NdotH) * NdotH + 1.0;
    float D                 = rough4 / (3.14159 * (d * d));

    float F                 = specular + (1.0 - specular) * exp2((-5.55473 * LdotH - 6.98316) * LdotH);

    float k                 = rough2 * 0.5;
    float G_SmithL          = NdotL * (1.0 - k) + k;
    float G_SmithV          = NdotV * (1.0 - k) + k;
    float G                 = 0.25 / (G_SmithL * G_SmithV);

    return G * D * F;
}

float metallic;
float roughness;

void w4_userLightModel(inout vec3 results[2], vec3 vectorToLight, vec3 vectorToEye, vec3 normal, vec3 lColor, float shadowFactor, float fading)
{
    float attenuation = min(1.0 / (0.001 + fading * length(vectorToLight)), 1.0);

    vec3 lDirection = normalize(vectorToLight);
    vec3 halfVector = normalize(lDirection + vectorToEye);
    float NdotL = clamp(dot(normal, lDirection),  0.0, 1.0);
    float LdotH = clamp(dot(lDirection, halfVector), 0.0, 1.0);
    float NdotV = clamp(dot(normal, vectorToEye),  0.0, 1.0);
    float NdotH = clamp(dot(normal, halfVector), 0.0, 1.0);

    vec3 lDiffuse = lColor * attenuation;
    results[0] += NdotL * Diffuse(NdotL, LdotH, NdotV, roughness) * lDiffuse;
    results[1] += NdotL * Specular(NdotL, LdotH, NdotV, NdotH, roughness, metallic) * lDiffuse;
}

void w4_main()
{
    vec4 baseColor          = texture2D(texture0,vUV);
    float ambientOcclution  = texture2D(texture1,vUV).r;
    metallic          = clamp(texture2D(texture2,vUV).b+metallicOffset, 0.0, 1.0);
    roughness         = clamp(texture2D(texture2,vUV).g+roughnessOffset, 0.0, 1.0);

    vec4 emissive           = texture2D(texture4,vUV) * emissiveFactor;

    mat3 vTBN               = mat3(vT, vB, vN);

    vec3 normal             = texture2D(texture3, vUV).rgb;
    normal                  = normalize(normal * 2.0 - 1.0);
    normal                  = normalize(vTBN * normal);

    vec3 eDirection          = normalize(vE - vP);

    vec3 lightFactors[2];
    w4_calculateLightFactor(lightFactors, vP, vE, vN);

    lightFactors[0] = mix(lightFactors[0], lightFactors[0] * ambientOcclution, ambientOcclution);

    vec3 finalColor             = emissive.rgb + baseColor.rgb * lightFactors[0] + lightFactors[1];
    gl_FragColor                = vec4(finalColor, baseColor.a);
}