float3              iMouse;
float4              iResolution;
float               iTime;
float               iFrame;
RWTexture2D<float4> outImage : register(u1);

#define fract     frac
#define mix       lerp
#define mod(x, y) (x - y * floor(x / y))

float2 make_float2(float s)
{
    return float2(s, s);
}
float3 make_float3(float s)
{
    return float3(s, s, s);
}

// "Fractal Cartoon" - former "DE edge detection" by Kali

// There are no lights and no AO, only color by normals and dark edges.

// update: Nyan Cat cameo, thanks to code from mu6k: https://www.shadertoy.com/view/4dXGWH

//#define SHOWONLYEDGES
#define NYAN
#define WAVES
#define BORDER

#define RAY_STEPS 150

#define BRIGHTNESS 1.2
#define GAMMA      1.4
#define SATURATION .65

#define detail .001
#define t      iTime * .5

static const float3 origin = float3(-1., .7, 0.);
static float        det    = 0.0;

// 2D rotation function
float2x2 rot(float a)
{
    return float2x2(cos(a), sin(a), -sin(a), cos(a));
}

// "Amazing Surface" fractal
float4 formula(float4 p)
{
    p.xz = abs(p.xz + 1.) - abs(p.xz - 1.) - p.xz;
    p.y -= .25;
    //p.xy*=rot(radians(35.));
    p.xy = mul(rot(radians(35.)), p.xy);
    p    = p * 2. / clamp(dot(p.xyz, p.xyz), .2, 1.);
    return p;
}

// Distance function
float de(float3 pos)
{
#ifdef WAVES
    pos.y += sin(pos.z - t * 6.) * .15; //waves!
#endif
    float  hid  = 0.;
    float3 tpos = pos;
    tpos.z      = abs(3. - mod(tpos.z, 6.));
    float4 p    = float4(tpos, 1.);
    for (int i = 0; i < 4; i++) {
        p = formula(p);
    }
    float fr = (length(max(make_float2(0.), p.yz - 1.5)) - 1.) / p.w;
    float ro = max(abs(pos.x + 1.) - .3, pos.y - .35);
    ro       = max(ro, -max(abs(pos.x + 1.) - .1, pos.y - .5));
    pos.z    = abs(.25 - mod(pos.z, .5));
    ro       = max(ro, -max(abs(pos.z) - .2, pos.y - .3));
    ro       = max(ro, -max(abs(pos.z) - .01, -pos.y + .32));
    float d  = min(fr, ro);
    return d;
}

// Camera path
float3 path(float ti)
{
    ti *= 1.5;
    float3 p = float3(sin(ti), (1. - sin(ti * 2.)) * .5, -ti * 5.) * .5;
    return p;
}

// Calc normals, and here is edge detection, set to variable "edge"

static float edge = 0.;
float3       normal(float3 p)
{
    float3 e = float3(0.0, det * 5., 0.0);

    float d1 = de(p - e.yxx), d2 = de(p + e.yxx);
    float d3 = de(p - e.xyx), d4 = de(p + e.xyx);
    float d5 = de(p - e.xxy), d6 = de(p + e.xxy);
    float d = de(p);
    edge    = abs(d - 0.5 * (d2 + d1)) + abs(d - 0.5 * (d4 + d3)) + abs(d - 0.5 * (d6 + d5)); //edge finder
    edge    = min(1., pow(edge, .55) * 15.);
    return normalize(float3(d1 - d2, d3 - d4, d5 - d6));
}

// Used Nyan Cat code by mu6k, with some mods

float4 rainbow(float2 p)
{
    float q = max(p.x, -0.1);
    float s = sin(p.x * 7.0 + t * 70.0) * 0.08;
    p.y += s;
    p.y *= 1.1;

    float4 c;
    if (p.x > 0.0)
        c = float4(0, 0, 0, 0);
    else if (0.0 / 6.0 < p.y && p.y < 1.0 / 6.0)
        c = float4(255, 43, 14, 255) / 255.0;
    else if (1.0 / 6.0 < p.y && p.y < 2.0 / 6.0)
        c = float4(255, 168, 6, 255) / 255.0;
    else if (2.0 / 6.0 < p.y && p.y < 3.0 / 6.0)
        c = float4(255, 244, 0, 255) / 255.0;
    else if (3.0 / 6.0 < p.y && p.y < 4.0 / 6.0)
        c = float4(51, 234, 5, 255) / 255.0;
    else if (4.0 / 6.0 < p.y && p.y < 5.0 / 6.0)
        c = float4(8, 163, 255, 255) / 255.0;
    else if (5.0 / 6.0 < p.y && p.y < 6.0 / 6.0)
        c = float4(122, 85, 255, 255) / 255.0;
    else if (abs(p.y) - .05 < 0.0001)
        c = float4(0., 0., 0., 1.);
    else if (abs(p.y - 1.) - .05 < 0.0001)
        c = float4(0., 0., 0., 1.);
    else
        c = float4(0, 0, 0, 0);
    c.a *= .8 - min(.8, abs(p.x * .08));
    c.xyz = mix(c.xyz, make_float3(length(c.xyz)), .15);
    return c;
}

float4 nyan(float2 p)
{
    //float2 uv = p*float2(0.4,1.0);
    //float ns=3.0;
    //float nt = iTime*ns; nt-=mod(nt,240.0/256.0/6.0); nt = mod(nt,240.0/256.0);
    //float ny = mod(iTime*ns,1.0); ny-=mod(ny,0.75); ny*=-0.05;
    //float4 color = texture(iChannel1,float2(uv.x/3.0+210.0/256.0-nt+0.05,.5-uv.y-ny));
    //if (uv.x<-0.3) color.a = 0.0;
    //if (uv.x>0.2) color.a=0.0;
    //return color;
    return float4(0, 0, 0, 0);
}

// Raymarching and 2D graphics

float3 raymarch(in float3 from, in float3 dir)

{
    edge = 0.;
    float3 p, norm;
    float  d       = 100.;
    float  totdist = 0.;
    for (int i = 0; i < RAY_STEPS; i++) {
        if (d > det && totdist < 25.0) {
            p   = from + totdist * dir;
            d   = de(p);
            det = detail * exp(.13 * totdist);
            totdist += d;
        }
    }
    float3 col = make_float3(0.);
    p -= (det - d) * dir;
    norm = normal(p);
#ifdef SHOWONLYEDGES
    col = 1. - float3(edge); // show wireframe version
#else
    col = (1. - abs(norm)) * max(0., 1. - edge * .8); // set normal as color with dark edges
#endif
    totdist = clamp(totdist, 0., 26.);
    dir.y -= .02;
    //float sunsize=7.-max(0.,texture(iChannel0,float2(.6,.2)).x)*5.; // responsive sun size
    float sunsize = 7. - max(0., 0.) * 5.;                                                                       // responsive sun size
    float an      = atan2(dir.x, dir.y) + iTime * 1.5;                                                           // angle for drawing and rotating sun
    float s       = pow(clamp(1.0 - length(dir.xy) * sunsize - abs(.2 - mod(an, .4)), 0., 1.), .1);              // sun
    float sb      = pow(clamp(1.0 - length(dir.xy) * (sunsize - .2) - abs(.2 - mod(an, .4)), 0., 1.), .1);       // sun border
    float sg      = pow(clamp(1.0 - length(dir.xy) * (sunsize - 4.5) - .5 * abs(.2 - mod(an, .4)), 0., 1.), 3.); // sun rays
    float y       = mix(.45, 1.2, pow(smoothstep(0., 1., .75 - dir.y), 2.)) * (1. - sb * .5);                    // gradient sky

    // set up background with sky and sun
    float3 backg = float3(0.5, 0., 1.) * ((1. - s) * (1. - sg) * y + (1. - sb) * sg * float3(1., .8, 0.15) * 3.);
    backg += float3(1., .9, .1) * s;
    backg = max(backg, sg * float3(1., .9, .5));

    col = mix(float3(1., .9, .3), col, exp(-.004 * totdist * totdist)); // distant fading to sun color
    if (totdist > 25.)
        col = backg; // hit background
    col = pow(col, make_float3(GAMMA)) * BRIGHTNESS;
    col = mix(make_float3(length(col)), col, SATURATION);
#ifdef SHOWONLYEDGES
    col = 1. - float3(length(col));
#else
    col *= float3(1., .9, .85);
#ifdef NYAN
    //dir.yx*=rot(dir.x);
    dir.yx         = mul(rot(dir.x), dir.yx);
    float2 ncatpos = (dir.xy + float2(-3. + mod(-t, 6.), -.27));
    float4 ncat    = nyan(ncatpos * 5.);
    float4 rain    = rainbow(ncatpos * 10. + float2(.8, .5));
    if (totdist > 8.)
        col = mix(col, max(make_float3(.2), rain.xyz), rain.a * .9);
    if (totdist > 8.)
        col = mix(col, max(make_float3(.2), ncat.xyz), ncat.a * .9);
#endif
#endif
    return col;
}

// get camera position
float3 move(inout float3 dir)
{
    float3 go    = path(t);
    float3 adv   = path(t + .7);
    float  hd    = de(adv);
    float3 advec = normalize(adv - go);
    float  an    = adv.x - go.x;
    an *= min(1., abs(adv.z - go.z)) * sign(adv.z - go.z) * .7;
    //dir.xy*=float2x2(cos(an),sin(an),-sin(an),cos(an));
    dir.xy = mul(float2x2(cos(an), sin(an), -sin(an), cos(an)), dir.xy);
    an     = advec.y * 1.7;
    dir.yz = mul(float2x2(cos(an), sin(an), -sin(an), cos(an)), dir.yz);
    an     = atan2(advec.x, advec.z);
    dir.xz = mul(float2x2(cos(an), sin(an), -sin(an), cos(an)), dir.xz);
    return go;
}

void mainImage(out float4 fragColor, in float2 fragCoord)
{
    float2 uv    = fragCoord.xy / iResolution.xy * 2. - 1.;
    float2 oriuv = uv;
    uv.y *= iResolution.y / iResolution.x;
    float2 mouse = (iMouse.xy / iResolution.xy - .5) * 3.;
    if (iMouse.z < 1.)
        mouse = float2(0., -0.05);
    float  fov = .9 - max(0., .7 - iTime * .3);
    float3 dir = normalize(float3(uv * fov, 1.));
    //dir.yz*=rot(mouse.y);
    dir.yz = mul(rot(mouse.y), dir.yz);
    //dir.xz*=rot(mouse.x);
    dir.xz       = mul(rot(mouse.x), dir.xz);
    float3 from  = origin + move(dir);
    float3 color = raymarch(from, dir);
#ifdef BORDER
    color = mix(make_float3(0.), color, pow(max(0., .95 - length(oriuv * oriuv * oriuv * float2(1.05, 1.1))), .3));
#endif
    fragColor = float4(color, 1.);
}

[numthreads(8, 8, 1)] void csmain(uint3 tid
                                  : SV_DispatchThreadID) {
    float4 outColor  = (float4)0;
    float2 fragCoord = float2(tid.x, iResolution.y - tid.y) + float2(0.5, 0.5);
    mainImage(outColor, fragCoord);
    outImage[tid.xy] = outColor;
}