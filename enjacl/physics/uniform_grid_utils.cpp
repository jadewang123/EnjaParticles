#ifndef _UNIFORM_GRID_UTILS_CL_
#define _UNIFORM_GRID_UTILS_CL_


/* TO BE INCLUDED FROM OTHER FILES. In OpenCL, I believe that all device code
// must be in the same file as the kernel using it. 
*/

/*----------------------------------------------------------------------*/

// Template parameters
//#define D Step1::Data

#define D float
#define O SPHNeighborCalc<Step1::Calc, Step1::Data>

#undef USE_TEX

#include "cl_macros.h"
#include "cl_structures.h"
#include "neighbors.cpp"



/*--------------------------------------------------------------*/
int4 calcGridCell(float4 p, float4 grid_min, float4 grid_delta)
{
	// subtract grid_min (cell position) and multiply by delta
	//return make_int4((p-grid_min) * grid_delta);

	//float4 pp = (p-grid_min)*grid_delta;
	float4 pp;
	pp.x = (p.x-grid_min.x)*grid_delta.x;
	pp.y = (p.y-grid_min.y)*grid_delta.y;
	pp.z = (p.z-grid_min.z)*grid_delta.z;
	pp.w = (p.w-grid_min.w)*grid_delta.w;

	int4 ii;
	ii.x = (int) pp.x;
	ii.y = (int) pp.y;
	ii.z = (int) pp.z;
	ii.w = (int) pp.w;
	return ii;
}

/*--------------------------------------------------------------*/
uint calcGridHash(int4 gridPos, float4 grid_res, __constant bool wrapEdges)
{
	// each variable on single line or else STRINGIFY DOES NOT WORK
	int gx;
	int gy;
	int gz;

	if(wrapEdges) {
		int gsx = (int)floor(grid_res.x);
		int gsy = (int)floor(grid_res.y);
		int gsz = (int)floor(grid_res.z);

//          //power of 2 wrapping..
//          gx = gridPos.x & gsx-1;
//          gy = gridPos.y & gsy-1;
//          gz = gridPos.z & gsz-1;

		// wrap grid... but since we can not assume size is power of 2 we can't use binary AND/& :/
		gx = gridPos.x % gsx;
		gy = gridPos.y % gsy;
		gz = gridPos.z % gsz;
		if(gx < 0) gx+=gsx;
		if(gy < 0) gy+=gsy;
		if(gz < 0) gz+=gsz;
	} else {
		gx = gridPos.x;
		gy = gridPos.y;
		gz = gridPos.z;
	}


	//We choose to simply traverse the grid cells along the x, y, and z axes, in that order. The inverse of
	//this space filling curve is then simply:
	// index = x + y*width + z*width*height
	//This means that we process the grid structure in "depth slice" order, and
	//each such slice is processed in row-column order.

	return (gz*grid_res.y + gy) * grid_res.x + gx; 
}

	/*--------------------------------------------------------------*/
	/* Iterate over particles found in the nearby cells (including cell of position_i)
	*/
	void IterateParticlesInCell(
		__global float4*    var_sorted,
		__constant uint 	numParticles, 
		__constant int4 	cellPos,
		__constant uint 	index_i, 
		__constant float4 	position_i, 
		__global int* 		cell_indexes_start,
		__global int* 		cell_indexes_end, 
		__constant struct GridParams* gp,
		__constant struct FluidParams* fp
    )
	{
		// get hash (of position) of current cell
		//volatile uint cellHash = UniformGridUtils::calcGridHash<true>(cellPos, cGridParams.grid_res);
		// wrap edges (false)
		uint cellHash = calcGridHash(cellPos, gp->grid_res, false);

		/* get start/end positions for this cell/bucket */
		uint startIndex = FETCH(cell_indexes_start,cellHash);

		/* check cell is not empty
		 * WHERE IS 0xffffffff SET?  NO IDEA ************************
		 */
		if (startIndex != 0xffffffff) {	   
			uint endIndex = FETCH(cell_indexes_end, cellHash);

			/* iterate over particles in this cell */
			for(uint index_j=startIndex; index_j < endIndex; index_j++) {			
				// For now, nothing to loop over. ADD WHEN CODE WORKS. 
				// Is there a neighbor?
#if 1
				ForPossibleNeighbor(var_sorted, numParticles, index_i, index_j, position_i, gp, fp);
#endif
			}
		}
	}

	/*--------------------------------------------------------------*/
	/* Iterate over particles found in the nearby cells (including cell of position_i) 
	 *template<class O, class D>
	 */
	void IterateParticlesInNearbyCells(
		__global float4* vars_sorted,
		uint			numParticles,
		__constant uint 	index_i, 
		__constant float4   position_i, 
		__global int* 		cell_indices_start,
		__global int* 		cell_indices_end,
		__constant struct GridParams* gp,
		__constant struct FluidParams* fp)
	{
		// How to chose which PreCalc to use? 
		// TODO LATER
		//PreCalc(data, index_i); // TODO

		// get cell in grid for the given position
		int4 cell = calcGridCell(position_i, gp->grid_min, gp->grid_delta);


		// iterate through the 3^3 cells in and around the given position
		// can't unroll these loops, they are not innermost 
		for(int z=cell.z-1; z<=cell.z+1; ++z) {
			for(int y=cell.y-1; y<=cell.y+1; ++y) {
				for(int x=cell.x-1; x<=cell.x+1; ++x) {
					int4 ipos;
					ipos.x = x;
					ipos.y = y;
					ipos.z = z;
					ipos.w = 1;
	#if 1
					IterateParticlesInCell(vars_sorted, numParticles, ipos, index_i, position_i, cell_indices_start, cell_indices_end, gp, fp);
	#endif
				}
			}
		}

		// TO REMOVE
		//O::PostCalc(data, index_i);

		// TO DO LATER
		//PostCalc(data, index_i);

	}

	//----------------------------------------------------------------------
//--------------------------------------------------------------
__kernel void K_SumStep1(
				uint    numParticles,
				uint	nb_vars,
				__global float4* vars,   // *** ERROR
				__global float4* sorted_vars,
        		__global int*    cell_indexes_start,
        		__global int*    cell_indexes_end,
				__constant struct GridParams* gp,
				__constant struct GridParams* fp
				)
{
    // particle index
	uint index = get_global_id(0);
    if (index >= numParticles) return;

    //Step1::Data data;
    //data.dParticleDataSorted = dParticleDataSorted;

	//This is done as part of the template approach since the Data can then be used as a template object 
	//in Cuda
	vars = sorted_vars; // not needed

	/* assume position is 0th variable */
    float4 position_i = FETCH_VAR(vars, index, POS);

    // Do calculations on particles in neighboring cells


	#if 1
    IterateParticlesInNearbyCells(sorted_vars, numParticles, index, position_i, cell_indexes_start, cell_indexes_end, gp, fp);
	#endif
}

/*-------------------------------------------------------------- */
#endif
