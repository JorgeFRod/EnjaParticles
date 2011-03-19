#ifndef RTPS_RENDER_H_INCLUDED
#define RTPS_RENDER_H_INCLUDED

#include <map>

#include <GL/glew.h>

/*#if defined __APPLE__ || defined(MACOSX)
    #include <GLUT/glut.h>
#else
    #include <GL/glut.h>
//OpenCL stuff
#endif*/


#if defined __APPLE__ || defined(MACOSX)
//OpenGL stuff
    #include <GLUT/glut.h>
    #include <OpenGL/gl.h>
#else
//OpenGL stuff
    #include <GL/glut.h>
    #include <GL/gl.h>
#endif



#include "RTPSettings.h"
#include "../structs.h"
#include "../timege.h"
#include "../opencl/CLL.h"
#include "../opencl/Kernel.h"
#include "../opencl/Buffer.h"

namespace rtps
{

    /*enum Shaders
    {
        SHADER_DEPTH=0,SHADER_CURVATURE_FLOW,SHADER_FRESNEL
    };*/

    class Render
    {
    public:
        Render(GLuint pos_vbo, GLuint vel_vbo, int num, CL *cli, RTPSettings* _settings=0);
        //Render(GLuint pos_vbo, GLuint vel_vbo, int num, CL *cli, RTPSettings& _settings);
        ~Render();

        //decide which kind of rendering to use
        enum RenderType
        {
            POINTS, SPRITES
        };
        enum ShaderType
        {
            NO_SHADER,SPHERE_SHADER, DEPTH_SHADER, GAUSSIAN_X_SHADER, GAUSSIAN_Y_SHADER, BILATERAL_GAUSSIAN_SHADER, NORMAL_SHADER, CURVATURE_FLOW_SHADER, MIKEP_SHADER, COPY_TO_FB
        };

        void setNum(int nn)
        {
            num = nn;
        }
        void setDepthSmoothing(ShaderType shade)
        {
            smoothing = shade;
        }
        void setParticleRadius(float pradius);

        void drawArrays();

        void renderPointsAsSpheres();


        void orthoProjection();
        void perspectiveProjection();
        void fullscreenQuad();

        void render_box(float4 min, float4 max); //put in render utils
        void render_table(float4 min, float4 max); //put in render utils


        void writeBuffersToDisk();
        void writeFramebufferTextures();
        int writeTexture(GLuint tex, const char* filename) const;

        enum
        {
            TI_RENDER=0, TI_GLSL
        }; //2
        GE::Time* timers[2];
        int setupTimers();
        void printTimers();

        virtual void render();
        virtual void setWindowDimensions(GLuint width,GLuint height);

    protected:
        int loadTexture(std::string texture_file, std::string texture_name);
        GLuint compileShaders(const char* vertex_file, const char* fragment_file, const char* geometry_file = NULL, GLenum* geom_param=NULL, GLint* geom_value=NULL, int geom_param_len=0);
        std::map<ShaderType,GLuint> glsl_program;    
        std::map<std::string,GLuint> gl_framebuffer_texs;
        std::map<std::string,GLuint> gl_textures;
        virtual void deleteFramebufferTextures();
        virtual void createFramebufferTextures();
        GLuint window_height,window_width;
        GLuint pos_vbo;
        GLuint col_vbo;
        CL *cli;
        float particle_radius;
        float near_depth;
        float far_depth;
        //number of particles
        int num;

        RenderType rtype;
        bool blending;
        bool write_framebuffers;
        ShaderType smoothing;
        std::vector<GLuint> fbos;
        std::vector<GLuint> rbos;
        Buffer<float>   cl_depth;
        Kernel  k_curvature_flow;


        // Added by GE, March 6, 2011
        // reference guarantees the location pointed to cannot be changed
        // But that is creating problems because one cannot have a default 
        // property for a reference
        RTPSettings* settings; 


        int generateCheckerBoardTex(GLubyte* color1,GLubyte* color2,int num_squares, int length);
        int generateCircleTexture(GLubyte r, GLubyte g, GLubyte b, GLubyte alpha, int diameter);

        void convertDepthToRGB(const GLfloat* depth, GLuint size, GLubyte* rgb) const;

        //GE
        float getParticleRadius()
        {
            return particle_radius;
        }
    };  


}

#endif
