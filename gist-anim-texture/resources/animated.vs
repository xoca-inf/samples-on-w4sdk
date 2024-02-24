attribute vec2 w4_a_uv0;
attribute vec3 w4_a_normal;

uniform float startTime;
uniform float countX;
uniform float countY;
uniform float duration;

varying vec2 vTexCoord;

void w4_main()
{
    gl_Position = w4_u_projectionView * w4_u_model * vec4(w4_a_position , 1.0);

    float framePart = mod(w4_u_time - startTime, duration) / duration;
    float frameNum = floor(countX * countY * framePart);

    float frameX = min(mod(frameNum, countX), countX - 1.0);
    float frameY = floor(frameNum / countX);

    vTexCoord.x = (frameX + w4_a_uv0.x) / countX;
    vTexCoord.y = 1. -(frameY + w4_a_uv0.y) / countY;
}
