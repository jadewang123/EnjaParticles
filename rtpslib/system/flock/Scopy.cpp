#include "FLOCK.h"

#include <string>

namespace rtps 
{

    void FLOCK::loadScopy()
    {
        printf("create scopy kernel\n");
        std::string path(FLOCK_CL_SOURCE_DIR);
        path = path + "/scopy.cl";
        k_scopy = Kernel(ps->cli, path, "scopy");

    }

    void FLOCK::scopy(int n, cl_mem xsrc, cl_mem ydst)
    {
        int args = 0;
        k_scopy.setArg(args++, n);
        k_scopy.setArg(args++, xsrc);
        k_scopy.setArg(args++, ydst);

        size_t global = (size_t) n;
        size_t local = 128; //cl.getMaxWorkSize(kern.getKernel());

        //not sure why i can't use local
        k_scopy.execute(global, local);
        ps->cli->queue.finish();

    }

}
