attribute vec2 w4_a_uv0;

varying vec2 vTexCoord;

void w4_main()
{
    gl_Position = w4_u_projectionView * w4_u_model * vec4(w4_a_position , 1.0);
    vTexCoord = vec2(w4_a_uv0.x, 1.0 - w4_a_uv0.y);
}

