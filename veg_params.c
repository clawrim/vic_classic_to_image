#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "double_stack.h"
#include "vic.h"

#define swapbuf() do { char *p = p1; p1 = p2; p2 = p; } while(0)
static char buf1[BUF_SIZE], buf2[BUF_SIZE], *p1 = buf1, *p2 = buf2;

struct veg_params_s *read_classic_veg_params(struct global_params_s *gp)
{
    struct veg_params_s *veg_params;
    struct veg_cell_s **cells;
    FILE *fp;
    int nalloc;

    if (!(fp = fopen(gp->vegparam, "r")))
        error("Cannot open file: %s\n", gp->vegparam);

    nalloc = 0;

    veg_params = malloc(sizeof *veg_params);
    veg_params->root_zones = gp->root_zones;
    veg_params->n_cells = 0;
    veg_params->cells = cells = NULL;

    while (fgets(p1, BUF_SIZE, fp)) {
        struct veg_cell_s *cell;
        int i;

        if (p1[0] == '#' || p1[0] == '\r' || p1[0] == '\n' || !p1[0])
            continue;

        if (sscanf(p1, "%s", p2) != 1 || p2[0] == '#' || !p2[0])
            continue;

        if (veg_params->n_cells == nalloc) {
            nalloc += REALLOC_INCREMENT;
            veg_params->cells = cells =
                realloc(cells, sizeof *cells * nalloc);
        }
        cells[veg_params->n_cells++] = cell = malloc(sizeof *cell);

        sscanf(p1, "%d %d", &cell->gridcel, &cell->Nveg);

        if (!cell->Nveg) {
            cell->veg_class = NULL;
            cell->Cv = NULL;
            cell->root_depth = cell->root_fract = NULL;
            cell->LAI = cell->FCANOPY = cell->ALBEDO = NULL;
            continue;
        }

        cell->veg_class = malloc(sizeof *cell->veg_class * cell->Nveg);
        cell->Cv = malloc(sizeof *cell->Cv * cell->Nveg);
        cell->root_depth = malloc(sizeof *cell->root_depth * cell->Nveg);
        cell->root_fract = malloc(sizeof *cell->root_fract * cell->Nveg);

        if (gp->vegparam_lai)
            cell->LAI = malloc(sizeof *cell->LAI * cell->Nveg);
        else
            cell->LAI = NULL;

        if (gp->vegparam_fcan)
            cell->FCANOPY = malloc(sizeof *cell->FCANOPY * cell->Nveg);
        else
            cell->FCANOPY = NULL;

        if (gp->vegparam_alb)
            cell->ALBEDO = malloc(sizeof *cell->ALBEDO * cell->Nveg);
        else
            cell->ALBEDO = NULL;

        for (i = 0; i < cell->Nveg; i++) {
            int j;

            if (!fgets(p1, BUF_SIZE, fp))
                error("Incorrect format: %s\n", gp->vegparam);

            sscanf(p1, "%d %lf %[^\r\n]", &cell->veg_class[i], &cell->Cv[i],
                   p2);
            swapbuf();

            if (veg_params->root_zones) {
                cell->root_depth[i] =
                    malloc(sizeof *cell->root_depth[i] *
                           veg_params->root_zones);
                cell->root_fract[i] =
                    malloc(sizeof *cell->root_fract[i] *
                           veg_params->root_zones);

                for (j = 0; j < veg_params->root_zones; j++) {
                    sscanf(p1, "%lf %lf %[^\r\n]", &cell->root_depth[i][j],
                           &cell->root_fract[i][j], p2);
                    swapbuf();
                }
            }
            else
                cell->root_depth[i] = cell->root_fract[i] = NULL;

            if (gp->blowing)
                sscanf(p1, "%lf %lf %lf", &cell->sigma_slope, &cell->lag_one,
                       &cell->fetch);

            if (gp->vegparam_lai) {
                if (!fgets(p1, BUF_SIZE, fp))
                    error("Incorrect format: %s\n", gp->vegparam);

                cell->LAI[i] = malloc(sizeof *cell->LAI[i] * 12);
                for (j = 0; j < 12; j++) {
                    sscanf(p1, "%lf %[^\r\n]", &cell->LAI[i][j], p2);
                    swapbuf();
                }
            }

            if (gp->vegparam_fcan) {
                if (!fgets(p1, BUF_SIZE, fp))
                    error("Incorrect format: %s\n", gp->vegparam);

                cell->FCANOPY[i] = malloc(sizeof *cell->FCANOPY[i] * 12);
                for (j = 0; j < 12; j++) {
                    sscanf(p1, "%lf %[^\r\n]", &cell->FCANOPY[i][j], p2);
                    swapbuf();
                }
            }

            if (gp->vegparam_alb) {
                if (!fgets(p1, BUF_SIZE, fp))
                    error("Incorrect format: %s\n", gp->vegparam);

                cell->ALBEDO[i] = malloc(sizeof *cell->ALBEDO[i] * 12);
                for (j = 0; j < 12; j++) {
                    sscanf(p1, "%lf %[^\r\n]", &cell->ALBEDO[i][j], p2);
                    swapbuf();
                }
            }
        }
    }

    if (ferror(fp))
        error("Cannot read file: %s\n", gp->vegparam);

    fclose(fp);

    return veg_params;
}

void free_veg_params(struct veg_params_s *veg_params)
{
    int i;

    for (i = 0; i < veg_params->n_cells; i++) {
        int j;

        free(veg_params->cells[i]->veg_class);
        free(veg_params->cells[i]->Cv);
        for (j = 0; j < veg_params->cells[i]->Nveg; j++) {
            free(veg_params->cells[i]->root_depth[j]);
            free(veg_params->cells[i]->root_fract[j]);
            if (veg_params->cells[i]->LAI)
                free(veg_params->cells[i]->LAI[j]);
            if (veg_params->cells[i]->FCANOPY)
                free(veg_params->cells[i]->FCANOPY[j]);
            if (veg_params->cells[i]->ALBEDO)
                free(veg_params->cells[i]->ALBEDO[j]);
        }
        free(veg_params->cells[i]->root_depth);
        free(veg_params->cells[i]->root_fract);
        free(veg_params->cells[i]->LAI);
        free(veg_params->cells[i]->FCANOPY);
        free(veg_params->cells[i]->ALBEDO);
        free(veg_params->cells[i]);
    }

    free(veg_params->cells);
    free(veg_params);
}
