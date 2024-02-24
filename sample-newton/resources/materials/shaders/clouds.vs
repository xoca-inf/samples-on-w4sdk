attribute vec3 w4_a_normal;
attribute vec2 w4_a_uv0;

varying vec2 v_TexCoord;
void w4_main()
{
    v_TexCoord = w4_a_uv0;
    gl_Position = vec4(w4_a_position,1.0);

}

