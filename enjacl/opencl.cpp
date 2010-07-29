#include <string.h>
#include <string>

#include <GL/glew.h>
#if defined __APPLE__ || defined(MACOSX)
    //OpenGL stuff
    #include <OpenGL/gl.h>
    #include <OpenGL/glext.h>
    #include <GLUT/glut.h>
    #include <OpenGL/CGLCurrent.h> //is this really necessary?
#else
    //OpenGL stuff
    #include <GL/glx.h>
#endif



#include "enja.h"
#include "util.h"
#include "timege.h"
//#include "incopencl.h"

int EnjaParticles::update()
{
    cl_int ciErrNum = CL_SUCCESS;
    cl_event evt; //can't do opencl visual profiler without passing an event

 
#ifdef GL_INTEROP   
    // map OpenGL buffer object for writing from OpenCL
    //clFinish(cqCommandQueue);
    ts_cl[3]->start();
    glFinish();
    ts_cl[3]->stop();

	ts_cl[0]->start();
    //ciErrNum = clEnqueueAcquireGLObjects(cqCommandQueue, 1, &vbo_cl, 0,0,0);
    ciErrNum = clEnqueueAcquireGLObjects(cqCommandQueue, 3, cl_vbos, 0,NULL, &evt);
    clReleaseEvent(evt);
    //printf("gl interop, acquire: %s\n", oclErrorString(ciErrNum));
    clFinish(cqCommandQueue);
	ts_cl[0]->stop();
#endif

    //clFinish(cqCommandQueue);
	ts_cl[1]->start();
    ciErrNum = clSetKernelArg(ckKernel, 6, sizeof(float), &dt);
    //ciErrNum = clSetKernelArg(ckKernel, 2, sizeof(float), &dt);
    ciErrNum |= clEnqueueNDRangeKernel(cqCommandQueue, ckKernel, 1, NULL, szGlobalWorkSize, NULL, 0, NULL, &evt );
    clReleaseEvent(evt);
    //printf("enqueueue nd range kernel: %s\n", oclErrorString(ciErrNum));
    clFinish(cqCommandQueue); //wont get reliable timings unless we finish the queue for each action
    ts_cl[1]->stop();

#ifdef GL_INTEROP
    // unmap buffer object
    //ciErrNum = clEnqueueReleaseGLObjects(cqCommandQueue, 1, &vbo_cl, 0,0,0);
    
    //clFinish(cqCommandQueue);
    ts_cl[2]->start();
    ciErrNum = clEnqueueReleaseGLObjects(cqCommandQueue, 3, cl_vbos, 0, NULL, &evt);
    clReleaseEvent(evt);
    //printf("gl interop, acquire: %s\n", oclErrorString(ciErrNum));
    clFinish(cqCommandQueue);
    ts_cl[2]->stop();
#else

    // Explicit Copy 
    // this doesn't get called when we use GL_INTEROP
    glBindBufferARB(GL_ARRAY_BUFFER, v_vbo);    
    // map the buffer object into client's memory
    void* ptr = glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);
    ciErrNum = clEnqueueReadBuffer(cqCommandQueue, cl_vbos[0], CL_TRUE, 0, vbo_size, ptr, 0, NULL, &evt);
    clReleaseEvent(evt);
    glUnmapBufferARB(GL_ARRAY_BUFFER); 
    
    glBindBufferARB(GL_ARRAY_BUFFER, c_vbo);    
    // map the buffer object into client's memory
    ptr = glMapBufferARB(GL_ARRAY_BUFFER, GL_WRITE_ONLY_ARB);
    ciErrNum = clEnqueueReadBuffer(cqCommandQueue, cl_vbos[1], CL_TRUE, 0, vbo_size, ptr, 0, NULL, &evt);
    clReleaseEvent(evt);
    glUnmapBufferARB(GL_ARRAY_BUFFER); 
#endif


}


void EnjaParticles::popCorn()
{


    cl_event evt; //can't do opencl visual profiler without passing an event
    //This is a purely internal helper function, all this code could easily be at the bottom of init_cl
    //init_cl shouldn't change much, and this may
    #ifdef GL_INTEROP
        //printf("gl interop!\n");
        // create OpenCL buffer from GL VBO
        cl_vbos[0] = clCreateFromGLBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, v_vbo, &ciErrNum);
        cl_vbos[1] = clCreateFromGLBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, c_vbo, &ciErrNum);
        cl_vbos[2] = clCreateFromGLBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, i_vbo, &ciErrNum);
        //printf("SUCCES?: %s\n", oclErrorString(ciErrNum));
    #else
        //printf("no gl interop!\n");
        // create standard OpenCL mem buffer
        cl_vbos[0] = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo_size, NULL, &ciErrNum);
        cl_vbos[1] = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo_size, NULL, &ciErrNum);
        cl_vbos[2] = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, sizeof(int) * num, NULL, &ciErrNum);
        //Since we don't get the data from OpenGL we have to manually push the CPU side data to the GPU
        ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_vbos[0], CL_TRUE, 0, vbo_size, &vert_gen[0], 0, NULL, &evt);
        clReleaseEvent(evt);
        ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_vbos[1], CL_TRUE, 0, vbo_size, &colors[0], 0, NULL, &evt);
        clReleaseEvent(evt);
        ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_vbos[2], CL_TRUE, 0, sizeof(int) * num, &colors[0], 0, NULL, &evt);
        clReleaseEvent(evt);
        //make sure we are finished copying over before going on
    #endif
    
    //support arrays for the particle system
    cl_vert_gen = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo_size, NULL, &ciErrNum);
    cl_velo_gen = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo_size, NULL, &ciErrNum);
    cl_velocities = clCreateBuffer(cxGPUContext, CL_MEM_WRITE_ONLY, vbo_size, NULL, &ciErrNum);
    
    ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_vert_gen, CL_TRUE, 0, vbo_size, &vert_gen[0], 0, NULL, &evt);
    clReleaseEvent(evt);
    ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_velo_gen, CL_TRUE, 0, vbo_size, &velo_gen[0], 0, NULL, &evt);
    clReleaseEvent(evt);
    ciErrNum = clEnqueueWriteBuffer(cqCommandQueue, cl_velocities, CL_TRUE, 0, vbo_size, &velo_gen[0], 0, NULL, &evt);
    clReleaseEvent(evt);
    clFinish(cqCommandQueue);
    

    //printf("about to set kernel args\n");
    ciErrNum  = clSetKernelArg(ckKernel, 0, sizeof(cl_mem), (void *) &cl_vbos[0]);      //vertices is first arguement to kernel
    ciErrNum  = clSetKernelArg(ckKernel, 1, sizeof(cl_mem), (void *) &cl_vbos[1]);      //colors is second arguement to kernel
    ciErrNum  = clSetKernelArg(ckKernel, 2, sizeof(cl_mem), (void *) &cl_vbos[2]);      //indices is third arguement to kernel
    ciErrNum  = clSetKernelArg(ckKernel, 3, sizeof(cl_mem), (void *) &cl_vert_gen);     //vertex vert_gen
    ciErrNum  = clSetKernelArg(ckKernel, 4, sizeof(cl_mem), (void *) &cl_velo_gen);     //velocity gen
    ciErrNum  = clSetKernelArg(ckKernel, 5, sizeof(cl_mem), (void *) &cl_velocities);     //velocities
    //we now pack life into the w component of velocity
    //ciErrNum  = clSetKernelArg(ckKernel, 6, sizeof(cl_mem), (void *) &cl_life);         //life
    printf("done with popCorn()\n");

}


int EnjaParticles::init_cl()
{
    setup_cl();
   
    cqCommandQueue = clCreateCommandQueue(cxGPUContext, cdDevices[uiDeviceUsed], 0, &ciErrNum);
    //shrCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Program Setup
    int pl;
    size_t program_length;
    //printf("open the program\n");
    
    //CL_SOURCE_DIR is set in the CMakeLists.txt
    std::string path(CL_SOURCE_DIR);
    path += programs[system];
    printf("path to opencl file: %s\n", path.c_str());
    char* cSourceCL = file_contents(path.c_str(), &pl);
    //printf("file: %s\n", cSourceCL);
    program_length = (size_t)pl;

    // create the program
    cpProgram = clCreateProgramWithSource(cxGPUContext, 1,
                      (const char **) &cSourceCL, &program_length, &ciErrNum);

    //printf("building the opencl program\n");
    // build the program
    ciErrNum = clBuildProgram(cpProgram, 0, NULL, "-cl-fast-relaxed-math", NULL, NULL);
    //ciErrNum = clBuildProgram(cpProgram, 0, NULL, NULL, NULL, NULL);
	if(ciErrNum != CL_SUCCESS){
		cl_build_status build_status;
		ciErrNum = clGetProgramBuildInfo(cpProgram, cdDevices[uiDeviceUsed], CL_PROGRAM_BUILD_STATUS, sizeof(cl_build_status), &build_status, NULL);

		char *build_log;
		size_t ret_val_size;
		ciErrNum = clGetProgramBuildInfo(cpProgram, cdDevices[uiDeviceUsed], CL_PROGRAM_BUILD_LOG, 0, NULL, &ret_val_size);

		build_log = new char[ret_val_size+1];
		ciErrNum = clGetProgramBuildInfo(cpProgram, cdDevices[uiDeviceUsed], CL_PROGRAM_BUILD_LOG, ret_val_size, build_log, NULL);
		build_log[ret_val_size] = '\0';
		printf("BUILD LOG: \n %s", build_log);
	}
/*
    if (ciErrNum != CL_SUCCESS)
    {
        printf("houston we have a problem\n%s\n", oclErrorString(ciErrNum));
    }
*/
    //printf("program built\n");
    ckKernel = clCreateKernel(cpProgram, "enja", &ciErrNum);
    printf("kernel made: %s\n", oclErrorString(ciErrNum));


    ts_cl[0] = new GE::Time("acquire", 5);
    ts_cl[1] = new GE::Time("ndrange", 5);
    ts_cl[2] = new GE::Time("release", 5);
    ts_cl[3] = new GE::Time("glFinish", 5);

    popCorn();

    return 1;
}




int EnjaParticles::setup_cl()
{
    //setup devices and context
    szGlobalWorkSize[0] = num; //set the workgroup size to number of particles

    cl_int ciErrNum;
    //Get the NVIDIA platform
    ciErrNum = oclGetPlatformID(&cpPlatform);
    //oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Get the number of GPU devices available to the platform
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, 0, NULL, &uiDevCount);
    //oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Create the device list
    cdDevices = new cl_device_id [uiDevCount];
    ciErrNum = clGetDeviceIDs(cpPlatform, CL_DEVICE_TYPE_GPU, uiDevCount, cdDevices, NULL);
    //oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Get device requested on command line, if any
    uiDeviceUsed = 0;
    unsigned int uiEndDev = uiDevCount - 1;

    bool bSharingSupported = false;
    for(unsigned int i = uiDeviceUsed; (!bSharingSupported && (i <= uiEndDev)); ++i) 
    {
        size_t extensionSize;
        ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, 0, NULL, &extensionSize );
        //oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
        if(extensionSize > 0) 
        {
            char* extensions = (char*)malloc(extensionSize);
            ciErrNum = clGetDeviceInfo(cdDevices[i], CL_DEVICE_EXTENSIONS, extensionSize, extensions, &extensionSize);
            //oclCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);
            std::string stdDevString(extensions);
            free(extensions);

            size_t szOldPos = 0;
            size_t szSpacePos = stdDevString.find(' ', szOldPos); // extensions string is space delimited
            while (szSpacePos != stdDevString.npos)
            {
                if( strcmp(GL_SHARING_EXTENSION, stdDevString.substr(szOldPos, szSpacePos - szOldPos).c_str()) == 0 ) 
                {
                    // Device supports context sharing with OpenGL
                    uiDeviceUsed = i;
                    bSharingSupported = true;
                    break;
                }
                do 
                {
                    szOldPos = szSpacePos + 1;
                    szSpacePos = stdDevString.find(' ', szOldPos);
                } 
                while (szSpacePos == szOldPos);
            }
        }
    }

    // Define OS-specific context properties and create the OpenCL context
    //#if defined (__APPLE_CC__)
    #if defined (__APPLE__) || defined(MACOSX)
        CGLContextObj kCGLContext = CGLGetCurrentContext();
        CGLShareGroupObj kCGLShareGroup = CGLGetShareGroup(kCGLContext);
        cl_context_properties props[] =
        {
            CL_CONTEXT_PROPERTY_USE_CGL_SHAREGROUP_APPLE, (cl_context_properties)kCGLShareGroup,
            0
        };
        cxGPUContext = clCreateContext(props, 0,0, NULL, NULL, &ciErrNum);
    #else
        #if defined WIN32 // Win32
            cl_context_properties props[] = 
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)wglGetCurrentContext(), 
                CL_WGL_HDC_KHR, (cl_context_properties)wglGetCurrentDC(), 
                CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 
                0
            };
            cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
        #else
            cl_context_properties props[] = 
            {
                CL_GL_CONTEXT_KHR, (cl_context_properties)glXGetCurrentContext(), 
                CL_GLX_DISPLAY_KHR, (cl_context_properties)glXGetCurrentDisplay(), 
                CL_CONTEXT_PLATFORM, (cl_context_properties)cpPlatform, 
                0
            };
            cxGPUContext = clCreateContext(props, 1, &cdDevices[uiDeviceUsed], NULL, NULL, &ciErrNum);
        #endif
    #endif
    //shrCheckErrorEX(ciErrNum, CL_SUCCESS, pCleanup);

    // Log device used (reconciled for requested requested and/or CL-GL interop capable devices, as applies)
    //shrLog("Device # %u, ", uiDeviceUsed);
    //oclPrintDevName(LOGBOTH, cdDevices[uiDeviceUsed]);
 
}
