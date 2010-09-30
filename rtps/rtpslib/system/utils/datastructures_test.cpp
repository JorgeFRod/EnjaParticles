// This software contains source code provided by NVIDIA Corporation.
// Specifically code from the CUDA 2.3 SDK "Particles" sample

#ifndef _DATASTRUCTURES_
#define _DATASTRUCTURES_

//----------------------------------------------------------------------
__kernel void datastructures(
					int	    numParticles,
					int      nb_vars,
					__global float4*   dParticles,
					__global float4*   dParticlesSorted, 
		   			__global uint* sort_hashes,
		   			__global uint* sort_indices,
		   			__global uint* cell_indices_start,
		   			__global uint* cell_indices_end,
					__local  uint* sharedHash
			  )
{
	uint index = get_global_id(0);

	// particle index	
	//uint index = __umul24(blockIdx.x, blockDim.x) + threadIdx.x;		
	if (index >= numParticles) return;


	// blockSize + 1 elements	
	//extern __shared__ uint sharedHash[];	

	uint hash = sort_hashes[index];

	// Load hash data into shared memory so that we can look 
	// at neighboring particle's hash value without loading
	// two hash values per thread	

	uint tid = get_local_id(0);

#if 1
	sharedHash[tid+1] = hash;  // SOMETHING WRONG WITH hash on Fermi

	if (index > 0 && tid == 0) {
		// first thread in block must load neighbor particle hash
		sharedHash[0] = sort_hashes[index-1];
	}

#ifndef __DEVICE_EMULATION__
	barrier(CLK_LOCAL_MEM_FENCE);
	//__syncthreads ();
#endif

	// If this particle has a different cell index to the previous
	// particle then it must be the first particle in the cell,
	// so store the index of this particle in the cell.
	// As it isn't the first particle, it must also be the cell end of
	// the previous particle's cell

	if ((index == 0 || hash != sharedHash[tid]) )
	{
		cell_indices_start[hash] = index; // ERROR
		if (index > 0) {
			cell_indices_end[sharedHash[tid]] = index;
		}
	}
	//return;

	if (index == numParticles - 1) {
		cell_indices_end[hash] = index + 1;
	}

	uint sortedIndex = sort_indices[index];

	// Copy data from old unsorted buffer to sorted buffer

	for (int j=0; j < nb_vars; j++) {
		dParticlesSorted[index+j*numParticles]	= dParticles[sortedIndex+j*numParticles];
		//dParticlesSorted[index+j*numParticles].x = 3.; // = (float4) (3.,3.,3.,3.);
		//dParticlesSorted[index+j*numParticles].y = 4.; // = (float4) (3.,3.,3.,3.);
		//dParticlesSorted[index+j*numParticles].z = 5.; // = (float4) (3.,3.,3.,3.);
		//dParticlesSorted[index+j*numParticles].w = 6.; // = (float4) (3.,3.,3.,3.);
	}
#endif
}
//----------------------------------------------------------------------

//);

#endif
