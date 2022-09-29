#version 450 core

layout (location = 0) uniform float Thickness;
layout (location = 1) uniform vec2 Viewport;

in VertexData
{
    vec4 color;
} VertexIn[4];

out VertexData
{
    vec4 fColor;
} VertexOut;

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 12) out;

vec2 toScreenSpace(vec4 vertex)
{
    return vec2(vertex.xy / vertex.w) * Viewport;
}

float toZValue(vec4 vertex)
{
    return (vertex.z/vertex.w);
}

void drawLine(vec2 points[2], vec4 colors[2], float zValues[2])
{
    vec2 p0 = points[0];
    vec2 p1 = points[1];

    // Determine the line direction
    vec2 v = normalize(p1 - p0);

    // Determine the line normal
    vec2 n = vec2(-v.y, v.x);

    // generate the triangle strip
    VertexOut.fColor = colors[0];
    gl_Position = vec4((p0 + Thickness * n) / Viewport, zValues[0], 1.0);
    EmitVertex();

    VertexOut.fColor = colors[0];
    gl_Position = vec4((p0 - Thickness * n) / Viewport, zValues[0], 1.0);
    EmitVertex();

    VertexOut.fColor = colors[1];
    gl_Position = vec4((p1 + Thickness * n) / Viewport, zValues[1], 1.0);
    EmitVertex();

    VertexOut.fColor = colors[1];
    gl_Position = vec4((p1 - Thickness * n) / Viewport, zValues[1], 1.0);
    EmitVertex();

    EndPrimitive();
}

void main(void)
{
    // 2 control points
    vec4 control[2];
    control[0] = gl_in[0].gl_Position;
    control[1] = gl_in[3].gl_Position;

    // 4 attached colors
    vec4 color[2];
    color[0] = VertexIn[0].color;
    color[1] = VertexIn[3].color;

    vec2 linePoints[2];
    linePoints[0] = toScreenSpace(control[0]);
    linePoints[1] = toScreenSpace(control[1]);
    float lineZVals[2];
    lineZVals[0] = toZValue(control[0]);
    lineZVals[1] = toZValue(control[1]);

    drawLine(linePoints, color, lineZVals);
}
