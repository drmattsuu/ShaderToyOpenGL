#version 430 core

layout (location = 0) uniform float Thickness;
layout (location = 1) uniform vec2 Viewport;

layout (location = 2) uniform float MiterLimit = 0.1;
layout (location = 3) uniform int Segments = 30;

in VertexData{
    vec4 color;
} VertexIn[4];

out VertexData{
//    vec2 mTexCoord;
    vec4 fColor;
} VertexOut;

const int SegmentsMax = 30; // max_vertices = (SegmentsMax+1)*4;
const int SegmentsMin = 3; // min number of segments per curve

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 124) out;

vec2 toScreenSpace(vec4 vertex)
{
    return vec2( vertex.xy / vertex.w ) * Viewport;
}

vec4 fromScreenSpace(vec2 vertex)
{
    vec2 worldSpaceVtx = vertex / Viewport;
    return vec4(worldSpaceVtx[0], worldSpaceVtx[1], 0.0, 1.0);
}

void buildPoint(int i)
{
    vec2 position = toScreenSpace(gl_in[i].gl_Position);
    VertexOut.fColor = VertexIn[i].color;
    gl_Position = fromScreenSpace(position + vec2(-Thickness, -Thickness));
    EmitVertex();
    gl_Position = fromScreenSpace(position + vec2(Thickness, -Thickness));
    EmitVertex();
    gl_Position =  fromScreenSpace(position + vec2(-Thickness, Thickness));
    EmitVertex();
    gl_Position = fromScreenSpace(position + vec2(Thickness, Thickness));
    EmitVertex();
    EndPrimitive();
}

void main() {    
//    buildPoint(0);
//    buildPoint(1);
//    buildPoint(2);
//    buildPoint(3);

    int nSegments = min(Segments, SegmentsMax);
    nSegments = max(nSegments, SegmentsMin);

    // 4 control points
    vec4 B[4];
    B[0] = gl_in[0].gl_Position;
    B[1] = gl_in[1].gl_Position;
    B[2] = gl_in[2].gl_Position;
    B[3] = gl_in[3].gl_Position;

    // 4 attached colors
    vec4 C[4];
    C[0] = VertexIn[0].color;
    C[1] = VertexIn[1].color;
    C[2] = VertexIn[2].color;
    C[3] = VertexIn[3].color;

    /* use the points to build a bezier line */
    float delta = 1.0 / float(nSegments);
    vec4 Points[4]; // segments of curve in 3d
    vec4 colors[4]; // interpolated colors
    float zValues[4];
    int j = 0; // bezier segment index for color interpolation

    // First Point

    for (int i = 1; i <= nSegments-1; ++i)
    {
        
    }

    // Last Point
}

//vec2 toScreenSpace(vec4 vertex)
//{
//    return vec2( vertex.xy / vertex.w ) * Viewport;
//}
//
//float toZValue(vec4 vertex)
//{
//    return (vertex.z/vertex.w);
//}
//
//void drawSegment(vec2 points[4], vec4 colors[4], float zValues[4])
//{
//    vec2 p0 = points[0];
//    vec2 p1 = points[1];
//    vec2 p2 = points[2];
//    vec2 p3 = points[3];
//
//    // Native Culling
//    // vec2 area = Viewport * 4;
//    // if( p1.x < -area.x || p1.x > area.x ) return;
//    // if( p1.y < -area.y || p1.y > area.y ) return;
//    // if( p2.x < -area.x || p2.x > area.x ) return;
//    // if( p2.y < -area.y || p2.y > area.y ) return;
//
//    /* determine the direction of each of the 3 segments (previous, current, next) */
//    vec2 v0 = normalize( p1 - p0 );
//    vec2 v1 = normalize( p2 - p1 );
//    vec2 v2 = normalize( p3 - p2 );
//
//    /* determine the normal of each of the 3 segments (previous, current, next) */
//    vec2 n0 = vec2( -v0.y, v0.x );
//    vec2 n1 = vec2( -v1.y, v1.x );
//    vec2 n2 = vec2( -v2.y, v2.x );
//
//    /* determine miter lines by averaging the normals of the 2 segments */
//    vec2 miter_a = normalize( n0 + n1 );	// miter at start of current segment
//    vec2 miter_b = normalize( n1 + n2 ); // miter at end of current segment
//
//    /* determine the length of the miter by projecting it onto normal and then inverse it */
//    float an1 = dot(miter_a, n1);
//    float bn1 = dot(miter_b, n2);
//    if (an1==0) an1 = 1;
//    if (bn1==0) bn1 = 1;
//    float length_a = Thickness / an1;
//    float length_b = Thickness / bn1;
//
//    /* prevent excessively long miters at sharp corners */
//    if( dot( v0, v1 ) < -MiterLimit ) {
//        miter_a = n1;
//        length_a = Thickness;
//
//        /* close the gap */
//        if( dot( v0, n1 ) > 0 ) {
//            VertexOut.mTexCoord = vec2( 0, 0 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( ( p1 + Thickness * n0 ) / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            VertexOut.mTexCoord = vec2( 0, 0 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( ( p1 + Thickness * n1 ) / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            VertexOut.mTexCoord = vec2( 0, 0.5 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( p1 / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            EndPrimitive();
//        }
//        else {
//            VertexOut.mTexCoord = vec2( 0, 1 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( ( p1 - Thickness * n1 ) / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            VertexOut.mTexCoord = vec2( 0, 1 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( ( p1 - Thickness * n0 ) / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            VertexOut.mTexCoord = vec2( 0, 0.5 );
//            VertexOut.mColor = colors[1];
//            gl_Position = vec4( p1 / Viewport, zValues[1], 1.0 );
//            EmitVertex();
//
//            EndPrimitive();
//        }
//    }
//    if( dot( v1, v2 ) < -MiterLimit ) {
//        miter_b = n1;
//        length_b = Thickness;
//    }
//    // generate the triangle strip
//    VertexOut.mTexCoord = vec2( 0, 0 );
//    VertexOut.mColor = colors[1];
//    gl_Position = vec4( ( p1 + length_a * miter_a ) / Viewport, zValues[1], 1.0 );
//    EmitVertex();
//
//    VertexOut.mTexCoord = vec2( 0, 1 );
//    VertexOut.mColor = colors[1];
//    gl_Position = vec4( ( p1 - length_a * miter_a ) / Viewport, zValues[1], 1.0 );
//    EmitVertex();
//
//    VertexOut.mTexCoord = vec2( 0, 0 );
//    VertexOut.mColor = colors[2];
//    gl_Position = vec4( ( p2 + length_b * miter_b ) / Viewport, zValues[2], 1.0 );
//    EmitVertex();
//
//    VertexOut.mTexCoord = vec2( 0, 1 );
//    VertexOut.mColor = colors[2];
//    gl_Position = vec4( ( p2 - length_b * miter_b ) / Viewport, zValues[2], 1.0 );
//    EmitVertex();
//
//    EndPrimitive();
//
//}
//
//void main(void)
//{
//    // 4 points
//    vec2 points[4];
//    points[0] = toScreenSpace(gl_in[0].gl_Position);
//    points[1] = toScreenSpace(gl_in[1].gl_Position);
//    points[2] = toScreenSpace(gl_in[2].gl_Position);
//    points[3] = toScreenSpace(gl_in[3].gl_Position);
//
//    // 4 attached colors
//    vec4 colors[4];
//    colors[0] = VertexIn[0].mColor;
//    colors[1] = VertexIn[1].mColor;
//    colors[2] = VertexIn[2].mColor;
//    colors[3] = VertexIn[3].mColor;
//
//        // deepness values
//    float zValues[4];
//    zValues[0] = toZValue(gl_in[0].gl_Position);
//    zValues[1] = toZValue(gl_in[1].gl_Position);
//    zValues[2] = toZValue(gl_in[2].gl_Position);
//    zValues[3] = toZValue(gl_in[3].gl_Position);
//
//    drawSegment(points, colors, zValues);
//}
//