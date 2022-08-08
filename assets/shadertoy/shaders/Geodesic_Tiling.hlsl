float3              iMouse;
float4              iResolution;
float               iTime;
float               iFrame;
RWTexture2D<float4> outImage : register(u1);

#define fract     frac
#define mix       lerp
#define mod(x, y) (x - y * floor(x / y))

#define MODEL_ROTATION  float2(.3, .25)
#define CAMERA_ROTATION float2(.5, .5)

// 0: Defaults
// 1: Model
// 2: Camera
#define MOUSE_CONTROL 1

//#define DEBUG

// 1, 2, or 3
//#define LOOP 1

// --------------------------------------------------------
// HG_SDF
// https://www.shadertoy.com/view/Xs3GRB
// --------------------------------------------------------

void pR(inout float2 p, float a)
{
    p = cos(a) * p + sin(a) * float2(p.y, -p.x);
}

float pReflect(inout float3 p, float3 planeNormal, float offset)
{
    float t = dot(p, planeNormal) + offset;
    if (t < 0.) {
        p = p - (2. * t) * planeNormal;
    }
    return sign(t);
}

float smax(float a, float b, float r)
{
    float m = max(a, b);
    if ((-a < r) && (-b < r)) {
        return max(m, -(r - sqrt((r + a) * (r + a) + (r + b) * (r + b))));
    }
    else {
        return m;
    }
}

// --------------------------------------------------------
// Icosahedron domain mirroring
// Adapted from knighty https://www.shadertoy.com/view/MsKGzw
// --------------------------------------------------------

#define PI 3.14159265359

static float3 facePlane;
static float3 uPlane;
static float3 vPlane;

static int    Type = 5;
static float3 nc;
static float3 pab;
static float3 pbc;
static float3 pca;

void initIcosahedron()
{ //setup folding planes and vertex
    float cospin = cos(PI / float(Type)), scospin = sqrt(0.75 - cospin * cospin);
    nc  = float3(-0.5, -cospin, scospin); //3rd folding plane. The two others are xz and yz planes
    pbc = float3(scospin, 0., 0.5);       //No normalization in order to have 'barycentric' coordinates work evenly
    pca = float3(0., scospin, cospin);
    pbc = normalize(pbc);
    pca = normalize(pca); //for slightly better DE. In reality it's not necesary to apply normalization :)
    pab = float3(0, 0, 1);

    facePlane = pca;
    uPlane    = cross(float3(1, 0, 0), facePlane);
    vPlane    = float3(1, 0, 0);
}

void pModIcosahedron(inout float3 p)
{
    p = abs(p);
    pReflect(p, nc, 0.);
    p.xy = abs(p.xy);
    pReflect(p, nc, 0.);
    p.xy = abs(p.xy);
    pReflect(p, nc, 0.);
}

// --------------------------------------------------------
// Triangle tiling
// Adapted from mattz https://www.shadertoy.com/view/4d2GzV
// --------------------------------------------------------

static const float sqrt3 = 1.7320508075688772;
static const float i3    = 0.5773502691896258;

static const float2x2 cart2hex = float2x2(1, 0, i3, 2. * i3);
static const float2x2 hex2cart = float2x2(1, 0, -.5, .5 * sqrt3);

#define PHI (1.618033988749895)
#define TAU 6.283185307179586

struct TriPoints
{
    float2 a;
    float2 b;
    float2 c;
    float2 center;
    float2 ab;
    float2 bc;
    float2 ca;
};

TriPoints MakeTriPoints(
    float2 a,
    float2 b,
    float2 c,
    float2 center,
    float2 ab,
    float2 bc,
    float2 ca)
{
    TriPoints obj = (TriPoints)0;
    obj.a         = a;
    obj.b         = b;
    obj.c         = c;
    obj.center    = center;
    obj.ab        = ab;
    obj.bc        = bc;
    obj.ca        = ca;
    return obj;
}

TriPoints closestTriPoints(float2 p)
{
    float2 pTri = mul(cart2hex, p);
    float2 pi   = floor(pTri);
    float2 pf   = fract(pTri);

    float split1 = step(pf.y, pf.x);
    float split2 = step(pf.x, pf.y);

    float2 a = float2(split1, 1);
    float2 b = float2(1, split2);
    float2 c = float2(0, 0);

    a += pi;
    b += pi;
    c += pi;

    a = mul(hex2cart, a);
    b = mul(hex2cart, b);
    c = mul(hex2cart, c);

    float2 center = (a + b + c) / 3.;

    float2 ab = (a + b) / 2.;
    float2 bc = (b + c) / 2.;
    float2 ca = (c + a) / 2.;

    return MakeTriPoints(a, b, c, center, ab, bc, ca);
}

// --------------------------------------------------------
// Geodesic tiling
// --------------------------------------------------------

struct TriPoints3D
{
    float3 a;
    float3 b;
    float3 c;
    float3 center;
    float3 ab;
    float3 bc;
    float3 ca;
};

TriPoints3D MakeTriPoints3D(
    float3 a,
    float3 b,
    float3 c,
    float3 center,
    float3 ab,
    float3 bc,
    float3 ca)
{
    TriPoints3D obj = (TriPoints3D)0;
    obj.a           = a;
    obj.b           = b;
    obj.c           = c;
    obj.center      = center;
    obj.ab          = ab;
    obj.bc          = bc;
    obj.ca          = ca;
    return obj;
}

float3 intersection(float3 n, float3 planeNormal, float planeOffset)
{
    float denominator = dot(planeNormal, n);
    float t           = (dot((float3)0, planeNormal) + planeOffset) / -denominator;
    return n * t;
}

//// Edge length of an icosahedron with an inscribed sphere of radius of 1
//float edgeLength = 1. / ((sqrt(3.) / 12.) * (3. + sqrt(5.)));
//// Inner radius of the icosahedron's face
//float faceRadius = (1./6.) * sqrt(3.) * edgeLength;
static float faceRadius = 0.3819660112501051;

// 2D coordinates on the icosahedron face
float2 icosahedronFaceCoordinates(float3 p)
{
    float3 pn = normalize(p);
    float3 i  = intersection(pn, facePlane, -1.);
    return float2(dot(i, uPlane), dot(i, vPlane));
}

// Project 2D icosahedron face coordinates onto a sphere
float3 faceToSphere(float2 facePoint)
{
    return normalize(facePlane + (uPlane * facePoint.x) + (vPlane * facePoint.y));
}

TriPoints3D geodesicTriPoints(float3 p, float subdivisions)
{
    // Get 2D cartesian coordiantes on that face
    float2 uv = icosahedronFaceCoordinates(p);

    // Get points on the nearest triangle tile
    float     uvScale = subdivisions / faceRadius / 2.;
    TriPoints points  = closestTriPoints(uv * uvScale);

    // Project 2D triangle coordinates onto a sphere
    float3 a      = faceToSphere(points.a / uvScale);
    float3 b      = faceToSphere(points.b / uvScale);
    float3 c      = faceToSphere(points.c / uvScale);
    float3 center = faceToSphere(points.center / uvScale);
    float3 ab     = faceToSphere(points.ab / uvScale);
    float3 bc     = faceToSphere(points.bc / uvScale);
    float3 ca     = faceToSphere(points.ca / uvScale);

    return MakeTriPoints3D(a, b, c, center, ab, bc, ca);
}

// --------------------------------------------------------
// Spectrum colour palette
// IQ https://www.shadertoy.com/view/ll2GD3
// --------------------------------------------------------

float3 pal(in float t, in float3 a, in float3 b, in float3 c, in float3 d)
{
    return a + b * cos(6.28318 * (c * t + d));
}

float3 spectrum(float n)
{
    return pal(n, float3(0.5, 0.5, 0.5), float3(0.5, 0.5, 0.5), float3(1.0, 1.0, 1.0), float3(0.0, 0.33, 0.67));
}

// --------------------------------------------------------
// Model/Camera Rotation
// --------------------------------------------------------

float3x3 sphericalMatrix(float theta, float phi)
{
    float cx = cos(theta);
    float cy = cos(phi);
    float sx = sin(theta);
    float sy = sin(phi);
    return float3x3(
        cy, -sy * -sx, -sy * cx, 0, cx, sx, sy, cy * -sx, cy * cx);
}

float3x3 mouseRotation(bool enable, float2 xy)
{
    if (enable) {
        float2 mouse = iMouse.xy / iResolution.xy;

        if (mouse.x != 0. && mouse.y != 0.) {
            xy.x = mouse.x;
            xy.y = mouse.y;
        }
    }
    float rx, ry;

    rx = (xy.y + .5) * PI;
    ry = (-xy.x) * 2. * PI;

    return sphericalMatrix(rx, ry);
}

float3x3 modelRotation()
{
    float3x3 m = mouseRotation(MOUSE_CONTROL == 1, MODEL_ROTATION);
    return m;
}

float3x3 cameraRotation()
{
    float3x3 m = mouseRotation(MOUSE_CONTROL == 2, CAMERA_ROTATION);
    return m;
}

// --------------------------------------------------------
// Animation
// --------------------------------------------------------

static const float SCENE_DURATION     = 6.;
static const float CROSSFADE_DURATION = 2.;

static float time;

struct HexSpec
{
    float roundTop;
    float roundCorner;
    float height;
    float thickness;
    float gap;
};

HexSpec MakeHexSpec(
    float roundTop,
    float roundCorner,
    float height,
    float thickness,
    float gap)
{
    HexSpec obj     = (HexSpec)0;
    obj.roundTop    = roundTop;
    obj.roundCorner = roundCorner;
    obj.height      = height;
    obj.thickness   = thickness;
    obj.gap         = gap;
    return obj;
}

HexSpec newHexSpec(float subdivisions)
{
    return MakeHexSpec(
        .05 / subdivisions,
        .1 / subdivisions,
        2.,
        2.,
        .005);
}

// Animation 1

float animSubdivisions1()
{
    return mix(2.4, 3.4, cos(time * PI) * .5 + .5);
}

HexSpec animHex1(float3 hexCenter, float subdivisions)
{
    HexSpec spec = newHexSpec(subdivisions);

    float offset = time * 3. * PI;
    offset -= subdivisions;
    float blend = dot(hexCenter, pca);
    blend       = cos(blend * 30. + offset) * .5 + .5;
    spec.height = mix(1.75, 2., blend);

    spec.thickness = spec.height;

    return spec;
}

// Animation 2

float animSubdivisions2()
{
    return mix(1., 2.3, sin(time * PI / 2.) * .5 + .5);
}

HexSpec animHex2(float3 hexCenter, float subdivisions)
{
    HexSpec spec = newHexSpec(subdivisions);

    float blend = hexCenter.y;
    spec.height = mix(1.6, 2., sin(blend * 10. + time * PI) * .5 + .5);

    spec.roundTop    = .02 / subdivisions;
    spec.roundCorner = .09 / subdivisions;
    spec.thickness   = spec.roundTop * 4.;
    spec.gap         = .01;

    return spec;
}

// Animation 3

float animSubdivisions3()
{
    return 5.;
}

HexSpec animHex3(float3 hexCenter, float subdivisions)
{
    HexSpec spec = newHexSpec(subdivisions);

    float blend = acos(dot(hexCenter, pab)) * 10.;
    blend       = cos(blend + time * PI) * .5 + .5;
    spec.gap    = mix(.01, .4, blend) / subdivisions;

    spec.thickness = spec.roundTop * 2.;

    return spec;
}

// Transition between animations

float sineInOut(float t)
{
    return -0.5 * (cos(PI * t) - 1.0);
}

float transitionValues(float a, float b, float c)
{
#ifdef LOOP
#if LOOP == 1
    return a;
#endif
#if LOOP == 2
    return b;
#endif
#if LOOP == 3
    return c;
#endif
#endif
    float t      = time / SCENE_DURATION;
    float scene  = floor(mod(t, 3.));
    float blend  = fract(t);
    float delay  = (SCENE_DURATION - CROSSFADE_DURATION) / SCENE_DURATION;
    blend        = max(blend - delay, 0.) / (1. - delay);
    blend        = sineInOut(blend);
    float ab     = mix(a, b, blend);
    float bc     = mix(b, c, blend);
    float cd     = mix(c, a, blend);
    float result = mix(ab, bc, min(scene, 1.));
    result       = mix(result, cd, max(scene - 1., 0.));
    return result;
}

HexSpec transitionHexSpecs(HexSpec a, HexSpec b, HexSpec c)
{
    float roundTop    = transitionValues(a.roundTop, b.roundTop, c.roundTop);
    float roundCorner = transitionValues(a.roundCorner, b.roundCorner, c.roundCorner);
    float height      = transitionValues(a.height, b.height, c.height);
    float thickness   = transitionValues(a.thickness, b.thickness, c.thickness);
    float gap         = transitionValues(a.gap, b.gap, c.gap);
    return MakeHexSpec(roundTop, roundCorner, height, thickness, gap);
}

// --------------------------------------------------------
// Modelling
// --------------------------------------------------------

static const float3 FACE_COLOR       = float3(.9, .9, 1.);
static const float3 BACK_COLOR       = float3(.1, .1, .15);
static const float3 BACKGROUND_COLOR = float3(.0, .005, .03);

struct Model
{
    float  dist;
    float3 albedo;
    float  glow;
};

Model MakeModel(
    float  dist,
    float3 albedo,
    float  glow)
{
    Model obj  = (Model)0;
    obj.dist   = dist;
    obj.albedo = albedo;
    obj.glow   = glow;
    return obj;
}

Model hexModel(
    float3  p,
    float3  hexCenter,
    float3  edgeA,
    float3  edgeB,
    HexSpec spec)
{
    float d;

    float edgeADist = dot(p, edgeA) + spec.gap;
    float edgeBDist = dot(p, edgeB) - spec.gap;
    float edgeDist  = smax(edgeADist, -edgeBDist, spec.roundCorner);

    float outerDist = length(p) - spec.height;
    d               = smax(edgeDist, outerDist, spec.roundTop);

    float innerDist = length(p) - spec.height + spec.thickness;
    d               = smax(d, -innerDist, spec.roundTop);

    float3 color;

    float faceBlend = (spec.height - length(p)) / spec.thickness;
    faceBlend       = clamp(faceBlend, 0., 1.);
    color           = mix(FACE_COLOR, BACK_COLOR, step(.5, faceBlend));

    float3 edgeColor = spectrum(dot(hexCenter, pca) * 5. + length(p) + .8);
    float  edgeBlend = smoothstep(-.04, -.005, edgeDist);
    color            = mix(color, edgeColor, edgeBlend);

    return MakeModel(d, color, edgeBlend);
}

// checks to see which intersection is closer
Model opU(Model m1, Model m2)
{
    if (m1.dist < m2.dist) {
        return m1;
    }
    else {
        return m2;
    }
}

Model geodesicModel(float3 p)
{
    pModIcosahedron(p);

    float subdivisions = transitionValues(
        animSubdivisions1(),
        animSubdivisions2(),
        animSubdivisions3());
    TriPoints3D points = geodesicTriPoints(p, subdivisions);

    float3 edgeAB = normalize(cross(points.center, points.ab));
    float3 edgeBC = normalize(cross(points.center, points.bc));
    float3 edgeCA = normalize(cross(points.center, points.ca));

    Model   model, part;
    HexSpec spec;

    spec = transitionHexSpecs(
        animHex1(points.b, subdivisions),
        animHex2(points.b, subdivisions),
        animHex3(points.b, subdivisions));
    part  = hexModel(p, points.b, edgeAB, edgeBC, spec);
    model = part;

    spec = transitionHexSpecs(
        animHex1(points.c, subdivisions),
        animHex2(points.c, subdivisions),
        animHex3(points.c, subdivisions));
    part  = hexModel(p, points.c, edgeBC, edgeCA, spec);
    model = opU(model, part);

    spec = transitionHexSpecs(
        animHex1(points.a, subdivisions),
        animHex2(points.a, subdivisions),
        animHex3(points.a, subdivisions));
    part  = hexModel(p, points.a, edgeCA, edgeAB, spec);
    model = opU(model, part);

    return model;
}

Model map(float3 p)
{
    float3x3 m = modelRotation();
    //p *= m;
    p = mul(m, p);
#ifndef LOOP
    pR(p.xz, time * PI / 16.);
#endif
    Model model = geodesicModel(p);
    return model;
}

// --------------------------------------------------------
// LIGHTING
// Adapted from IQ https://www.shadertoy.com/view/Xds3zN
// --------------------------------------------------------

float3 doLighting(Model model, float3 pos, float3 nor, float3 ref, float3 rd)
{
    float3 lightPos     = normalize(float3(.5, .5, -1.));
    float3 backLightPos = normalize(float3(-.5, -.3, 1));
    float3 ambientPos   = float3(0, 1, 0);

    float3 lig = lightPos;
    float  amb = clamp((dot(nor, ambientPos) + 1.) / 2., 0., 1.);
    float  dif = clamp(dot(nor, lig), 0.0, 1.0);
    float  bac = pow(clamp(dot(nor, backLightPos), 0., 1.), 1.5);
    float  fre = pow(clamp(1.0 + dot(nor, rd), 0.0, 1.0), 2.0);

    float3 lin = (float3)0.0;
    lin += 1.20 * dif * (float3).9;
    lin += 0.80 * amb * float3(.5, .7, .8);
    lin += 0.30 * bac * (float3).25;
    lin += 0.20 * fre * (float3)1;

    float3 albedo = model.albedo;
    float3 col    = mix(albedo * lin, albedo, model.glow);

    return col;
}

// --------------------------------------------------------
// Ray Marching
// Adapted from cabbibo https://www.shadertoy.com/view/Xl2XWt
// --------------------------------------------------------

static const float MAX_TRACE_DISTANCE     = 8.;   // max trace distance
static const float INTERSECTION_PRECISION = .001; // precision of the intersection
static const int   NUM_OF_TRACE_STEPS     = 100;
static const float FUDGE_FACTOR           = .9; // Default is 1, reduce to fix overshoots

struct CastRay
{
    float3 origin;
    float3 direction;
};

struct Ray
{
    float3 origin;
    float3 direction;
    float  len;
};

struct Hit
{
    Ray    ray;
    Model  model;
    float3 pos;
    bool   isBackground;
    float3 normal;
    float3 color;
};

CastRay MakeCastRay(
    float3 origin,
    float3 direction)
{
    CastRay obj   = (CastRay)0;
    obj.origin    = origin;
    obj.direction = direction;
    return obj;
}

Ray MakeRay(
    float3 origin,
    float3 direction,
    float  len)
{
    Ray obj       = (Ray)0;
    obj.origin    = origin;
    obj.direction = direction;
    obj.len       = len;
    return obj;
}

Hit MakeHit(
    Ray    ray,
    Model  model,
    float3 pos,
    bool   isBackground,
    float3 normal,
    float3 color)
{
    Hit obj          = (Hit)0;
    obj.ray          = ray;
    obj.model        = model;
    obj.pos          = pos;
    obj.isBackground = isBackground;
    obj.normal       = normal;
    obj.color        = color;
    return obj;
}

float3 calcNormal(in float3 pos)
{
    float3 eps = float3(0.001, 0.0, 0.0);
    float3 nor = float3(
        map(pos + eps.xyy).dist - map(pos - eps.xyy).dist,
        map(pos + eps.yxy).dist - map(pos - eps.yxy).dist,
        map(pos + eps.yyx).dist - map(pos - eps.yyx).dist);
    return normalize(nor);
}

Hit raymarch(CastRay castRay)
{
    float currentDist = INTERSECTION_PRECISION * 2.0;
    Model model;

    Ray ray = MakeRay(castRay.origin, castRay.direction, 0.);

    for (int i = 0; i < NUM_OF_TRACE_STEPS; i++) {
        if (currentDist < INTERSECTION_PRECISION || ray.len > MAX_TRACE_DISTANCE) {
            break;
        }
        model       = map(ray.origin + ray.direction * ray.len);
        currentDist = model.dist;
        ray.len += currentDist * FUDGE_FACTOR;
    }

    bool   isBackground = false;
    float3 pos          = (float3)0;
    float3 normal       = (float3)0;
    float3 color        = (float3)0;

    if (ray.len > MAX_TRACE_DISTANCE) {
        isBackground = true;
    }
    else {
        pos    = ray.origin + ray.direction * ray.len;
        normal = calcNormal(pos);
    }

    return MakeHit(ray, model, pos, isBackground, normal, color);
}

// --------------------------------------------------------
// Rendering
// --------------------------------------------------------

void shadeSurface(inout Hit hit)
{
    float3 color = BACKGROUND_COLOR;

    if (hit.isBackground) {
        hit.color = color;
        return;
    }

    float3 ref = reflect(hit.ray.direction, hit.normal);

#ifdef DEBUG
    color = hit.normal * 0.5 + 0.5;
#else
    color = doLighting(
        hit.model,
        hit.pos,
        hit.normal,
        ref,
        hit.ray.direction);
#endif

    hit.color = color;
}

float3 render(Hit hit)
{
    shadeSurface(hit);
    return hit.color;
}

// --------------------------------------------------------
// Camera
// https://www.shadertoy.com/view/Xl2XWt
// --------------------------------------------------------

float3x3 calcLookAtMatrix(in float3 ro, in float3 ta, in float roll)
{
    float3 ww = normalize(ta - ro);
    float3 uu = normalize(cross(ww, float3(sin(roll), cos(roll), 0.0)));
    float3 vv = normalize(cross(uu, ww));
    return float3x3(uu, vv, ww);
}

void doCamera(out float3 camPos, out float3 camTar, out float camRoll, in float time, in float2 mouse)
{
    float dist = 5.5;
    camRoll    = 0.;
    camTar     = float3(0, 0, 0);
    camPos     = float3(0, 0, -dist);
    //camPos *= cameraRotation();
    camPos = mul(cameraRotation(), camPos);
    camPos += camTar;
}

// --------------------------------------------------------
// Gamma
// https://www.shadertoy.com/view/Xds3zN
// --------------------------------------------------------

static const float GAMMA = 2.2;

float3 gamma(float3 color, float g)
{
    return pow(color, (float3)g);
}

float3 linearToScreen(float3 linearRGB)
{
    return gamma(linearRGB, 1.0 / GAMMA);
}

void mainImage(out float4 fragColor, in float2 fragCoord)
{
    time = iTime;

#ifdef LOOP
#if LOOP == 1
    time = mod(time, 2.);
#endif
#if LOOP == 2
    time = mod(time, 4.);
#endif
#if LOOP == 3
    time = mod(time, 2.);
#endif
#endif

    initIcosahedron();

    float2 p = (-iResolution.xy + 2.0 * fragCoord.xy) / iResolution.y;
    float2 m = iMouse.xy / iResolution.xy;

    float3 camPos  = float3(0., 0., 2.);
    float3 camTar  = float3(0., 0., 0.);
    float  camRoll = 0.;

    // camera movement
    doCamera(camPos, camTar, camRoll, iTime, m);

    // camera matrix
    float3x3 camMat = calcLookAtMatrix(camPos, camTar, camRoll); // 0.0 is the camera roll

    // create view ray
    //float3 rd = normalize( camMat * float3(p.xy,2.0) ); // 2.0 is the lens length
    float3 rd = normalize(mul(camMat, float3(p.xy, 2.0))); // 2.0 is the lens length

    Hit hit = raymarch(MakeCastRay(camPos, rd));

    float3 color = render(hit);

#ifndef DEBUG
    color = linearToScreen(color);
#endif

    fragColor = float4(color, 1.0);
}

[numthreads(8, 8, 1)] void csmain(uint3 tid
                                  : SV_DispatchThreadID) {
    float4 outColor  = (float4)0;
    float2 fragCoord = float2(tid.x, iResolution.y - tid.y) + float2(0.5, 0.5);
    mainImage(outColor, fragCoord);
    outImage[tid.xy] = outColor;
}