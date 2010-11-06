#include "SPH.h"

#include <string>
using namespace std;

namespace rtps {

//----------------------------------------------------------------------

//----------------------------------------------------------------------
void SPH::loadNeighbors()
{
	printf("enter neighbor\n");

    try {
        string path(SPH_CL_SOURCE_DIR);
        path = path + "/neighbors_cl.cl";
        k_neighbors = Kernel(ps->cli, path, "neighbors");
        printf("bigger problem\n");
    } catch(cl::Error er) {
        printf("ERROR(neighborSearch): %s(%s)\n", er.what(), oclErrorString(er.err()));
        exit(1);
    }

	Kernel kern = k_neighbors;
	    	
    printf("setting kernel args\n");
	int iarg = 0;
	kern.setArg(iarg++, cl_vars_sorted.getDevicePtr());
	kern.setArg(iarg++, cl_cell_indices_start.getDevicePtr());
	kern.setArg(iarg++, cl_cell_indices_end.getDevicePtr());
	kern.setArg(iarg++, cl_GridParamsScaled.getDevicePtr());
	//kern.setArg(iarg++, cl_FluidParams->getDevicePtr());
	kern.setArg(iarg++, cl_SPHParams.getDevicePtr());

	// ONLY IF DEBUGGING
	kern.setArg(iarg++, clf_debug.getDevicePtr());
	kern.setArg(iarg++, cli_debug.getDevicePtr());
	//kern.setArg(iarg++, cl_index_neigh->getDevicePtr());



	}
//----------------------------------------------------------------------

void SPH::neighborSearch(int choice)
{

	// which == 0 : density update
	// which == 1 : force update

    /*
	if (which == 0) ts_cl[TI_DENS]->start();
	if (which == 1) ts_cl[TI_PRES]->start();
	if (which == 2) ts_cl[TI_COL]->start();
	if (which == 3) ts_cl[TI_COL_NORM]->start();
    */

    //Copy choice to SPHParams
	params.choice = choice;
    std::vector<SPHParams> vparams(0);
    vparams.push_back(params);
    cl_SPHParams.copyToDevice(vparams);


	size_t global = (size_t) num;
	int local = 128;

 	k_neighbors.execute(global, local);
	ps->cli->queue.finish();
   

    //DEBUGING
	printf("============================================\n");
	printf("which == %d *** \n", choice);

	printf("***** PRINT neighbors diagnostics ******\n");
    std::vector<int4> cli = cli_debug.copyToHost(num);
    std::vector<float4> clf = clf_debug.copyToHost(num);

	//for (int i=0; i < num; i++) {  
	for (int i=0; i < 2; i++) 
    {  
		printf("clf_debug: %f, %f, %f, %f\n", clf[i].x, clf[i].y, clf[i].z, clf[i].w);
		printf("cli_debug: %d, %d, %d, %d\n", cli[i].x, cli[i].y, cli[i].z, cli[i].w);
		printf("-----\n");
    }


}


} // namespace
