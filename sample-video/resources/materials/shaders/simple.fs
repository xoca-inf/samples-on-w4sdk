uniform sampler2D texture0;
varying vec2 vTexCoord;
uniform vec4 color;

void w4_main()
{
//    gl_FragColor = textureCube(cubemap0, vTexCoord) * myColor;// * vec4(1.0, lightPositions[0].g, lightPositions[5].b, 1.0);
   vec4 tex = texture2D(texture0, vTexCoord);
   tex *= color;
   gl_FragColor = tex;
}
