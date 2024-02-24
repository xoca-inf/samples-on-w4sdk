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
    w4_BlinnLightModel(results, vectorToLight, vectorToEye, N, lColor, shadowFactor, fading, shininess);
}

void w4_main()
{
   vec4 diffColor = texture2D(texture0, v_TexCoord);

   vec3 mixColor = texture2D(texture1, v_TexCoord).rgb;

   vec3 N = normalize ( v_normal );
   vec3 V = normalize ( v_eyePosition - v_position );

   vec3 lightFactors[2];
   w4_calculateLightFactor(lightFactors, v_position, v_eyePosition, N);

   vec3 lightColor = lightFactors[0] + lightFactors[1];

   vec3 color = mix(diffColor.rgb, mixColor, lightColor.r);

   gl_FragColor = vec4(color * lightColor, 1.);

}
