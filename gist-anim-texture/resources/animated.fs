varying vec2 vTexCoord;
uniform sampler2D texture0;

void w4_main()
{
    gl_FragColor = texture2D(texture0, vTexCoord);
}
 