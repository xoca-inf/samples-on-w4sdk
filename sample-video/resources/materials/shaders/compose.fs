uniform sampler2D texture0;
uniform sampler2D texture1;
varying vec2 vUv;

void w4_main()
{
   gl_FragColor = texture2D(texture0, vUv) + texture2D(texture1, vUv);
}
