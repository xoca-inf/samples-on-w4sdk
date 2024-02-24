uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform vec3 mainColor;

varying vec3 v_eyePosition;
varying vec3 v_normal;
varying vec3 v_position;
varying vec3 v_tangent;
varying mat3 v_TBN;

varying vec2 v_TexCoord;

const float shininess = 32.0;

void w4_userLightModel(inout vec3 results[2], vec3 vectorToLight, vec3 vectorToEye, vec3 N, vec3 lColor, float shadowFactor, float fading)
{
    w4_BlinnLightModel(results, vectorToLight, vectorToEye, N, lColor, fading, shadowFactor, shininess);
}

void w4_main()
{
   vec4 diffColor = texture2D(texture0, v_TexCoord);

   vec3 bumpMap = texture2D(texture1, v_TexCoord).rgb;
   bumpMap = normalize(bumpMap * 2.0 - 1.0);
   bumpMap = normalize(v_TBN * bumpMap);

   vec3 N = normalize ( bumpMap );

   vec3 lightFactors[2];
   w4_calculateLightFactor(lightFactors, v_position, v_eyePosition, N);
   gl_FragColor = vec4(diffColor.rgb * (lightFactors[0] + lightFactors[1]), 1.);
}
