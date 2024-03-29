#version 400 core

layout(vertices = 4) out;
int outa[gl_out.length()];

layout(quads) in;                   // ERROR
layout(ccw) out;                    // ERROR
layout(fractional_even_spacing) in; // ERROR

patch in vec4 patchIn;              // ERROR
patch out vec4 patchOut;

void main()
{
    barrier();

    int a = gl_MaxTessControlInputComponents +
            gl_MaxTessControlOutputComponents +
            gl_MaxTessControlTextureImageUnits +
            gl_MaxTessControlUniformComponents +
            gl_MaxTessControlTotalOutputComponents;

    vec4 p = gl_in[1].gl_Position;
    float ps = gl_in[1].gl_PointSize;
    float cd = gl_in[1].gl_ClipDistance[2];

    int pvi = gl_PatchVerticesIn;
    int pid = gl_PrimitiveID;
    int iid = gl_InvocationID;

    gl_out[1].gl_Position = p;
    gl_out[1].gl_PointSize = ps;
    gl_out[1].gl_ClipDistance[1] = cd;

    gl_TessLevelOuter[3] = 3.2;
    gl_TessLevelInner[1] = 1.3;
}

layout(vertices = 4) in;    // ERROR
layout(vertices = 5) out;   // ERROR

void foo()
{
    gl_out[4].gl_PointSize;  // ERROR
}

in vec2 ina;   // ERROR, not array
in vec2 inb[];
in vec2 inc[18];  // ERROR, wrong size
in vec2 ind[gl_MaxPatchVertices];

#extension GL_ARB_separate_shader_objects : enable

layout(location = 3) in vec4 ivla[];
layout(location = 4) in vec4 ivlb[];
layout(location = 4) in vec4 ivlc[];  // ERROR

layout(location = 3) out vec4 ovla[];
layout(location = 4) out vec4 ovlb[];
layout(location = 4) out vec4 ovlc[];  // ERROR
