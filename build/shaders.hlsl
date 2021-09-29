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

struct ComputeData
{
    float energy;
};

StructuredBuffer<ComputeData> data : register(t0);

float4 PS(VOut input) : SV_Target
{
    float2 pos = input.position;
    float energy = data[pos.x].energy;
    return float4(energy / 100.0f, 0.0f, 0.0f, 0.0f);
}