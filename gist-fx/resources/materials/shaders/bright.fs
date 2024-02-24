uniform sampler2D texture0;
uniform float brightPassThreshold;

varying vec2 vUv;

void w4_main()
{
   vec3 luminanceVector = vec3(0.2125, 0.7154, 0.0721);
   vec4 c = texture2D(texture0, vUv);

   float luminance = dot(luminanceVector, c.xyz);
   luminance = max(0.0, luminance - brightPassThreshold);
   c.xyz *= sign(luminance);
   c.a = 1.0;

   gl_FragColor = c;
}
