#version 420

uniform sampler3D u_sat;

uniform vec3 u_lightDir;
uniform int u_sliceIndex;

uniform int u_nbShells;
uniform int u_shellRadius;

uniform float u_scatteringConeAngle;
uniform int   u_nbSamplesAlongCone;

out vec4 illuminationVal;

//-----------------------------------------------------------------------------

vec4 satLookup(in ivec3 min, in ivec3 max)
{
    return texelFetch(u_sat, max, 0)
            - texelFetch(u_sat, ivec3( max.x, max.y, min.z ), 0)
            - texelFetch(u_sat, ivec3( max.x, min.y, max.z ), 0)
            - texelFetch(u_sat, ivec3( min.x, max.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, min.y, max.z ), 0)
            + texelFetch(u_sat, ivec3( min.x, max.y, min.z ), 0)
            + texelFetch(u_sat, ivec3( max.x, min.y, min.z ), 0)
            - texelFetch(u_sat, min, 0);
}

//-----------------------------------------------------------------------------

int getVoxelSecondaryCoord(in ivec2 lineOrigin, in vec2 lineVector, int x)
{
    float lineSlope = lineVector.y / lineVector.x;
    return int(lineSlope * float(x - lineOrigin.x) + lineOrigin.y);
}

//-----------------------------------------------------------------------------

float coneShadowQuery(in ivec3 voxelPos)
{
    // Cone faces the light.
    const vec3 coneDir = normalize(-u_lightDir);
    float coneAngle = u_scatteringConeAngle;

    int nbConeSamples = u_nbSamplesAlongCone;

    // Find the axis (x, y or z) which has the smallest angle to the light direction.
    int primaryAxis = abs(coneDir.y) > abs(coneDir.x)            ? 1 : 0;
    primaryAxis     = abs(coneDir.z) > abs(coneDir[primaryAxis]) ? 2 : primaryAxis;

    // Two remaining axes.
    int secondaryAxis0, secondaryAxis1;

    if(primaryAxis == 0)
    {
        secondaryAxis0 = 2;
        secondaryAxis1 = 1;
    }
    else if(primaryAxis == 1)
    {
        secondaryAxis0 = 0;
        secondaryAxis1 = 2;
    }
    else if(primaryAxis == 2)
    {
        secondaryAxis0 = 1;
        secondaryAxis1 = 0;
    }

    ivec3 satSize = textureSize(u_sat, 0);

    // Coordinate on the primary axis where the cone exits the volume.
    const int outCoord = coneDir[primaryAxis] < 0 ? 0 : satSize[primaryAxis] - 1;

    // Number of voxels separating our current voxel to the exit.
    int coneVoxelHeight = abs(voxelPos[primaryAxis] - outCoord);

    // Distance (in voxels) between cuboid samples along the cone.
    int cuboidHeight = coneVoxelHeight / nbConeSamples;

    if(cuboidHeight <= 1)
    {
        nbConeSamples = coneVoxelHeight / 2;
        cuboidHeight  = 2;
    }

    if(nbConeSamples <= 1)
    {
        return 1;
    }

    // Project cone origin and direction on the (primaryAxis, secondaryAxis0) plane.
    const ivec2 projOrig0  = ivec2(voxelPos[primaryAxis], voxelPos[secondaryAxis0]);
    const vec2  projDir0   = vec2(coneDir[primaryAxis], coneDir[secondaryAxis0]);

    // Do the same for the (primaryAxis, secondaryAxis1) plane.
    const ivec2 projOrig1  = ivec2(voxelPos[primaryAxis], voxelPos[secondaryAxis1]);
    const vec2  projDir1   = vec2(coneDir[primaryAxis], coneDir[secondaryAxis1]);

    vec2 v0, v1, v2, v3;

    // x is the radius of the cone at height 1.
    float x = tan(coneAngle);

    // We compute the two lines making up the cone projection on the first plane.
    // To do so we need to determine their vectors v0 and v1.
    float coneSlope = projDir0.y / projDir0.x;
    float a = 1 + coneSlope * coneSlope;
    v0.y = sqrt(a) / a;
    v0.x = -v0.y * coneSlope;

    v1  = -v0;
    v0 += projDir0;
    v1 += projDir0;

    // We do the same for the second plane.
    coneSlope = projDir1.y / projDir1.x;
    a = 1 + coneSlope * coneSlope;

    v2.y = sqrt(a) / a;
    v2.x = -v2.y * coneSlope;

    v3  = -v2;
    v2 += projDir1;
    v3 += projDir1;

    // The way the cone is facing.
    const int coneOrientation = int(sign(coneDir[primaryAxis]));

    const int incr = (cuboidHeight + 1) * coneOrientation;

    const int beginCoord = outCoord   - coneOrientation * cuboidHeight/2;
    const int endCoord   = beginCoord - (nbConeSamples) * incr;

    float coneSum = 0.f;
    int nbVoxels  = 0;

    ivec3 queryCubeMin, queryCubeMax;

    queryCubeMin[primaryAxis] = min(outCoord, outCoord - incr);
    queryCubeMax[primaryAxis] = max(outCoord, outCoord - incr);

    for(int i = beginCoord; i != endCoord; i -= incr)
    {
        int y00, y01, y10, y11;

        y00 = getVoxelSecondaryCoord(projOrig0, v0, i);
        y01 = getVoxelSecondaryCoord(projOrig0, v1, i);
        y10 = getVoxelSecondaryCoord(projOrig1, v2, i);
        y11 = getVoxelSecondaryCoord(projOrig1, v3, i);

        queryCubeMin[secondaryAxis0] = min(y00, y01);
        queryCubeMax[secondaryAxis0] = max(y00, y01);

        queryCubeMin[secondaryAxis1] = min(y10, y11);
        queryCubeMax[secondaryAxis1] = max(y10, y11);

        queryCubeMin[primaryAxis] -= incr;
        queryCubeMax[primaryAxis] -= incr;

        queryCubeMin = max(queryCubeMin, ivec3(0));
        queryCubeMax = min(queryCubeMax, satSize - ivec3(1));

        ivec3 cubeDiff = queryCubeMax - queryCubeMin;
        nbVoxels += cubeDiff.x * cubeDiff.y * cubeDiff.z;

        coneSum += satLookup(queryCubeMin, queryCubeMax).a;
    }

    return coneSum / float(nbVoxels);
}

//-----------------------------------------------------------------------------

vec4 ambientOcclusionAndColourBleedingQuery(in ivec3 voxelPos)
{
    ivec3 satSize = textureSize(u_sat, 0) - ivec3(1);

    ivec3 shellMin = max(voxelPos - ivec3(u_shellRadius), ivec3(0));
    ivec3 shellMax = min(voxelPos + ivec3(u_shellRadius), satSize);

    int radius = u_shellRadius;

    vec4 satLookupVal = satLookup(shellMin, shellMax);
    vec4 aoFactor = satLookupVal / float(radius * radius);

    for(int i = 1; i < u_nbShells; ++i)
    {
        shellMin = max(shellMin - ivec3(u_shellRadius), ivec3(0));
        shellMax = min(shellMax + ivec3(u_shellRadius), satSize);

        vec4 lastLookup = satLookupVal;
        satLookupVal = satLookup(shellMin, shellMax);

        radius += u_shellRadius;

        aoFactor += (satLookupVal - lastLookup) / float(radius * radius);
    }

    return pow(u_nbShells * u_shellRadius, -2.f) * (aoFactor);
}

//-----------------------------------------------------------------------------

void main(void)
{
    ivec3 voxelCoords = ivec3(gl_FragCoord.xy, u_sliceIndex);

    illuminationVal = ambientOcclusionAndColourBleedingQuery(voxelCoords);

    float shadowFactor = coneShadowQuery(voxelCoords);

    illuminationVal.a += shadowFactor;

    illuminationVal = exp(-illuminationVal);
}
