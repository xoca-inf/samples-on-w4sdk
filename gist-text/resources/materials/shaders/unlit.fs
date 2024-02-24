uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float aspect;

varying vec2 vUV;

void w4_main()
{
    float height = 1.0 / aspect;
    gl_FragColor = texture2D(texture0, vec2(vUV.x, clamp(vUV.y * aspect - (aspect -  1.0 + height) / 2.0, 0.0, 1.0))) + texture2D(texture1, vUV);
}
