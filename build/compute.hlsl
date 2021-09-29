struct ComputeData
{
    float energy;
};

RWStructuredBuffer<ComputeData> data : register(u0);

void TransferEnergy(uint id1, uint id2)
{
    float dTemperature = data[id2].energy - data[id1].energy;
    float dTime = 0.1f;//0.1f;
    float Q = dTemperature * dTime;
    data[id1].energy += Q;
    data[id2].energy -= Q;
} 

[numthreads(1, 1, 1)]
void CSMain(uint3 groupID : SV_GroupID)
{
    if(groupID.x != 0 || groupID.y != 0)
    {
        int x = groupID.x;
        if(groupID.x > 0)
        {
            x -= 1;
        }
        int y = groupID.y;
        if(groupID.y > 0)
        {
            y -= 1;
        }
        int multiplier = groupID.y > 0 ? 1 : 500;
        for(int i = 0; i < 500-1; i++)
        {
            uint id = 500 * y + (i * multiplier) + x;
            TransferEnergy(id, id + multiplier);  
        }
        //data[1].energy = multiplier;
    }
    //data[2].energy = multiplier;
    // data[groupID].energy += 20.0f;
}