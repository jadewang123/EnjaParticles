#include "../SPH.h"

namespace rtps {

void SPH::cpuLeapFrog()
{
    float h = ps->settings.dt;
    for(int i = 0; i < num; i++)
    {
        float4 p = positions[i];
        float4 v = velocities[i];
        float4 f = forces[i];

        //external force is gravity
        f.z += -9.8f;

        float speed = magnitude(f);
        if(speed > 600.0f) //velocity limit, need to pass in as struct
        {
            f.x *= 600.0f/speed;
            f.y *= 600.0f/speed;
            f.z *= 600.0f/speed;
        }

        float4 vnext = v;
        vnext.x += h*f.x;
        vnext.y += h*f.y;
        vnext.z += h*f.z;

        float xsphfactor = .1f;
        vnext.x += xsphfactor * xsphs[i].x;
        vnext.y += xsphfactor * xsphs[i].y;
        vnext.z += xsphfactor * xsphs[i].z;
       
        float scale = params.simulation_scale;
        p.x += h*vnext.x / scale;
        p.y += h*vnext.y / scale;
        p.z += h*vnext.z / scale;
        p.w = 1.0f; //just in case

        veleval[i].x = (v.x + vnext.x) *.5f;
        veleval[i].y = (v.y + vnext.y) *.5f;
        veleval[i].z = (v.z + vnext.z) *.5f;

        velocities[i] = vnext;
        positions[i] = p;
         
    }
    //printf("v.z %f p.z %f \n", velocities[0].z, positions[0].z);
}

}