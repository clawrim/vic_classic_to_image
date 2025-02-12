#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "double_stack.h"
#include "vic.h"

#define swapbuf() do { char *p = p1; p1 = p2; p2 = p; } while(0)
static char buf1[BUF_SIZE], buf2[BUF_SIZE], *p1 = buf1, *p2 = buf2;

struct veg_lib_s *read_classic_veg_lib(struct global_params_s *gp)
{
    struct veg_lib_s *veg_lib;
    struct veg_class_s **classes;
    FILE *fp;
    int nalloc;

    if (!(fp = fopen(gp->veglib, "r")))
        error("Cannot open file: %s\n", gp->veglib);

    nalloc = 0;

    veg_lib = malloc(sizeof *veg_lib);
    veg_lib->n_classes = 0;
    veg_lib->classes = classes = NULL;

    while (fgets(p1, BUF_SIZE, fp)) {
        struct veg_class_s *class;
        int overstory;
        int i;

        if (p1[0] == '#' || p1[0] == '\r' || p1[0] == '\n' || !p1[0])
            continue;

        if (sscanf(p1, "%s", p2) != 1 || p2[0] == '#' || !p2[0])
            continue;

        if (veg_lib->n_classes == nalloc) {
            nalloc += REALLOC_INCREMENT;
            veg_lib->classes = classes =
                realloc(classes, sizeof *classes * nalloc);
        }
        classes[veg_lib->n_classes++] = class = malloc(sizeof *class);

        sscanf(p1, "%d %d %lf %lf %[^\r\n]", &class->veg_class,
               &overstory, &class->rarc, &class->rmin, p2);
        swapbuf();
        class->overstory = overstory;

        class->LAI = malloc(sizeof *class->LAI * 12);
        for (i = 0; i < 12; i++) {
            sscanf(p1, "%lf %[^\r\n]", &class->LAI[i], p2);
            swapbuf();
        }

        if (gp->veglib_fcan) {
            class->FCANOPY = malloc(sizeof *class->FCANOPY * 12);
            for (i = 0; i < 12; i++) {
                sscanf(p1, "%lf %[^\r\n]", &class->FCANOPY[i], p2);
                swapbuf();
            }
        }
        else
            class->FCANOPY = NULL;

        class->albedo = malloc(sizeof *class->albedo * 12);
        for (i = 0; i < 12; i++) {
            sscanf(p1, "%lf %[^\r\n]", &class->albedo[i], p2);
            swapbuf();
        }

        class->rough = malloc(sizeof *class->rough * 12);
        for (i = 0; i < 12; i++) {
            sscanf(p1, "%lf %[^\r\n]", &class->rough[i], p2);
            swapbuf();
        }

        class->displacement = malloc(sizeof *class->displacement * 12);
        for (i = 0; i < 12; i++) {
            sscanf(p1, "%lf %[^\r\n]", &class->displacement[i], p2);
            swapbuf();
        }

        sscanf(p1, "%lf %lf %lf %lf %lf %[^\r\n]", &class->wind_h,
               &class->RGL, &class->rad_atten, &class->wind_atten,
               &class->trunk_ratio, p2);
        swapbuf();

        if (gp->veglib_photo) {
            int NscaleFlag;

            sscanf(p1, "%d %lf %lf %lf %d %lf %lf %[^\r\n]", &class->Ctype,
                   &class->MaxCarboxRate, &class->MaxETransport,
                   &class->LightUseEff, &NscaleFlag, &class->Wnpp_inhib,
                   &class->NPPfactor_sat, p2);
            swapbuf();
            class->NscaleFlag = NscaleFlag;
        }

        class->comment = malloc(strlen(p1) + 1);
        strcpy(class->comment, p1);
    }

    if (ferror(fp))
        error("Cannot read file: %s\n", gp->veglib);

    fclose(fp);

    return veg_lib;
}

void free_veg_lib(struct veg_lib_s *veg_lib)
{
    int i;

    for (i = 0; i < veg_lib->n_classes; i++) {
        free(veg_lib->classes[i]->LAI);
        free(veg_lib->classes[i]->FCANOPY);
        free(veg_lib->classes[i]->albedo);
        free(veg_lib->classes[i]->rough);
        free(veg_lib->classes[i]->displacement);
        free(veg_lib->classes[i]->comment);
        free(veg_lib->classes[i]);
    }

    free(veg_lib->classes);
    free(veg_lib);
}
