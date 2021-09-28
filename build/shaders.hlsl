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
    /*float4 pos = input.position;
    pos.xy /= 800;*/
    //computeData[0].color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float energy = data[0].energy;
    return float4(energy, energy, energy, energy);
}