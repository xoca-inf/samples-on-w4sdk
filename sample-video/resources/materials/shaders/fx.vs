attribute vec2 w4_a_uv0;

varying vec2 vUv;

void w4_main()
{
    gl_Position = vec4(w4_a_position , 1.0);
    vUv = w4_a_uv0;
}

