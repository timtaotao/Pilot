#version 310 es

#extension GL_GOOGLE_include_directive : enable

#include "constants.h"

layout(input_attachment_index = 0, set = 0, binding = 0) uniform highp subpassInput in_color;

layout(set = 0, binding = 1) uniform sampler2D color_grading_lut_texture_sampler;

layout(location = 0) out highp vec4 out_color;

void main()
{
    highp ivec2 lut_tex_size = textureSize(color_grading_lut_texture_sampler, 0);
    highp float _COLORS      = float(lut_tex_size.y);

    highp vec4 color       = subpassLoad(in_color).rgba;

    highp float index = color.b * _COLORS - 1.0f;
    highp float index_u = floor(index) / _COLORS;

    highp float index_u_max = floor(index + 1.0f) / _COLORS;

    highp vec2 uv = vec2(index_u + color.r / _COLORS, color.g);
    highp vec2 uv_max = vec2(index_u_max + color.r / _COLORS, color.g);
    
    highp vec4 color_sampled = texture(color_grading_lut_texture_sampler, uv);
    highp vec4 color_sampled_max = texture(color_grading_lut_texture_sampler, uv_max);

    highp vec4 color_mixed = mix(color_sampled, color_sampled_max, index - floor(index));


    //out_color = color;
    out_color = vec4(color_mixed.rgb, color.a);
}
