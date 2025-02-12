#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "global.h"
#include "double_stack.h"
#include "vic.h"

void create_image_domain(struct global_params_s *gp, struct domain_s *domain)
{
    int i;
    int ncid, dimids[LON + 1], varids[XDIM + 1];
    char *lat = NULL, *lon = NULL, *varnames[XDIM + 1];

    nc_check(nc_create(gp->domain, NC_CLOBBER, &ncid),
             "Cannot create file: %s\n", gp->domain);

    /* dimensions */
    for (i = 0; i < gp->n_domain_types; i++) {
        switch (gp->domain_type[i]->variable) {
        case LAT:
            lat = gp->domain_type[i]->nc_name;
            break;
        case LON:
            lon = gp->domain_type[i]->nc_name;
            break;
        default:
            continue;
        }
    }

    if (!lat)
        error("NetCDF name not defined for LAT\n");
    if (!lon)
        error("NetCDF name not defined for LON\n");

    i = 0;
    nc_check(nc_def_dim(ncid, lat, domain->lat->n, &dimids[i++]),
             "Cannot define dimension: %s\n", lat);
    nc_check(nc_def_dim(ncid, lon, domain->lon->n, &dimids[i++]),
             "Cannot define dimension: %s\n", lon);

    /* variables */
    for (i = 0; i < gp->n_domain_types; i++) {
        enum domain_variable var = gp->domain_type[i]->variable;
        int int_fill = 0;
        double double_fill = 0;

        varnames[var] = gp->domain_type[i]->nc_name;

        switch (var) {
        case LAT:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 1, dimids,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            break;
        case LON:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 1, dimids + 1,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            break;
        case MASK:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_INT, 2, dimids, &varids[var]),
                     "Cannot define variable: %s\n", varnames[var]);
            nc_check(nc_put_att
                     (ncid, varids[var], "_FillValue", NC_INT, 1, &int_fill),
                     "Cannot put attribute: %s\n", varnames[var]);
            break;
        case AREA:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 2, dimids,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            nc_check(nc_put_att
                     (ncid, varids[var], "_FillValue", NC_DOUBLE, 1,
                      &double_fill), "Cannot put attribute: %s\n",
                     varnames[var]);
            nc_check(nc_put_att_text
                     (ncid, varids[var], "units", strlen(AREA_UNITS),
                      AREA_UNITS), "Cannot put attribute: %s\n",
                     varnames[var]);
            break;
        case FRAC:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 2, dimids,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            nc_check(nc_put_att
                     (ncid, varids[var], "_FillValue", NC_DOUBLE, 1,
                      &double_fill), "Cannot put attribute: %s\n",
                     varnames[var]);
            break;
        case YDIM:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 1, dimids,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            break;
        case XDIM:
            nc_check(nc_def_var
                     (ncid, varnames[var], NC_DOUBLE, 1, dimids + 1,
                      &varids[var]), "Cannot define variable: %s\n",
                     varnames[var]);
            break;
        }
    }

    nc_check(nc_enddef(ncid), "Cannot end definition\n");

    /* populate variables */
    nc_check(nc_put_var(ncid, varids[LAT], domain->lat->values),
             "Cannot put variable: %s\n", lat);
    nc_check(nc_put_var(ncid, varids[LON], domain->lon->values),
             "Cannot put variable: %s\n", lon);

    nc_check(nc_put_var(ncid, varids[MASK], domain->mask),
             "Cannot put variable: %s\n", varnames[MASK]);
    nc_check(nc_put_var(ncid, varids[AREA], domain->area),
             "Cannot put variable: %s\n", varnames[AREA]);
    nc_check(nc_put_var(ncid, varids[FRAC], domain->frac),
             "Cannot put variable: %s\n", varnames[FRAC]);

    nc_check(nc_close(ncid), "Cannot close file: %s\n", gp->domain);
}
