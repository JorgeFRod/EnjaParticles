#include "RTPSettings.h"
namespace rtps
{


    RTPSettings::RTPSettings()
    {
        system = SPH;
        max_particles = 2048;
        dt = .001f;
        grid = Domain(float4(-5,-.3f,0,0), float4(2, 2, 12, 0));
    }

    RTPSettings::RTPSettings(SysType system, int max_particles, float dt)
    {
        this->system = system;
        this->max_particles = max_particles;
        this->dt = dt;
        grid = Domain(float4(-5,-.3f,0,0), float4(2, 2, 12, 0));
    }

    RTPSettings::RTPSettings(SysType system, int max_particles, float dt, Domain grid)
    {
        this->system = system;
        this->max_particles = max_particles;
        this->dt = dt;
        this->grid = grid;
    }

//with triangle collision
RTPSettings::RTPSettings(SysType system, int max_particles, float dt, Domain grid, bool tri_collision)
{
    this->system = system;
    this->max_particles = max_particles;
    this->dt = dt;
    this->grid = grid;
    this->tri_collision = tri_collision;
}
RTPSettings::RTPSettings(SysType system, int max_particles, float dt, Domain grid, float maxspeed, float mindist, float searchradius, float color[])
{
    this->system = system;
    this->max_particles = max_particles;
    this->dt = dt;
    this->grid = grid;
    this->max_speed = maxspeed;
    this->min_dist = mindist;
    this->search_radius = searchradius;
    float r = color[0]; float g = color[1]; float b = color[2]; 
    this->color = float4(r,g,b,1.f);
}


    void RTPSettings::printSettings()
    {
        printf("SPH Settings\n");
        typedef std::map <std::string, std::string> MapType;

        MapType::const_iterator end = settings.end();
        for(MapType::const_iterator it = settings.begin(); it != end; ++it)
        {
            printf("%s: %s\n", it->first.c_str(), it->second.c_str());
        }
    }


}
