#include <stdio.h>

#include "importgl.h"

#include "System.h"
#include "Simple.h"

namespace rtps {


Simple::Simple(RTPS *psfr, int n)
{
    num = n;
    //store the particle system framework
    ps = psfr;

    /*
    std::vector<float4> positions(num);
    std::vector<float4> colors(num);
    std::vector<float4> forces(num);
    std::vector<float4> velocities(num);
    */
    printf("num: %d\n", num);
    positions.resize(num);
    colors.resize(num);
    forces.resize(num);
    velocities.resize(num);

    
    int j = 0;
    for(int i = 0; i < num; i++)
    {
        positions[i] = float4(i % 16, j, 0.0f, 0.0f);
        colors[i] = float4(1.0f, 0.0f, 0.0f, 0.0f);
        if(i % 16 == 0)
        {
            j++;
        }
    }
    //std::fill(positions.begin(), positions.end(), float4(0.0f, 0.0f, 0.0f, 1.0f));
    //std::fill(colors.begin(), colors.end(),float4(1.0f, 0.0f, 0.0f, 0.0f));
    std::fill(forces.begin(), forces.end(),float4(0.0f, 0.0f, 0.0f, 0.0f));
    std::fill(velocities.begin(), velocities.end(),float4(0.0f, 0.0f, 0.0f, 0.0f));
    
    managed = true;
    pos_vbo = createVBO(&positions[0], positions.size()*sizeof(float4), GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    printf("pos vbo: %d\n", pos_vbo);
    col_vbo = createVBO(&colors[0], colors.size()*sizeof(float4), GL_ARRAY_BUFFER, GL_DYNAMIC_DRAW);
    printf("col vbo: %d\n", col_vbo);

}

Simple::~Simple()
{
    if(pos_vbo && managed)
    {
        glBindBuffer(1, pos_vbo);
        glDeleteBuffers(1, (GLuint*)&pos_vbo);
        pos_vbo = 0;
    }
    if(col_vbo && managed)
    {
        glBindBuffer(1, col_vbo);
        glDeleteBuffers(1, (GLuint*)&col_vbo);
        col_vbo = 0;
    }
}

void Simple::update()
{

    //printf("calling cpuEuler\n");
    cpuEuler();

    //printf("pushing positions to gpu\n");
    glBindBuffer(GL_ARRAY_BUFFER, pos_vbo);
    glBufferData(GL_ARRAY_BUFFER, num * sizeof(float4), &positions[0], GL_DYNAMIC_DRAW);
    //printf("done pushing to gpu\n");
}


}