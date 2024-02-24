
attribute vec2 w4_a_uv0;
varying vec2 VTexCoords;

void w4_main()
{
    VTexCoords = w4_a_uv0;
    gl_Position = vec4(w4_a_position , 1.0);
}