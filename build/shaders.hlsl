struct VIn
{
    float4 position : POSITION;
};

struct VOut
{
    float4 position : SV_POSITION;
};

VOut VS(VIn input)
{
    VOut output;

    output.position = input.position;

    return output;
}

float4 lerp4(float4 a, float4 b, float t)
{
    return float4(
        lerp(a.x, b.x, t),
        lerp(a.y, b.y, t),
        lerp(a.z, b.z, t),
        lerp(a.w, b.w, t)
    );
}

struct ComputeData
{
    float energy;
};

StructuredBuffer<ComputeData> data : register(t0);

float4 PS(VOut input) : SV_Target
{
    float2 pos = input.position;
    float energy = data[pos.x-250 + 500 * pos.y].energy;
    float4 color = energy >= 0 ? lerp4(float4(1.0f, 1.0f, 0.0f, 1.0f), float4(1.0f, 0.0f, 0.0f, 1.0f), energy/500.0f) : 
    lerp4(float4(1.0f, 1.0f, 0.0f, 1.0f), float4(0.513f, 0.819f, 0.945f, 1.0f), abs(energy)/100.0f);
    return color;

    /*     float4 color = energy >= 0 ? lerp4(float4(.13f, .13f, .13f, 1.0f), float4(1.0f, 0.0f, 0.0f, 1.0f), energy/50.0f) : 
    lerp4(float4(.13f, .13f, .13f, 1.0f), float4(0.513f, 0.719f, 0.845f, 1.0f), abs(energy)/10.0f); */
    //Note(Eric): Use .13f for 0 temperature
}