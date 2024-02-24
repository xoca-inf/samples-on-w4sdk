uniform sampler2D texture0;
varying vec2 vTexCoord;

void w4_main()
{
   vec4 tex = texture2D(texture0, vTexCoord);
   gl_FragColor = tex;
}
