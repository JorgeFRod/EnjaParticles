#include "SPH.h"

namespace rtps
{

void SPH::loadBitonicSort()
{

    printf("about to instantiate sorting\n");
    
    bitonic = Bitonic<int>( ps->cli,    
                            &cl_sort_output_hashes,
                            &cl_sort_output_indices,
                            &cl_sort_hashes,
                            &cl_sort_indices);
    
}

void SPH::bitonic_sort(bool ghosts)
{
    try
    {
		int dir = 1; 		// dir: direction
		//int batch = num;
        if(ghosts)
        {
            int arrayLength = nb_ghosts;
            int batch = nb_ghosts / arrayLength;

            ghost_bitonic.Sort(batch, arrayLength, dir);

        }
        else
        {

		    int arrayLength = max_num;
            int batch = max_num / arrayLength;

            //printf("about to try sorting\n");
            bitonic.Sort(batch, arrayLength, dir);
        }
    
	} catch (cl::Error er) {
        printf("ERROR(bitonic sort): %s(%s)\n", er.what(), oclErrorString(er.err()));
		exit(0);
	}

    ps->cli->queue.finish();

    if(ghosts)
    {
        printf("ghosts scopy\n");
        scopy(num, cl_ghosts_sort_output_hashes.getDevicePtr(), 
                     cl_ghosts_sort_hashes.getDevicePtr());
        scopy(num, cl_ghosts_sort_output_indices.getDevicePtr(), 
                     cl_ghosts_sort_indices.getDevicePtr());

        printf("ghosts done scopy\n");
 
    }
    else
    {
        scopy(num, cl_sort_output_hashes.getDevicePtr(), 
                     cl_sort_hashes.getDevicePtr());
        scopy(num, cl_sort_output_indices.getDevicePtr(), 
                     cl_sort_indices.getDevicePtr());
    }
    
    /*
    int nbc = 10;
    std::vector<int> sh = cl_sort_hashes.copyToHost(nbc);
    std::vector<int> eci = cl_cell_indices_end.copyToHost(nbc);

    for(int i = 0; i < nbc; i++)
    {
        printf("before[%d] %d eci: %d\n; ", i, sh[i], eci[i]);
    }
    printf("\n");
    */


	/*
    ps->cli->queue.finish();

    sh = cl_sort_hashes.copyToHost(nbc);
    eci = cl_cell_indices_end.copyToHost(nbc);

    for(int i = 0; i < nbc; i++)
    {
        printf("after[%d] %d eci: %d\n; ", i, sh[i], eci[i]);
    }
    printf("\n");
    */



}

}
