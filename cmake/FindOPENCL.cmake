# - Try to find OpenCL
# Once done this will define
#  
#  OPENCL_FOUND        - system has OpenCL
#  OPENCL_INCLUDE_DIR  - the OpenCL include directory
#  OPENCL_LIBRARIES    - link these to use OpenCL
#
# WIN32 should work, but is untested

IF (WIN32)

	IF(NOT ${GPU_COMPUTING_TOOLKIT})
		# CYGWIN: 
		SET (GPU_COMPUTING_TOOLKIT "/cygdrive/c/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v3.2/")
	ENDIF (NOT ${GPU_COMPUTING_TOOLKIT}) 

    FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h 
		$ENV{GPU_COMPUTING_TOOLKIT}/include
	)

    # TODO this is only a hack assuming the 64 bit library will
    # not be found on 32 bit system
    FIND_LIBRARY(OPENCL_LIBRARIES 
		NAMES opencl OpenCL opencl32 opencl64
		PATHS
		${GPU_COMPUTING_TOOLKIT}/lib/x64
		${GPU_COMPUTING_TOOLKIT}/lib/Win32
	)

ELSE (WIN32)

    # Unix style platforms
    # We also search for OpenCL in the NVIDIA GPU SDK default location
    #SET(OPENCL_INCLUDE_DIR  "$ENV{OPENCL_HOME}/common/inc"
    #   CACHE PATH "path to Opencl Include files")

    message(***** OPENCL_INCLUDE_DIR: "${OPENCL_INCLUDE_DIR}" ********)

	# does not work. WHY? 
    #SET(inc  $ENV{CUDA_LOCAL}/../OpenCL/common/inc /usr/include)
    #FIND_PATH(OPENCL_INCLUDE_DIR CL/cl.h PATHS ${inc} /usr/include )

    FIND_LIBRARY(OPENCL_LIBRARIES OpenCL ENV LD_LIBRARY_PATH)

    message(***** OPENCL ENV: "$ENV{GPU_SDK}" ********)

#~/NVIDIA_GPU_Computing_SDK/OpenCL/common/inc/ 


ENDIF (WIN32)

SET( OPENCL_FOUND "NO" )
IF(OPENCL_LIBRARIES )
    SET( OPENCL_FOUND "YES" )
ENDIF(OPENCL_LIBRARIES)

MARK_AS_ADVANCED(
  OPENCL_INCLUDE_DIR
)
