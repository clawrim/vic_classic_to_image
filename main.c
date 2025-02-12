#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "vic.h"

int main(int argc, char **argv)
{
    int i = 1;
    char *classic_gp_path, *image_prefix;
    struct global_params_s *gp;
    struct soil_s *soil;
    struct veg_lib_s *veg_lib;
    struct veg_params_s *veg_params;

    if (argc < 3)
        /*
           error
           ("Usage: vic_soil2nc classic_global.txt image_global.txt domain.nc domain_type:nc_name,... params.nc histfreq:count,...\n");
         */
        error("Usage: vic_soil2nc classic_global.txt image_prefix\n");

    classic_gp_path = argv[i++];
    image_prefix = argv[i++];

    gp = read_global_params(classic_gp_path);
    if (!is_classic(gp))
        error("Not a classic global parameters file: %s\n", classic_gp_path);

    populate_image_global_params(gp, image_prefix);

    soil = read_classic_soil(gp);
    veg_lib = read_classic_veg_lib(gp);
    veg_params = read_classic_veg_params(gp);

    create_image_domain(gp, soil->domain);
    create_image_params(gp, soil, veg_lib, veg_params);

    free_global_params(gp);
    free_soil(soil);
    free_veg_lib(veg_lib);
    free_veg_params(veg_params);

    exit(EXIT_SUCCESS);
}
