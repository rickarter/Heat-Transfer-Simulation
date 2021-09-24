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

float4 PS(VOut input) : SV_Target
{
    float4 pos = input.position;
    return pos;
}