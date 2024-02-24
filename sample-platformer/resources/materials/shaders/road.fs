uniform sampler2D texture0;
varying vec2 vUV;


void w4_main()
{
    vec2 c2 = vec2(vUV.x + w4_u_time / 7.2, vUV.y);
    if (c2.x > 1.0)
        c2.x -= floor(c2.x);

    gl_FragColor = texture2D(texture0, c2);
}
