//update the particle position and color
__kernel void enja(__global float4* vertices, __global float4* colors, __global int* indices, __global float4* vert_gen, __global float4* velo_gen, __global float4* velocities, float h)

{
    unsigned int i = get_global_id(0);
    float life = velocities[i].w;
    h = h*5;
    life -= h;    //should probably depend on time somehow
    if(life <= 0.)
    {
        //reset this particle
        vertices[i].x = vert_gen[i].x;
        vertices[i].y = vert_gen[i].y;
        vertices[i].z = vert_gen[i].z;

        velocities[i].x = 2*velo_gen[i].x;
        velocities[i].y = 2*velo_gen[i].y;
        velocities[i].z = velo_gen[i].z;
        life = 1.0f;
    } 
    float xn = vertices[i].x;
    float yn = vertices[i].y;
    float zn = vertices[i].z;


    float vxn = velocities[i].x;
    float vyn = velocities[i].y;
    float vzn = velocities[i].z;
    velocities[i].x = vxn;
    velocities[i].y = vyn;
    velocities[i].z = vzn - h*9.8f;

    vertices[i].x = xn + h*velocities[i].x; //xn + h*(sigma * (yn - xn));
    vertices[i].y = yn + h*velocities[i].y; //yn + h*(xn*(rho - zn));
    vertices[i].z = zn + h*velocities[i].z; // + h*(xn*yn - beta * zn);

     
    //colors[i].x = 1.0f;
    colors[i].x = .5f-life*.5f;
    colors[i].y = .5f-life*.5f;
    //colors[i].z = .5f-life*.5f;
    colors[i].z = 1.0f;
    colors[i].w = life;

    //save the life!
    velocities[i].w = life;
}


