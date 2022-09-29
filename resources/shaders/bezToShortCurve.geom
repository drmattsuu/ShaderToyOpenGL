#version 450 core

layout (location = 0) uniform float Thickness;
layout (location = 1) uniform vec2 Viewport;

layout (location = 2) uniform float MiterLimit = 0.1;
layout (location = 3) uniform int Segments = 30;

//layout (location = 2) uniform float Thickness;
//layout (location = 3) uniform vec2 Viewport;
//
//layout (location = 4) uniform float MiterLimit = 0.1;
//layout (location = 5) uniform int Segments = 30;

const int SegmentsMax = 30;
const int SegmentsMin = 10;

const int SegmentsArraySize = SegmentsMax+1;

const float MaxRadius = 25.0;

int nSegments;

in VertexData
{
    vec4 color;
} VertexIn[4];

out VertexData
{
    vec4 fColor;
} VertexOut;

layout (lines_adjacency) in;
layout (triangle_strip, max_vertices = 128) out;

vec2 toScreenSpace(vec4 vertex)
{
    return vec2(vertex.xy / vertex.w) * Viewport;
}

void drawSegment(vec2 points[4], vec4 colors[4])
{
    vec2 p0 = points[0];
    vec2 p1 = points[1];
    vec2 p2 = points[2];
    vec2 p3 = points[3];

    /* determine the direction of each of the 3 segments (previous, current, next) */
    vec2 v0 = normalize(p1 - p0);
    vec2 v1 = normalize(p2 - p1);
    vec2 v2 = normalize(p3 - p2);

    /* determine the normal of each of the 3 segments (previous, current, next) */
    vec2 n0 = vec2(-v0.y, v0.x);
    vec2 n1 = vec2(-v1.y, v1.x);
    vec2 n2 = vec2(-v2.y, v2.x);

    /* determine miter lines by averaging the normals of the 2 segments */
    vec2 miter_a = normalize(n0 + n1); // miter at start of current segment
    vec2 miter_b = normalize(n1 + n2); // miter at end of current segment

    /* determine the length of the miter by projecting it onto normal and then inverse it */
    float an1 = dot(miter_a, n1);
    float bn1 = dot(miter_b, n2);
    if (an1 == 0)
        an1 = 1;
    if (bn1 == 0)
        bn1 = 1;
    float length_a = Thickness / an1;
    float length_b = Thickness / bn1;

    /* prevent excessively long miters at sharp corners */
    if(dot(v0, v1) < -MiterLimit)
    {
        miter_a = n1;
        length_a = Thickness;

        /* close the gap */
        int gapNeedsClosing = int(dot(v0, n1) > 0); // used in place of if to determine values
        float thicknessDirection = float((gapNeedsClosing * 2) - 1); // used in place of the +/-
        vec2 thicknessNormals[2] = {n1, n0};
        vec2 firstNormal = thicknessNormals[gapNeedsClosing]; // used in place of n0/n1
        vec2 secondNormal = thicknessNormals[1 - gapNeedsClosing]; // used in place of n0/n1

        VertexOut.fColor = colors[1];
        gl_Position = vec4((p1 + (thicknessDirection * Thickness * firstNormal)) / Viewport, 0.0, 1.0);
        EmitVertex();

        VertexOut.fColor = colors[1];
        gl_Position = vec4((p1 + (thicknessDirection * Thickness * secondNormal)) / Viewport, 0.0, 1.0);
        EmitVertex();

        VertexOut.fColor = colors[1];
        gl_Position = vec4(p1 / Viewport, 0.0, 1.0);
        EmitVertex();

        EndPrimitive();
    }

    if(dot(v1, v2) < -MiterLimit)
    {
        miter_b = n1;
        length_b = Thickness;
    }

    // generate the triangle strip
    VertexOut.fColor = colors[1];
    gl_Position = vec4((p1 + length_a * miter_a) / Viewport, 0.0, 1.0);
    EmitVertex();

    VertexOut.fColor = colors[1];
    gl_Position = vec4((p1 - length_a * miter_a) / Viewport, 0.0, 1.0);
    EmitVertex();

    VertexOut.fColor = colors[2];
    gl_Position = vec4((p2 + length_b * miter_b ) / Viewport, 0.0, 1.0);
    EmitVertex();

    VertexOut.fColor = colors[2];
    gl_Position = vec4((p2 - length_b * miter_b ) / Viewport, 0.0, 1.0);
    EmitVertex();

    EndPrimitive();
}

void processSegment(int i, inout int j, int nSegments, vec2 source, vec2 dest, vec4 C[4], inout vec2 Points[4], inout vec4 colors[4])
{
    float totalLength = length(dest - source);
    vec2 currentPoint = Points[1];
    float currentLength = length(dest - currentPoint);

    float mixRatio = min(currentLength / totalLength, 1.0);

    colors[2] = mix(C[0], C[3], abs(mixRatio-1));

    drawSegment(Points, colors);
}

void addPoint(vec2 point, inout vec2 Points[SegmentsArraySize], inout int currentPoint)
{
    if (currentPoint != 0)
    {
        vec2 lastPoint = Points[currentPoint-1];
        if(point.x == lastPoint.x && point.y == lastPoint.y)
            return;
    }
    Points[currentPoint++] = point;
}

void getTangentLineL2R(vec2 o1, vec2 o2, float r1, float r2, bool sourceAboveDest,
    inout vec2 start, inout vec2 end)
{
    float dx = o2.x - o1.x;
    float dy = o2.y - o1.y;
    float d = sqrt(dx * dx + dy * dy);

    float c = (r1 + r2) / d;
    float c2 = c * c;
    if (c2 <= 1.0f)
    {
        float h = sqrt(max(0.0f, 1.0f - c2));
        float vx = dx / d;
        float vy = dy / d;
        float si = (sourceAboveDest) ? 1.0f : -1.0f;
        float nx = (vx * c - si * h * vy);
        float ny = (vy * c + si * h * vx);

        start = vec2(o1.x + (nx * r1), o1.y + (ny * r1));
        end = vec2(o2.x - (nx * r2), o2.y - (ny * r2));
    }
    else
    {
        start = o1;
        end = o2;
    }
}

void pathArc(const vec2 centre, float radius, float startAngle, float sweepAngle,
    inout vec2 Points[SegmentsArraySize], inout int currentPoint)
{
    int nArcSegments = (nSegments / 2)-3;
    float a = radians(sweepAngle / float(nArcSegments-1));
    float sa = radians(startAngle);

    for(int i = 0; i < nArcSegments; ++i)
    {
        float angle = i * a;
        vec2 point = centre;
        point.x = centre.x + cos(sa + angle) * radius;
        point.y = centre.y + sin(sa + angle) * radius;
        addPoint(point, Points, currentPoint);
    }
}

void pathArcFromOutput(vec2 tanLine, vec2 source, float radius,
    bool sourceAboveDest, inout vec2 Points[SegmentsArraySize], inout int currentPoint)
{
    bool xNeg = source.x > tanLine.x;
    bool yNeg = source.y > tanLine.y;
    float delta = (source.x - tanLine.x) / radius;
    float angle = degrees(asin(delta));
    float arcAngle = 0.f;
    if (!xNeg && !yNeg && !sourceAboveDest)
        arcAngle = 90.0f + (90.0f + angle);
    else if (!xNeg && yNeg && !sourceAboveDest)
        arcAngle = -angle;
    else if (xNeg && !yNeg && !sourceAboveDest)
        arcAngle = 180.0f + angle;
    else if (!xNeg && yNeg && sourceAboveDest)
        arcAngle = -(180.0f + angle);
    else if (!xNeg && !yNeg && sourceAboveDest)
        arcAngle = angle;
    else if (xNeg && yNeg && sourceAboveDest)
        arcAngle = -180 - angle;

    float si = sourceAboveDest ? 1.f : -1.f;
    pathArc(source, radius, si * 90.f, arcAngle, Points, currentPoint);
}

void pathArcToInput(vec2 tanLine, vec2 dest, float radius,
    bool sourceAboveDest, inout vec2 Points[SegmentsArraySize], inout int currentPoint)
{
    bool xNeg = dest.x > tanLine.x;
    bool yNeg = dest.y > tanLine.y;
    float delta = (dest.x - tanLine.x) / radius;
    float angle = degrees(asin(delta));
    float arcAngle = 0.f;
    if (xNeg && !yNeg && !sourceAboveDest)
        arcAngle = angle;
    else if (xNeg && yNeg && !sourceAboveDest)
        arcAngle = 90.0f + (90.0f - angle);
    else if (!xNeg && yNeg && !sourceAboveDest)
        arcAngle = 90.0f + (90.0f - angle);
    else if (!xNeg && !yNeg && sourceAboveDest)
        arcAngle = -180.0f + angle;
    else if (xNeg && !yNeg && sourceAboveDest)
        arcAngle = -(180.0f - angle);
    else if (xNeg && yNeg && sourceAboveDest)
        arcAngle = -angle;

    float si = sourceAboveDest ? -1.f : 1.f;
    pathArc(dest, radius, (si * 90.0) + arcAngle, -arcAngle, Points, currentPoint);
}

// TODO : There is a lot of branching in here that we can probably improve upon, but for the sake
// of simplicity (and because this shader is not the common case) I've decided to leave it alone
// for now. 
void getTangentLinesL2R(vec2 source, vec2 dest, float sourceRadius, float destRadius,
     inout vec2 Points[SegmentsArraySize], inout int currentPoint)
{
    vec2 posDelta = dest - source;
    float sumRadius2 = (sourceRadius + destRadius) * 2.0;

    bool sourceCurve = sourceRadius > 0.1f;
    bool destCurve = destRadius > 0.1f;

    // In cases where the Y-distance between the start and end of a connection is smaller than the
    // sum of the two requrested arc diameters, we can do some extra work to restrict the radius of
    // the arc. Ignored for radii of 0.
    if(abs(posDelta[1]) < sumRadius2)
    {
        float ForwardFactor = mix(0.5f, 1.0f / 3.0f, 0.0f);
        float YDist = abs(posDelta.y) * ForwardFactor;
        float RDist = length(posDelta) * ForwardFactor;
        float MinDist = min(YDist, RDist);

        if (sourceCurve)
            sourceRadius = min(MinDist, MaxRadius);
        if (destCurve)
            destRadius = min(MinDist, MaxRadius);
    }
    
    vec2 offsetSrc = source;
    vec2 offsetDest = dest;

    bool sourceAboveDest = (posDelta.y < 0.0f);
    if (sourceAboveDest)
    {
        offsetSrc.y -= sourceRadius;
        offsetDest.y += destRadius;
    }
    else
    {
        offsetSrc.y += sourceRadius;
        offsetDest.y -= destRadius;
    }

    vec2 tanLineStart;
    vec2 tanLineEnd;
    getTangentLineL2R(offsetSrc, offsetDest, sourceRadius, destRadius, sourceAboveDest,
        tanLineStart, tanLineEnd);
        
    vec2 start = sourceCurve ? tanLineStart : source;
    vec2 end = destCurve ? tanLineEnd : dest;

    if(sourceCurve)
    {
        pathArcFromOutput(tanLineStart, offsetSrc, sourceRadius, sourceAboveDest, Points, currentPoint);
    }
    else
    {
        addPoint(source, Points, currentPoint);
    }
    
    // seed some points in the middle to help with the transition of colors
    addPoint(start + (end - start) * 0.2, Points, currentPoint);
    addPoint(start + (end - start) * 0.4, Points, currentPoint);
    addPoint(start + (end - start) * 0.6, Points, currentPoint);
    addPoint(start + (end - start) * 0.8, Points, currentPoint);

    if(destCurve)
    {
        pathArcToInput(tanLineEnd, offsetDest, destRadius, sourceAboveDest, Points, currentPoint);
    }
    else
    {
        addPoint(dest, Points, currentPoint);
    }
}

void main(void)
{
    /* cut segments number if larger or smaller than allowed */
    nSegments = min(Segments, SegmentsMax);
    nSegments = max(nSegments, SegmentsMin);

    // 2 control points
    vec2 controls[2];
    controls[0] = toScreenSpace(gl_in[0].gl_Position);
    controls[1] = toScreenSpace(gl_in[3].gl_Position);

    // 2 positions where the tangent meets the expected arc
    vec2 tangents[2];
    tangents[0] = toScreenSpace(gl_in[1].gl_Position);
    tangents[1] = toScreenSpace(gl_in[2].gl_Position);

    // 4 attached colors
    vec4 C[4];
    C[0] = VertexIn[0].color;
    C[1] = VertexIn[1].color;
    C[2] = VertexIn[2].color;
    C[3] = VertexIn[3].color;

    float startRadius = length(tangents[0] - controls[0]);
    float endRadius = length(tangents[1] - controls[1]);
    
    startRadius = min(startRadius, MaxRadius);
    endRadius = min(endRadius, MaxRadius);

    vec2 linePoints[SegmentsArraySize];
    for (int i = 0; i < SegmentsArraySize; ++i)
    {
        linePoints[i] = vec2(0.0);
    }
    int currentPoint = 0;

    getTangentLinesL2R(controls[0], controls[1], startRadius, endRadius, linePoints, currentPoint);

    int maxPoint = currentPoint-1;

    float delta = 1.0 / float(maxPoint);
    vec2 Points[4] = vec2[](vec2(0.0),vec2(0.0),vec2(0.0),vec2(0.0)); // segments of curve in 3d
    vec4 colors[4] = vec4[](vec4(0.0),vec4(0.0),vec4(0.0),vec4(0.0)); // interpolated colors
    int j = 0;

    // First Point
    Points[1] = linePoints[0];
    Points[2] = linePoints[1];
    Points[3] = linePoints[2];
    vec2 dir = normalize(Points[2] - Points[1]);
    Points[0] = Points[1] + dir * 0.01;
    /* color interpolation: No color interpolation for the first point*/
    colors[1] = C[0];

    processSegment(0, j, maxPoint, controls[0], controls[1], C, Points, colors);

    for (int i = 1; i < maxPoint-1; ++i)
    {
        Points[0] = Points[1];
        Points[1] = Points[2];
        Points[2] = Points[3];
        Points[3] = linePoints[i+2];

        if (Points[0].x == Points[1].x && Points[0].y == Points[1].y)
            continue;

        /* color interpolation: define which bezier segment the point belongs to and then interpolate
        between the two colors of that segment */
        colors[1] = colors[2];
        processSegment(i, j, maxPoint, controls[0], controls[1], C, Points, colors);
    }

    /* last point */
    Points[0] = Points[1];
    Points[1] = Points[2];
    Points[2] = Points[3];
    dir = normalize(Points[2] - Points[1]);
    Points[3] = Points[2] + dir * 0.01;
    /* color interpolation: define which bezier segment the point belongs to and then interpolate
    between the two colors of that segment */
    colors[1] = colors[2];

    processSegment(maxPoint-1, j, maxPoint, controls[0], controls[1], C, Points, colors);
}