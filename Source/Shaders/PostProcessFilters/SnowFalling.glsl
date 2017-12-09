uniform sampler2D u_colorTexture;
uniform sampler2D u_texture;
uniform sampler2D u_depthTexture;

uniform float u_fallSpeed;
uniform float u_snowThick;
uniform float u_windDirection;

varying vec2 v_textureCoordinates;

float fallSpeed = u_fallSpeed;
float pi = 3.14159265359;
float gTime;
float depth;
vec3 pos;
vec2 uv;
vec2 p;
float aspectRatio;

struct Camera
{
    vec3 eye, target;			// eye - camera position, target - the point at which the camera is pointed at
    vec3 forward, right, up;	// the camera CoordinateSystem
    float zoom;					// the amount of "scaling" in the camera {right,up} plane
};

Camera camera;

struct Ray
{
    vec3 origin;				// the origin of the ray
    vec3 direction;				// the ray direction vector. Normally an unit vector.
};

Ray ray;

float unitSin(float t)
{
    return 0.5 + 0.5 * sin(t);
}

float flakeVolume()
{
    float sum = 0.0;

    float snowThicknessThreshold = 1.0 - u_snowThick;
    vec3 snowDirection = normalize(czm_viewerPositionWC);
    vec3 snowDirectionEC = normalize((czm_view * vec4(snowDirection,0.0)).xyz);

    vec3 cameraDirectionEC = vec3(0.0,0.0,-1.0);
    float cosineTheta = (dot(cameraDirectionEC,snowDirectionEC) / (length(cameraDirectionEC) * length(snowDirectionEC)));
    vec3 perDirPoint = cameraDirectionEC - cosineTheta * snowDirectionEC;
    vec3 perDirectionWC = normalize((czm_inverseView * vec4(perDirPoint.xyz,0.0)).xyz);

    vec4 rayOriginWorldHomo = (czm_inverseView * vec4(ray.origin, 1.0));
    vec3 rayOriginWorld = rayOriginWorldHomo.xyz / rayOriginWorldHomo.w;
    vec3 rayDirectionWorld = (czm_inverseView * vec4(ray.direction, 0.0)).xyz;
    rayDirectionWorld = normalize(rayDirectionWorld);

    vec3 windForce = normalize(sin(u_windDirection) * (czm_view * vec4(0.6,0.5,0.1,0.0)).xyz);
    snowDirectionEC += windForce;
    vec3 snowCamOri = ray.origin + 0.01 * snowDirectionEC;

    float fall;
    vec3 p1, p2;
    float teta;
    float t1, t2;
    //float a = pow(snowDirectionEC.x, 2.0) + pow(snowDirectionEC.y, 2.0);
    //float b = 2.0 * (snowDirectionEC.x * snowCamOri.x + snowDirectionEC.y * snowCamOri.y);
    //float c = pow(snowCamOri.x, 2.0) + pow(snowCamOri.y, 2.0);
    float a = pow(rayDirectionWorld.x, 2.0) + pow(rayDirectionWorld.z, 2.0);
    float b = 2.0 * (rayDirectionWorld.x * rayOriginWorld.x/6000000.0 + rayDirectionWorld.z * rayOriginWorld.z/6000000.0);
    float c = pow(rayOriginWorld.x/6000000.0, 2.0) + pow(rayOriginWorld.z/6000000.0, 2.0);
    float ac4 = 4.0 * a*c;
    float a4 = 4.0 * a;
    float a2 = 2.0 * a;
    float bb = b*b;
    float bbSubAC4 = bb - ac4;
    for (float r = 1.0; r <= 16.0; r+=0.5)
    {
        float R = r + sin(pi * r * gTime * 0.05) / (r * 0.25);
        float delta = bbSubAC4 + a4 * R*R;
        if (delta >= 0.0)
        {
            t1 = (-b - sqrt(delta))/a2;
            t2 = (-b + sqrt(delta))/a2;
            p1 = ray.origin + t1 * ray.direction;
            p2 = ray.origin + t2 * ray.direction;
            /*
            if (t1 < depth && t1 > 10.0)
            {
                teta = atan(p1.z, p1.x) / (2.0 * pi);
                fall = (0.235 + 0.235 * unitSin(r)) * fallSpeed * gTime  +  cos(r);
                float s = 10.0;

                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.25 * teta * r, 0.3 * p1.y + fall)).r);
                s *= smoothstep(snowThicknessThreshold + 0.05, 1.0, texture2D(u_texture, vec2(0.21 * p1.y + fall, -0.45 * teta * r)).r);
                s *= smoothstep(snowThicknessThreshold + 0.1, 1.0, texture2D(u_texture, vec2(-(0.21 * p1.y + fall), 0.45 * teta * r)).r);

                sum += s;
            }
            if (t2 < depth && t2 > 0.0)
            {
                teta = atan(p2.z, p2.x) / (2.0 * pi);
                fall = (0.125 + 0.125 * unitSin(r)) * fallSpeed * gTime  +  cos(r);
                float s = 10.0;

                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.125 * teta * r, 0.116 * p2.y + fall)).r);
                s *= smoothstep(snowThicknessThreshold + 0.05, 1.0, texture2D(u_texture, vec2(-(0.11 * p2.y + fall), 0.09 * teta * r)).r);
                s *= smoothstep(snowThicknessThreshold + 0.1, 1.0, texture2D(u_texture, vec2(0.11 * p2.y + fall, -0.09 * teta * r)).r);

                sum += s;
            }
            */
            if (t1 < depth && t1 > 2.0)
            {
                teta = atan(p1.z, p1.x) / (2.0 * pi);
                fall = (0.235 + 0.35 * unitSin(r)) * fallSpeed * gTime  +  cos(r);
                float s = 6.0;

                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.4 * teta * r, 0.1 * p1.y + fall)).r);
                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.11 * p1.y + fall, -0.4 * teta * r)).r);
                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(-(0.11 * p1.y + fall), 0.4 * teta * r)).r);

                sum += s;
            }
            if (t2 < depth && t2 > 0.0)
            {
                teta = atan(p2.z, p2.x) / (2.0 * pi);
                fall = (0.235 + 0.35 * unitSin(r)) * fallSpeed * gTime  +  cos(r);
                float s = 6.0;

                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.4 * teta * r, 0.1 * p2.y + fall)).r);
                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(-(0.11 * p2.y + fall), 0.4 * teta * r)).r);
                s *= smoothstep(snowThicknessThreshold, 1.0, texture2D(u_texture, vec2(0.11 * p2.y + fall, -0.4 * teta * r)).r);

                sum += s;
            }
        }
    }
    return sum / 2.0;
}

vec4 screenSpaceBlizzard()
{
    float flake = flakeVolume();
    return vec4(1.0, 1.0, 1.0, clamp(flake, 0.0, 1.0));
}
// computes the ray that starts from the camera eye and is directed towards the given fragment position
// the fragment position 'p' is in [-1,1]x[-1,1];
Ray shootRay()
{
    Ray r;
    r.origin = vec3(0.0,0.0,0.0);
    r.direction = normalize(vec3(p.x,p.y,-1.0) - r.origin);
    return r;
}
vec4 screenSpaceFog(vec3 p)
{
    return vec4(0.9, 0.95, 1.0, 0.4 * clamp(1.0 - exp(-pow(depth * 0.03, 2.0)), 0.0, 1.0));
}
vec4 toEye(vec2 uv, float depth)
{
   vec2 xy = vec2((uv.x * 2.0 - 1.0), ((1.0 - uv.y) * 2.0 - 1.0));
   vec4 posInCamera = czm_inverseProjection * vec4(xy, depth, 1.0);
   posInCamera = posInCamera / posInCamera.w;
   return posInCamera;
}
vec3 renderEverything(vec2 offset)
{
    //ray is a ray from camera in camera space
    ray = shootRay();
    vec3 col = vec3(0.0);
    float tmax = czm_entireFrustum.y - czm_entireFrustum.x;
    float h = 1.0;
    float t = 10.0;

    float zDepth = texture2D(u_depthTexture, v_textureCoordinates).r;
    vec4 posInCamera = toEye(v_textureCoordinates,depth);
    //depth = tmax;
    depth = length(posInCamera.xyz - ray.origin);
    if (zDepth < 1.0)
    {
        //pos and rayorigin and raydirection are all in camera space
        //pos = ray.origin + t * ray.direction;
        //depth = length(posInCamera.xyz - ray.origin);
        col = texture2D(u_colorTexture, v_textureCoordinates).rgb;
    }
    else
    {
        //depth = tmax;
        col = vec3(texture2D(u_colorTexture, v_textureCoordinates).r);
    }
    vec4 flake = screenSpaceBlizzard();
    col = flake.a * flake.rgb + (1.0 - flake.a) * col.rgb;
    vec4 fog = screenSpaceFog(pos);
    col = fog.a * fog.rgb + (1.0 - fog.a) * col.rgb;

    return col;
}
void main(void)
{
    float cameraToCenter = length(czm_viewerPositionWC);
    //if(cameraToCenter <= 6381000.0)
    //{
        //earthEllipsoid = czm_getWgs84EllipsoidEC();
        gTime = mod(czm_frameNumber/60.0,12.0);
        uv = v_textureCoordinates;
        aspectRatio = czm_viewport.z / czm_viewport.w;
        p = -1.0 + 2.0 * uv;
        p.x = p.x * aspectRatio;
        vec3 col = vec3(0.0);

        vec3 colT0 = renderEverything(vec2(0.0));
        gTime += 0.01;
        vec3 colT1 = renderEverything(vec2(0.05));
        gTime += 0.01;
        vec3 colT2 = renderEverything(vec2(0.1));
        col = 0.25 * colT0 + 0.5 * colT1 + 0.25 * colT2;

        //gamma correct
        col = pow(col,vec3(0.4545));

        gl_FragColor = vec4(col, 1.0);
        //gl_FragColor = mix(texture2D(u_colorTexture, v_textureCoordinates), vec4(col,1.0), 0.5);
    //}
    //else
    //{
    //    gl_FragColor = texture2D(u_colorTexture, v_textureCoordinates);
    //}
}
