float4 VS(float4 position : POSITION) : SV_Position
{
    return position;
}

float4 PS() : SV_Target
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}