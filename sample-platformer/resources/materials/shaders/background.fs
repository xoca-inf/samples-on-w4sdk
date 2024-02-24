uniform sampler2D texture0;
uniform sampler2D texture1;

varying vec2 vUV;

void w4_main()
{
    vec2 c1 = vec2(vUV.x + w4_u_time / 25.0, vUV.y);
    vec2 c2 = vec2(vUV.x + w4_u_time / 7.2, vUV.y);
    if (c2.x > 1.0)
        c2.x -= floor(c2.x);
    if (c1.x > 1.0)
        c1.x -= floor(c1.x);

    vec4 layer1  = texture2D(texture0, c1);
    vec4 layer2  = texture2D(texture1, c2);

    gl_FragColor = mix(layer1, layer2, layer2.a);
}
