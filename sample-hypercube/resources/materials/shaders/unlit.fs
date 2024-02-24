uniform sampler2D texture0;
varying vec2 vTexCoord;

void w4_main()
{
    vec4 diff = texture2D(texture0, vTexCoord);
    gl_FragColor = vec4(diff.r, diff.g, diff.b, 1.0);
}
