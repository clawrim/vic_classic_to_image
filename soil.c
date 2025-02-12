#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "global.h"
#include "double_stack.h"
#include "vic.h"

struct latlon_s
{
    double lat, lon;
} *ll;

static int compare_soil_cells(const void *, const void *);
static double calc_cell_area_m2(struct global_params_s *, double, double);
static double calc_distance_m(double, double, double, double);

#define swapbuf() do { char *p = p1; p1 = p2; p2 = p; } while(0)
static char buf1[BUF_SIZE], buf2[BUF_SIZE], *p1 = buf1, *p2 = buf2;

struct soil_s *read_classic_soil(struct global_params_s *gp)
{
    struct soil_s *soil;
    struct domain_s *domain;
    struct soil_cell_s **cells;
    FILE *fp;
    int nalloc;
    int i, j, k;

    if (!(fp = fopen(gp->soil, "r")))
        error("Cannot open file: %s\n", gp->soil);

    nalloc = 0;

    soil = malloc(sizeof *soil);
    soil->domain = domain = malloc(sizeof *domain);
    domain->lat = malloc(sizeof *domain->lat);
    domain->lon = malloc(sizeof *domain->lon);

    init_double_stack_s(domain->lat);
    init_double_stack_s(domain->lon);

    soil->n_cells = 0;
    soil->cells = cells = NULL;

    while (fgets(p1, BUF_SIZE, fp)) {
        int run_cell, fs_active;
        struct soil_cell_s *cell;

        if (soil->n_cells == nalloc) {
            nalloc += REALLOC_INCREMENT;
            soil->cells = cells = realloc(cells, sizeof *cells * nalloc);
        }
        cells[soil->n_cells++] = cell = malloc(sizeof *cell);

        sscanf(p1, "%d %d %lf %lf %lf %lf %lf %lf %lf %[^\r\n]", &run_cell,
               &cell->gridcel, &cell->lat, &cell->lon, &cell->infilt,
               &cell->Ds, &cell->Dsmax, &cell->Ws, &cell->c, p2);
        swapbuf();

        cell->run_cell = run_cell;

        if (find_double(domain->lat, cell->lat) < 0)
            push_double(domain->lat, cell->lat);

        if (find_double(domain->lon, cell->lon) < 0)
            push_double(domain->lon, cell->lon);

        cell->expt = malloc(sizeof *cell->expt * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->expt[i], p2);
            swapbuf();
        }

        cell->Ksat = malloc(sizeof *cell->Ksat * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->Ksat[i], p2);
            swapbuf();
        }

        cell->phi_s = malloc(sizeof *cell->phi_s * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->phi_s[i], p2);
            swapbuf();
        }

        cell->init_moist = malloc(sizeof *cell->init_moist * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->init_moist[i], p2);
            swapbuf();
        }

        sscanf(p1, "%lf %[^\r\n]", &cell->elev, p2);
        swapbuf();

        cell->depth = malloc(sizeof *cell->depth * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->depth[i], p2);
            swapbuf();
        }

        sscanf(p1, "%lf %lf %[^\r\n]", &cell->avg_T, &cell->dp, p2);
        swapbuf();

        cell->bubble = malloc(sizeof *cell->bubble * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->bubble[i], p2);
            swapbuf();
        }

        cell->quartz = malloc(sizeof *cell->quartz * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->quartz[i], p2);
            swapbuf();
        }

        cell->bulk_density = malloc(sizeof *cell->bulk_density * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->bulk_density[i], p2);
            swapbuf();
        }

        cell->soil_density = malloc(sizeof *cell->soil_density * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->soil_density[i], p2);
            swapbuf();
        }

        if (gp->organic_fract) {
            cell->organic = malloc(sizeof *cell->organic * gp->nlayer);
            for (i = 0; i < gp->nlayer; i++) {
                sscanf(p1, "%lf %[^\r\n]", &cell->organic[i], p2);
                swapbuf();
            }

            cell->bulk_dens_org =
                malloc(sizeof *cell->bulk_dens_org * gp->nlayer);
            for (i = 0; i < gp->nlayer; i++) {
                sscanf(p1, "%lf %[^\r\n]", &cell->bulk_dens_org[i], p2);
                swapbuf();
            }

            cell->soil_dens_org =
                malloc(sizeof *cell->soil_dens_org * gp->nlayer);
            for (i = 0; i < gp->nlayer; i++) {
                sscanf(p1, "%lf %[^\r\n]", &cell->soil_dens_org[i], p2);
                swapbuf();
            }
        }
        else
            cell->organic = cell->bulk_dens_org = cell->soil_dens_org = NULL;

        sscanf(p1, "%lf %[^\r\n]", &cell->off_gmt, p2);
        swapbuf();

        cell->Wcr_FRACT = malloc(sizeof *cell->Wcr_FRACT * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->Wcr_FRACT[i], p2);
            swapbuf();
        }

        cell->Wpwp_FRACT = malloc(sizeof *cell->Wpwp_FRACT * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->Wpwp_FRACT[i], p2);
            swapbuf();
        }

        sscanf(p1, "%lf %lf %lf %[^\r\n]", &cell->rough, &cell->snow_rough,
               &cell->annual_prec, p2);
        swapbuf();

        cell->resid_moist = malloc(sizeof *cell->resid_moist * gp->nlayer);
        for (i = 0; i < gp->nlayer; i++) {
            sscanf(p1, "%lf %[^\r\n]", &cell->resid_moist[i], p2);
            swapbuf();
        }

        sscanf(p1, "%d %[^\r\n]", &fs_active, p2);
        swapbuf();

        cell->fs_active = fs_active;

        if (gp->spatial_frost) {
            sscanf(p1, "%lf %lf %[^\r\n]", &cell->frost_slope,
                   &cell->max_snow_distrib_slope, p2);
            swapbuf();
        }

        if (gp->july_tavg_supplied) {
            sscanf(p1, "%lf %[^\r\n]", &cell->July_Tavg, p2);
            swapbuf();
        }
    }

    if (ferror(fp))
        error("Cannot read file: %s\n", gp->soil);

    fclose(fp);

    qsort(domain->lat->values, domain->lat->n, sizeof(double),
          compare_doubles);
    qsort(domain->lon->values, domain->lon->n, sizeof(double),
          compare_doubles);

    if (!gp->lakes && domain->lat->n >= 2)
        gp->resolution = domain->lat->values[1] - domain->lat->values[0];
    else
        error("Cannot determine resolution\n");

    qsort(cells, soil->n_cells, sizeof(struct soil_cell_s *),
          compare_soil_cells);

    domain->mask =
        malloc(sizeof *domain->mask * domain->lat->n * domain->lon->n);
    domain->area =
        malloc(sizeof *domain->area * domain->lat->n * domain->lon->n);
    domain->frac =
        malloc(sizeof *domain->frac * domain->lat->n * domain->lon->n);

    k = 0;
    for (i = 0; i < domain->lat->n; i++)
        for (j = 0; j < domain->lon->n; j++) {
            int idx = i * domain->lon->n + j;
            double lat = domain->lat->values[i], lon = domain->lon->values[j];

            if (k < soil->n_cells && cells[k]->lat == lat &&
                cells[k]->lon == lon) {
                domain->mask[idx] = 1;
                domain->area[idx] = calc_cell_area_m2(gp, lat, lon);
                /* TODO: calculate frac, but classic input doesn't have this
                 * info */
                domain->frac[idx] = 1;
                k++;
            }
            else {
                domain->mask[idx] = 0;
                domain->area[idx] = 0;
                domain->frac[idx] = 0;
            }
        }

    return soil;
}

static int compare_soil_cells(const void *p1, const void *p2)
{
    struct soil_cell_s *cell1 = *((struct soil_cell_s **)p1);
    struct soil_cell_s *cell2 = *((struct soil_cell_s **)p2);

    return cell1->lat > cell2->lat ? 1 : (cell1->lat <
                                          cell2->lat ? -1 : (cell1->lon >
                                                             cell2->lon) -
                                          (cell1->lon < cell2->lon));
}

/* adopted from VIC/vic/drivers/classic/src/compute_cell_area.c */
static double calc_cell_area_m2(struct global_params_s *gp, double lat,
                                double lon)
{
    double area;

    if (gp->equal_area)
        area = gp->resolution * 1e6;    /* km2 to m2 */
    else {
        int n = 10, i;
        double half = gp->resolution / 2, dlat = gp->resolution / n;
        double s, w, e, dy;

        lat = fabs(lat);
        lon = fabs(lon);

        s = lat - half;
        w = lon - half;
        e = lon + half;

        dy = calc_distance_m(lat, lon, lat + dlat, lon);

        area = 0;
        for (i = 0; i < n; i++) {
            area += calc_distance_m(s, w, s, e) * dy;
            s += dlat;
        }
    }

    return area;
}

/* adopted from VIC/vic/drivers/classic/src/get_dist.c */
static double calc_distance_m(double lat1, double lon1, double lat2,
                              double lon2)
{
    double distance;
    double theta1;
    double phi1;
    double theta2;
    double phi2;
    double dtor;
    double term1;
    double term2;
    double term3;
    double tmp;

    dtor = 2.0 * M_PI / 360.0;
    theta1 = dtor * lon1;
    phi1 = dtor * lat1;
    theta2 = dtor * lon2;
    phi2 = dtor * lat2;
    term1 = cos(phi1) * cos(theta1) * cos(phi2) * cos(theta2);
    term2 = cos(phi1) * sin(theta1) * cos(phi2) * sin(theta2);
    term3 = sin(phi1) * sin(phi2);
    tmp = term1 + term2 + term3;
    tmp = (double)(1.0 < tmp) ? 1.0 : tmp;
    distance = CONST_REARTH * acos(tmp);

    return distance;
}

void free_soil(struct soil_s *soil)
{
    int i;

    free_double_stack_s(soil->domain->lat);
    free_double_stack_s(soil->domain->lon);
    free(soil->domain);

    for (i = 0; i < soil->n_cells; i++) {
        free(soil->cells[i]->expt);
        free(soil->cells[i]->Ksat);
        free(soil->cells[i]->phi_s);
        free(soil->cells[i]->init_moist);
        free(soil->cells[i]->depth);
        free(soil->cells[i]->bubble);
        free(soil->cells[i]->quartz);
        free(soil->cells[i]->bulk_density);
        free(soil->cells[i]->soil_density);
        free(soil->cells[i]->organic);
        free(soil->cells[i]->bulk_dens_org);
        free(soil->cells[i]->soil_dens_org);
        free(soil->cells[i]->Wcr_FRACT);
        free(soil->cells[i]->Wpwp_FRACT);
        free(soil->cells[i]->resid_moist);
        free(soil->cells[i]);
    }

    free(soil->cells);
    free(soil);
}
