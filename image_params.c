#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netcdf.h>
#include "global.h"
#include "double_stack.h"
#include "vic.h"

void create_image_params(struct global_params_s *gp, struct soil_s *soil,
                         struct veg_lib_s *veg_lib,
                         struct veg_params_s *veg_params)
{
    int ncid;
    int veg_class_dimid, string_dimid, root_zone_dimid, snow_band_dimid,
        month_dimid, nlayer_dimid, lat_dimid, lon_dimid;
    /* dimension variables */
    int veg_class_varid, root_zone_varid, snow_band_varid, month_varid,
        layer_varid, lat_varid, lon_varid;
    /* cell numbers and mask variables */
    int veg_descr_varid, cellnum_varid, mask_varid;

    /* soil variables */
    int run_cell_varid, gridcell_varid, lats_varid, lons_varid, infilt_varid,
        Ds_varid, Dsmax_varid, Ws_varid, c_varid, expt_varid, Ksat_varid,
        phi_s_varid, init_moist_varid, elev_varid, depth_varid, avg_T_varid,
        dp_varid, bubble_varid, quartz_varid, bulk_density_varid,
        soil_density_varid, organic_varid, bulk_dens_org_varid,
        soil_dens_org_varid, off_gmt_varid, Wcr_FRACT_varid, Wpwp_FRACT_varid,
        rough_varid, snow_rough_varid, annual_prec_varid, resid_moist_varid,
        fs_active_varid, frost_slope_varid, max_snow_distrib_slope_varid,
        July_Tavg_varid;
    /* vegetation variables */
    int Nveg_varid, Cv_varid, root_depth_varid, root_fract_varid,
	overstory_varid, rarc_varid, rmin_varid;
    int dimids[4];
    int *ints, nints;
    size_t start[4], count[4];
    int veg_descr_len;
    int d;
    int i;

    /* dimensions */
    nc_check(nc_create(gp->parameters, NC_CLOBBER, &ncid),
             "Cannot create file: %s\n", gp->parameters);

    nints = 12;

    if (veg_lib->n_classes > nints)
        nints = veg_lib->n_classes;
    nc_check(nc_def_dim
             (ncid, "veg_class", veg_lib->n_classes, &veg_class_dimid),
             "Cannot define dimension: veg_class\n");

    veg_descr_len = 0;
    for (i = 0; i < veg_lib->n_classes; i++) {
        int len = strlen(veg_lib->classes[i]->comment);

        if (len > veg_descr_len)
            veg_descr_len = len;
    }
    if (veg_descr_len) {
        char buf[BUF_SIZE];

        sprintf(buf, "string%d", veg_descr_len);
        nc_check(nc_def_dim(ncid, buf, veg_descr_len, &string_dimid),
                 "Cannot define dimension: string%d\n", veg_descr_len);
    }

    if (gp->root_zones > nints)
        nints = gp->root_zones;
    nc_check(nc_def_dim(ncid, "root_zone", gp->root_zones, &root_zone_dimid),
             "Cannot define dimension: root_zone\n");

    if (gp->snow_band->bands > nints)
        nints = gp->snow_band->bands;
    nc_check(nc_def_dim
             (ncid, "snow_band", gp->snow_band->bands, &snow_band_dimid),
             "Cannot define dimension: snow_band\n");

    nc_check(nc_def_dim(ncid, "month", 12, &month_dimid),
             "Cannot define dimension: month\n");

    if (gp->nlayer > nints)
        nints = gp->nlayer;
    nc_check(nc_def_dim(ncid, "nlayer", gp->nlayer, &nlayer_dimid),
             "Cannot define dimension: nlayer\n");

    nc_check(nc_def_dim(ncid, "lat", soil->domain->lat->n, &lat_dimid),
             "Cannot define dimension: lat\n");
    nc_check(nc_def_dim(ncid, "lon", soil->domain->lon->n, &lon_dimid),
             "Cannot define dimension: lon\n");

    /* variables */
    /* dimension variables */
    nc_check(nc_def_var
             (ncid, "veg_class", NC_INT, 1, &veg_class_dimid,
              &veg_class_varid), "Cannot define variable: veg_class\n");
    nc_check(nc_def_var
             (ncid, "root_zone", NC_INT, 1, &root_zone_dimid,
              &root_zone_varid), "Cannot define variable: root_zone\n");
    nc_check(nc_def_var
             (ncid, "snow_band", NC_INT, 1, &snow_band_dimid,
              &snow_band_varid), "Cannot define variable: snow_band\n");

    nc_check(nc_def_var(ncid, "month", NC_INT, 1, &month_dimid, &month_varid),
             "Cannot define variable: month\n");
    nc_check(nc_def_var
             (ncid, "layer", NC_INT, 1, &nlayer_dimid, &layer_varid),
             "Cannot define variable: layer\n");
    nc_check(nc_def_var(ncid, "lat", NC_DOUBLE, 1, &lat_dimid, &lat_varid),
             "Cannot define variable: lat\n");
    nc_check(nc_def_var(ncid, "lon", NC_DOUBLE, 1, &lon_dimid, &lon_varid),
             "Cannot define variable: lon\n");

    /* location variables */
    d = 0;
    dimids[d++] = nlayer_dimid;
    dimids[d++] = lat_dimid;
    dimids[d++] = lon_dimid;
    nc_check(nc_def_var
             (ncid, "cellnum", NC_INT, d - 1, dimids + 1, &cellnum_varid),
             "Cannot define variable: cellnum\n");
    nc_check(nc_def_var(ncid, "mask", NC_INT, d - 1, dimids + 1, &mask_varid),
             "Cannot define variable: mask\n");

    /* soil variables */
    nc_check(nc_def_var
             (ncid, "run_cell", NC_INT, d - 1, dimids + 1, &run_cell_varid),
             "Cannot define variable: run_cell\n");
    nc_check(nc_def_var
             (ncid, "gridcell", NC_INT, d - 1, dimids + 1, &gridcell_varid),
             "Cannot define variable: gridcell\n");
    nc_check(nc_def_var(ncid, "lats", NC_DOUBLE, d - 1, dimids + 1, &lats_varid),
             "Cannot define variable: lats\n");
    nc_check(nc_def_var(ncid, "lons", NC_DOUBLE, d - 1, dimids + 1, &lons_varid),
             "Cannot define variable: lons\n");
    nc_check(nc_def_var
             (ncid, "infilt", NC_DOUBLE, d - 1, dimids + 1, &infilt_varid),
             "Cannot define variable: infilt\n");
    nc_check(nc_def_var(ncid, "Ds", NC_DOUBLE, d - 1, dimids + 1, &Ds_varid),
             "Cannot define variable: Ds\n");
    nc_check(nc_def_var
             (ncid, "Dsmax", NC_DOUBLE, d - 1, dimids + 1, &Dsmax_varid),
             "Cannot define variable: Dsmax\n");
    nc_check(nc_def_var(ncid, "Ws", NC_DOUBLE, d - 1, dimids + 1, &Ws_varid),
             "Cannot define variable: Ws\n");
    nc_check(nc_def_var(ncid, "c", NC_DOUBLE, d - 1, dimids + 1, &c_varid),
             "Cannot define variable: c\n");

    nc_check(nc_def_var(ncid, "expt", NC_DOUBLE, d, dimids, &expt_varid),
             "Cannot define variable: expt\n");
    nc_check(nc_def_var(ncid, "Ksat", NC_DOUBLE, d, dimids, &Ksat_varid),
             "Cannot define variable: Ksat\n");
    nc_check(nc_def_var(ncid, "phi_s", NC_DOUBLE, d, dimids, &phi_s_varid),
             "Cannot define variable: phi_s\n");
    nc_check(nc_def_var
             (ncid, "init_moist", NC_DOUBLE, d, dimids, &init_moist_varid),
             "Cannot define variable: init_moist\n");

    nc_check(nc_def_var(ncid, "elev", NC_DOUBLE, d - 1, dimids + 1, &elev_varid),
             "Cannot define variable: elev\n");

    nc_check(nc_def_var(ncid, "depth", NC_DOUBLE, d, dimids, &depth_varid),
             "Cannot define variable: depth\n");

    nc_check(nc_def_var
             (ncid, "avg_T", NC_DOUBLE, d - 1, dimids + 1, &avg_T_varid),
             "Cannot define variable: avg_T\n");
    nc_check(nc_def_var(ncid, "dp", NC_DOUBLE, d - 1, dimids + 1, &dp_varid),
             "Cannot define variable: dp\n");

    nc_check(nc_def_var(ncid, "bubble", NC_DOUBLE, d, dimids, &bubble_varid),
             "Cannot define variable: bubble\n");
    nc_check(nc_def_var(ncid, "quartz", NC_DOUBLE, d, dimids, &quartz_varid),
             "Cannot define variable: quartz\n");
    nc_check(nc_def_var
             (ncid, "bulk_density", NC_DOUBLE, d, dimids,
              &bulk_density_varid), "Cannot define variable: bulk_density\n");
    nc_check(nc_def_var
             (ncid, "soil_density", NC_DOUBLE, d, dimids,
              &soil_density_varid), "Cannot define variable: soil_density\n");

    if (gp->organic_fract) {
        nc_check(nc_def_var
                 (ncid, "organic", NC_DOUBLE, d, dimids, &organic_varid),
                 "Cannot define variable: organic\n");
        nc_check(nc_def_var
                 (ncid, "bulk_dens_org", NC_DOUBLE, d, dimids,
                  &bulk_dens_org_varid),
                 "Cannot define variable: bulk_dens_org\n");
        nc_check(nc_def_var
                 (ncid, "soil_dens_org", NC_DOUBLE, d, dimids,
                  &soil_dens_org_varid),
                 "Cannot define variable: soil_dens_org\n");
    }

    nc_check(nc_def_var
             (ncid, "off_gmt", NC_DOUBLE, d - 1, dimids + 1, &off_gmt_varid),
             "Cannot define variable: off_gmt\n");

    nc_check(nc_def_var
             (ncid, "Wcr_FRACT", NC_DOUBLE, d, dimids, &Wcr_FRACT_varid),
             "Cannot define variable: Wcr_FRACT\n");
    nc_check(nc_def_var
             (ncid, "Wpwp_FRACT", NC_DOUBLE, d, dimids, &Wpwp_FRACT_varid),
             "Cannot define variable: Wpwp_FRACT\n");

    nc_check(nc_def_var
             (ncid, "rough", NC_DOUBLE, d - 1, dimids + 1, &rough_varid),
             "Cannot define variable: rough\n");
    nc_check(nc_def_var
             (ncid, "snow_rough", NC_DOUBLE, d - 1, dimids + 1,
              &snow_rough_varid), "Cannot define variable: snow_rough\n");
    nc_check(nc_def_var
             (ncid, "annual_prec", NC_DOUBLE, d - 1, dimids + 1,
              &annual_prec_varid), "Cannot define variable: annual_prec\n");

    nc_check(nc_def_var
             (ncid, "resid_moist", NC_DOUBLE, d, dimids, &resid_moist_varid),
             "Cannot define variable: resid_moist\n");

    nc_check(nc_def_var
             (ncid, "fs_active", NC_INT, d - 1, dimids + 1, &fs_active_varid),
             "Cannot define variable: fs_active\n");

    if (gp->spatial_frost) {
        nc_check(nc_def_var
                 (ncid, "frost_slope", NC_DOUBLE, d - 1, dimids + 1,
                  &frost_slope_varid),
                 "Cannot define variable: frost_slope\n");
        nc_check(nc_def_var
                 (ncid, "max_snow_distrib_slope", NC_DOUBLE, d - 1, dimids + 1,
                  &max_snow_distrib_slope_varid),
                 "Cannot define variable: max_snow_distrib_slope\n");
    }

    if (gp->july_tavg_supplied)
        nc_check(nc_def_var
                 (ncid, "July_Tavg", NC_DOUBLE, d - 1, dimids + 1,
                  &July_Tavg_varid), "Cannot define variable: July_Tavg\n");

    /* vegetation variables */
    nc_check(nc_def_var(ncid, "Nveg", NC_INT, d - 1, dimids + 1, &Nveg_varid),
             "Cannot define variable: Nveg\n");

    d = 0;
    dimids[d++] = veg_class_dimid;
    dimids[d++] = lat_dimid;
    dimids[d++] = lon_dimid;
    nc_check(nc_def_var(ncid, "Cv", NC_DOUBLE, d, dimids, &Cv_varid), "Cannot define variable: Cv\n");

    d = 0;
    dimids[d++] = veg_class_dimid;
    dimids[d++] = root_zone_dimid;
    dimids[d++] = lat_dimid;
    dimids[d++] = lon_dimid;
    nc_check(nc_def_var(ncid, "root_depth", NC_DOUBLE, d, dimids, &root_depth_varid), "Cannot define variable: root_depth\n");
    nc_check(nc_def_var(ncid, "root_fract", NC_DOUBLE, d, dimids, &root_fract_varid), "Cannot define variable: root_fract\n");

    d = 0;
    dimids[d++] = veg_class_dimid;
    dimids[d++] = lat_dimid;
    dimids[d++] = lon_dimid;
    nc_check(nc_def_var(ncid, "overstory", NC_INT, d, dimids, &overstory_varid), "Cannot define variable: overstory\n");
    nc_check(nc_def_var(ncid, "rarc", NC_DOUBLE, d, dimids, &rarc_varid), "Cannot define variable: rarc\n");
    nc_check(nc_def_var(ncid, "rmin", NC_DOUBLE, d, dimids, &rmin_varid), "Cannot define variable: rmin\n");

    d = 0;
    dimids[d++] = veg_class_dimid;
    dimids[d++] = string_dimid;
    nc_check(nc_def_var
             (ncid, "veg_descr", NC_CHAR, d, dimids, &veg_descr_varid),
             "Cannot define variable: veg_descr\n");

    nc_check(nc_enddef(ncid), "Cannot end definition\n");

    /* populate variables */
    ints = malloc(sizeof *ints * nints);
    for (i = 0; i < nints; i++)
        ints[i] = i + 1;

    /* dimension variables */
    nc_check(nc_put_var(ncid, month_varid, ints),
             "Cannot put variable: month\n");
    nc_check(nc_put_var(ncid, layer_varid, ints),
             "Cannot put variable: layer\n");
    nc_check(nc_put_var(ncid, lat_varid, soil->domain->lat->values),
             "Cannot put variable: lat\n");
    nc_check(nc_put_var(ncid, lon_varid, soil->domain->lon->values),
             "Cannot put variable: lon\n");
    nc_check(nc_put_var(ncid, snow_band_varid, ints),
             "Cannot put variable: snow_band\n");
    nc_check(nc_put_var(ncid, root_zone_varid, ints),
             "Cannot put variable: root_zone\n");
    nc_check(nc_put_var(ncid, veg_class_varid, ints),
             "Cannot put variable: veg_class\n");

    free(ints);

    /* vegetation variables */
    start[1] = 0;
    count[0] = 1;
    for (i = 0; i < veg_lib->n_classes; i++) {
        start[0] = i;
        count[1] = strlen(veg_lib->classes[i]->comment);
        nc_check(nc_put_vara
                 (ncid, veg_descr_varid, start, count,
                  veg_lib->classes[i]->comment),
                 "Cannot put variable: veg_descr\n");
    }

    for (i = 0; i < soil->n_cells; i++) {
        int run_cell = soil->cells[i]->run_cell, fs_active =
            soil->cells[i]->fs_active;
        int lat_idx, lon_idx, mask_idx, vp_idx;
	int j;

	d = 0;
        start[d++] = 0;
        start[d++] = lat_idx =
            find_double(soil->domain->lat, soil->cells[i]->lat);
        start[d++] = lon_idx =
            find_double(soil->domain->lon, soil->cells[i]->lon);
	d = 0;
        count[d++] = gp->nlayer;
        count[d++] = 1;
        count[d++] = 1;

        mask_idx = lat_idx * soil->domain->lon->n + lon_idx;

        /* location variables */
        nc_check(nc_put_var1
                 (ncid, cellnum_varid, start + 1, &soil->cells[i]->gridcel),
                 "Cannot put variable: cellnum\n");
        nc_check(nc_put_var1
                 (ncid, mask_varid, start + 1, &soil->domain->mask[mask_idx]),
                 "Cannot put variable: mask\n");

        /* soil variables */
        nc_check(nc_put_var1(ncid, run_cell_varid, start + 1, &run_cell),
                 "Cannot put variable: run_cell\n");
        nc_check(nc_put_var1
                 (ncid, gridcell_varid, start + 1, &soil->cells[i]->gridcel),
                 "Cannot put variable: gridcell\n");
        nc_check(nc_put_var1
                 (ncid, lats_varid, start + 1, &soil->cells[i]->lat),
                 "Cannot put variable: lats\n");
        nc_check(nc_put_var1
                 (ncid, lons_varid, start + 1, &soil->cells[i]->lon),
                 "Cannot put variable: lons\n");
        nc_check(nc_put_var1
                 (ncid, infilt_varid, start + 1, &soil->cells[i]->infilt),
                 "Cannot put variable: infilt\n");
        nc_check(nc_put_var1(ncid, Ds_varid, start + 1, &soil->cells[i]->Ds),
                 "Cannot put variable: Ds\n");
        nc_check(nc_put_var1
                 (ncid, Dsmax_varid, start + 1, &soil->cells[i]->Dsmax),
                 "Cannot put variable: Dsmax\n");
        nc_check(nc_put_var1(ncid, Ws_varid, start + 1, &soil->cells[i]->Ws),
                 "Cannot put variable: Ws\n");
        nc_check(nc_put_var1(ncid, c_varid, start + 1, &soil->cells[i]->c),
                 "Cannot put variable: c\n");

        nc_check(nc_put_vara
                 (ncid, expt_varid, start, count, soil->cells[i]->expt),
                 "Cannot put variable: expt\n");
        nc_check(nc_put_vara
                 (ncid, Ksat_varid, start, count, soil->cells[i]->Ksat),
                 "Cannot put variable: Ksat\n");
        nc_check(nc_put_vara
                 (ncid, phi_s_varid, start, count, soil->cells[i]->phi_s),
                 "Cannot put variable: phi_s\n");
        nc_check(nc_put_vara
                 (ncid, init_moist_varid, start, count,
                  soil->cells[i]->init_moist),
                 "Cannot put variable: init_moist\n");

        nc_check(nc_put_var1
                 (ncid, elev_varid, start + 1, &soil->cells[i]->elev),
                 "Cannot put variable: elev\n");

        nc_check(nc_put_vara
                 (ncid, depth_varid, start, count, soil->cells[i]->depth),
                 "Cannot put variable: depth\n");

        nc_check(nc_put_var1
                 (ncid, avg_T_varid, start + 1, &soil->cells[i]->avg_T),
                 "Cannot put variable: avg_T\n");
        nc_check(nc_put_var1(ncid, dp_varid, start + 1, &soil->cells[i]->dp),
                 "Cannot put variable: dp\n");

        nc_check(nc_put_vara
                 (ncid, bubble_varid, start, count, soil->cells[i]->bubble),
                 "Cannot put variable: bubble\n");
        nc_check(nc_put_vara
                 (ncid, quartz_varid, start, count, soil->cells[i]->quartz),
                 "Cannot put variable: quartz\n");
        nc_check(nc_put_vara
                 (ncid, bulk_density_varid, start, count,
                  soil->cells[i]->bulk_density),
                 "Cannot put variable: bulk_density\n");
        nc_check(nc_put_vara
                 (ncid, soil_density_varid, start, count,
                  soil->cells[i]->soil_density),
                 "Cannot put variable: soil_density\n");

        if (gp->organic_fract) {
            nc_check(nc_put_vara
                     (ncid, organic_varid, start, count,
                      soil->cells[i]->organic),
                     "Cannot put variable: organic\n");
            nc_check(nc_put_vara
                     (ncid, bulk_dens_org_varid, start, count,
                      soil->cells[i]->bulk_dens_org),
                     "Cannot put variable: bulk_dens_org\n");
            nc_check(nc_put_vara
                     (ncid, soil_dens_org_varid, start, count,
                      soil->cells[i]->soil_dens_org),
                     "Cannot put variable: soil_dens_org\n");
        }

        nc_check(nc_put_var1
                 (ncid, off_gmt_varid, start + 1, &soil->cells[i]->off_gmt),
                 "Cannot put variable: off_gmt\n");

        nc_check(nc_put_vara
                 (ncid, Wcr_FRACT_varid, start, count,
                  soil->cells[i]->Wcr_FRACT),
                 "Cannot put variable: Wcr_FRACT\n");
        nc_check(nc_put_vara
                 (ncid, Wpwp_FRACT_varid, start, count,
                  soil->cells[i]->Wpwp_FRACT),
                 "Cannot put variable: Wpwp_FRACT\n");

        nc_check(nc_put_var1
                 (ncid, rough_varid, start + 1, &soil->cells[i]->rough),
                 "Cannot put variable: rough\n");
        nc_check(nc_put_var1
                 (ncid, snow_rough_varid, start + 1,
                  &soil->cells[i]->snow_rough),
                 "Cannot put variable: snow_rough\n");
        nc_check(nc_put_var1
                 (ncid, annual_prec_varid, start + 1,
                  &soil->cells[i]->annual_prec),
                 "Cannot put variable: annual_prec\n");

        nc_check(nc_put_vara
                 (ncid, resid_moist_varid, start, count,
                  soil->cells[i]->resid_moist),
                 "Cannot put variable: resid_moist\n");

        nc_check(nc_put_var1(ncid, fs_active_varid, start + 1, &fs_active),
                 "Cannot put variable: fs_active\n");

        if (gp->spatial_frost) {
            nc_check(nc_put_var1
                     (ncid, frost_slope_varid, start + 1,
                      &soil->cells[i]->frost_slope),
                     "Cannot put variable: frost_slope\n");
            nc_check(nc_put_var1
                     (ncid, max_snow_distrib_slope_varid, start + 1,
                      &soil->cells[i]->max_snow_distrib_slope),
                     "Cannot put variable: max_snow_distrib_slope\n");
        }

        if (gp->july_tavg_supplied)
            nc_check(nc_put_var1
                     (ncid, July_Tavg_varid, start + 1,
                      &soil->cells[i]->July_Tavg),
                     "Cannot put variable: July_Tavg\n");

	/* vegetation variables */
	for(vp_idx = 0; vp_idx < veg_params->n_cells && veg_params->cells[vp_idx]->gridcel != soil->cells[i]->gridcel; vp_idx++);
	if(vp_idx == veg_params->n_cells)
	    error("Cannot find vegetation parameters for grid cell %d\n", soil->cells[i]->gridcel);

        nc_check(nc_put_var1(ncid, Nveg_varid, start + 1, &veg_params->cells[vp_idx]->Nveg), "Cannot put variable: Nveg\n");

	d = 0;
        start[d++] = 0;
        start[d++] = lat_idx;
        start[d++] = lon_idx;
	d = 0;
        count[d++] = veg_params->cells[vp_idx]->Nveg;
        count[d++] = 1;
        count[d++] = 1;
        nc_check(nc_put_vara(ncid, Cv_varid, start, count, veg_params->cells[vp_idx]->Cv), "Cannot put variable: Cv\n");

	d = 1;
	start[d++] = 0;
	start[d++] = lat_idx;
	start[d++] = lon_idx;
	d = 0;
	count[d++] = 1;
	count[d++] = veg_params->root_zones;
	count[d++] = 1;
	count[d++] = 1;

	for(j = 0 ; j < veg_params->cells[vp_idx]->Nveg; j++){
	    start[0] = j;
	    nc_check(nc_put_vara(ncid, root_depth_varid, start, count, veg_params->cells[vp_idx]->root_depth[j]), "Cannot put variable: root_depth\n");
	    nc_check(nc_put_vara(ncid, root_fract_varid, start, count, veg_params->cells[vp_idx]->root_fract[j]), "Cannot put variable: root_fract\n");
	}
    }

#if 0
    for (i = 0; i < veg_lib->n_classes; i++) {
	int overstory = veg_lib->classes[i]->overstory;

        nc_check(nc_put_var1(ncid, overstory_varid, start + 1, &soil->cells[i]->gridcel),
                 "Cannot put variable: cellnum\n");

    }
#endif

    nc_check(nc_close(ncid), "Cannot close file: %s\n", gp->parameters);
}
