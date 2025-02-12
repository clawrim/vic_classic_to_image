#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include "vic.h"

#define DOMAIN "domain.nc"
#define PARAMETERS "params.nc"

static int read_int(const char *);
static float read_float(const char *);
static bool read_bool(const char *);
static char *read_string(const char *);
static enum calendar read_calendar(const char *);
static enum time_units read_time_units(const char *);
static enum grnd_flux_type read_grnd_flux_type(const char *);
static enum snow_density read_snow_density(const char *);
static enum aero_resist_cansnow read_aero_resist_cansnow(const char *);
static enum rc_mode read_rc_mode(const char *);
static enum file_format read_file_format(const char *);
static enum endian read_endian(const char *);
static struct force_type_s *read_force_type(const char *);
static struct domain_type_s *read_domain_type(const char *);
static enum baseflow read_baseflow(const char *);
static enum veg_src read_veg_src(const char *);
static struct snow_band_s *read_snow_band(const char *);
static struct freq_s *read_freq(const char *);
static int read_compress(const char *);
static struct outvar_s *read_outvar(const char *);

struct global_params_s *read_global_params(char *global_path)
{
    struct global_params_s *gp;
    FILE *fp;
    char buf[BUF_SIZE];
    int forcing = -1;

    if (!(fp = fopen(global_path, "r")))
        error("Cannot open file: %s\n", global_path);

    gp = calloc(1, sizeof *gp);

    while (!feof(fp) && fgets(buf, BUF_SIZE, fp)) {
        char key[BUF_SIZE], *p;

        if (buf[0] == '#' || buf[0] == '\r' || buf[0] == '\n' || !buf[0])
            continue;

        if (sscanf(buf, "%s", key) != 1 || key[0] == '#' || !key[0])
            continue;

        if ((p = strchr(buf, '\r')))
            *p = 0;
        if ((p = strchr(buf, '\n')))
            *p = 0;

        if (strcasecmp(key, "NLAYER") == 0)
            gp->nlayer = read_int(buf);
        else if (strcasecmp(key, "NODES") == 0)
            gp->nodes = read_int(buf);
        else if (strcasecmp(key, "MODEL_STEPS_PER_DAY") == 0)
            gp->model_steps_per_day = read_int(buf);
        else if (strcasecmp(key, "SNOW_STEPS_PER_DAY") == 0)
            gp->snow_steps_per_day = read_int(buf);
        else if (strcasecmp(key, "RUNOFF_STEPS_PER_DAY") == 0)
            gp->runoff_steps_per_day = read_int(buf);
        else if (strcasecmp(key, "STARTYEAR") == 0)
            gp->startyear = read_int(buf);
        else if (strcasecmp(key, "STARTMONTH") == 0)
            gp->startmonth = read_int(buf);
        else if (strcasecmp(key, "STARTDAY") == 0)
            gp->startday = read_int(buf);
        else if (strcasecmp(key, "STARTSEC") == 0)
            gp->startsec = read_int(buf);
        else if (strcasecmp(key, "NRECS") == 0)
            gp->nrecs = read_int(buf);
        else if (strcasecmp(key, "ENDYEAR") == 0)
            gp->endyear = read_int(buf);
        else if (strcasecmp(key, "ENDMONTH") == 0)
            gp->endmonth = read_int(buf);
        else if (strcasecmp(key, "ENDDAY") == 0)
            gp->endday = read_int(buf);
        else if (strcasecmp(key, "CALENDAR") == 0)
            gp->calendar = read_calendar(buf);
        else if (strcasecmp(key, "OUT_TIME_UNITS") == 0)
            gp->out_time_units = read_time_units(buf);
        else if (strcasecmp(key, "FULL_ENERGY") == 0)
            gp->full_energy = read_bool(buf);
        else if (strcasecmp(key, "CLOSE_ENERGY") == 0)
            gp->close_energy = read_bool(buf);
        else if (strcasecmp(key, "FROZEN_SOIL") == 0)
            gp->frozen_soil = read_bool(buf);
        else if (strcasecmp(key, "QUICK_FLUX") == 0)
            gp->quick_flux = read_bool(buf);
        else if (strcasecmp(key, "IMPLICIT") == 0)
            gp->implicit = read_bool(buf);
        else if (strcasecmp(key, "QUICK_SOLVE") == 0)
            gp->quick_solve = read_bool(buf);
        else if (strcasecmp(key, "NOFLUX") == 0)
            gp->noflux = read_bool(buf);
        else if (strcasecmp(key, "EXP_TRANS") == 0)
            gp->exp_trans = read_bool(buf);
        else if (strcasecmp(key, "GRND_FLUX_TYPE") == 0)
            gp->grnd_flux_type = read_grnd_flux_type(buf);
        else if (strcasecmp(key, "TFALLBACK") == 0)
            gp->tfallback = read_bool(buf);
        else if (strcasecmp(key, "SHARE_LAYER_MOIST") == 0)
            gp->share_layer_moist = read_bool(buf);
        else if (strcasecmp(key, "SPATIAL_FROST") == 0)
            gp->spatial_frost = read_bool(buf);
        else if (strcasecmp(key, "SNOW_DENSITY") == 0)
            gp->snow_density = read_snow_density(buf);
        else if (strcasecmp(key, "BLOWING") == 0)
            gp->blowing = read_bool(buf);
        else if (strcasecmp(key, "BLOWING_VAR_THRESHOLD") == 0)
            gp->blowing_var_threshold = read_bool(buf);
        else if (strcasecmp(key, "BLOWING_CALC_PROB") == 0)
            gp->blowing_calc_prob = read_bool(buf);
        else if (strcasecmp(key, "BLOWING_SIMPLE") == 0)
            gp->blowing_simple = read_bool(buf);
        else if (strcasecmp(key, "BLOWING_FETCH") == 0)
            gp->blowing_fetch = read_bool(buf);
        else if (strcasecmp(key, "BLOWING_SPATIAL_WIND") == 0)
            gp->blowing_spatial_wind = read_bool(buf);
        else if (strcasecmp(key, "COMPUTE_TREELINE") == 0)
            gp->compute_treeline = read_int(buf);
        else if (strcasecmp(key, "CORRPREC") == 0)
            gp->corrprec = read_bool(buf);
        else if (strcasecmp(key, "SPATIAL_SNOW") == 0)
            gp->spatial_snow = read_bool(buf);
        else if (strcasecmp(key, "MIN_WIND_SPEED") == 0)
            gp->min_wind_speed = read_float(buf);
        else if (strcasecmp(key, "AERO_RESIST_CANSNOW") == 0)
            gp->aero_resist_cansnow = read_aero_resist_cansnow(buf);
        else if (strcasecmp(key, "CARBON") == 0)
            gp->carbon = read_bool(buf);
        else if (strcasecmp(key, "RC_MODE") == 0)
            gp->rc_mode = read_rc_mode(buf);
        else if (strcasecmp(key, "VEGLIB_PHOTO") == 0)
            gp->veglib_photo = read_bool(buf);
        else if (strcasecmp(key, "CONTINUEONERROR") == 0)
            gp->continueonerror = read_bool(buf);
        else if (strcasecmp(key, "INIT_STATE") == 0)
            gp->init_state = read_string(buf);
        else if (strcasecmp(key, "STATENAME") == 0)
            gp->statename = read_string(buf);
        else if (strcasecmp(key, "STATEYEAR") == 0)
            gp->stateyear = read_int(buf);
        else if (strcasecmp(key, "STATEMONTH") == 0)
            gp->statemonth = read_int(buf);
        else if (strcasecmp(key, "STATEDAY") == 0)
            gp->stateday = read_int(buf);
        else if (strcasecmp(key, "STATESEC") == 0)
            gp->statesec = read_int(buf);
        else if (strcasecmp(key, "STATE_FORMAT") == 0)
            gp->state_format = read_file_format(buf);
        else if (strcasecmp(key, "FORCING1") == 0) {
            gp->forcing1 = read_string(buf);
            forcing = 0;
        }
        else if (strcasecmp(key, "FORCING2") == 0) {
            gp->forcing2 = read_string(buf);
            forcing = 1;
        }
        else if (strcasecmp(key, "FORCE_FORMAT") == 0)
            gp->force_format = read_file_format(buf);
        else if (strcasecmp(key, "FORCE_ENDIAN") == 0)
            gp->force_endian = read_endian(buf);
        else if (strcasecmp(key, "N_TYPES") == 0)
            /* XXX: unused in the VIC code; instead, FORCE_TYPEs are counted */
            ;
        else if (strcasecmp(key, "FORCE_TYPE") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->n_types)
                gp->n_types = calloc(2, sizeof *gp->n_types);

            gp->n_types[forcing]++;

            if (!gp->force_type)
                gp->force_type = calloc(2, sizeof *gp->force_type);
            gp->force_type[forcing] =
                realloc(gp->force_type[forcing],
                        gp->n_types[forcing] * sizeof **gp->force_type);
            gp->force_type[forcing][gp->n_types[forcing] - 1] =
                read_force_type(buf);
        }
        else if (strcasecmp(key, "FORCE_STEPS_PER_DAY") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->force_steps_per_day)
                gp->force_steps_per_day =
                    calloc(2, sizeof *gp->force_steps_per_day);
            gp->force_steps_per_day[forcing] = read_int(buf);
        }
        else if (strcasecmp(key, "FORCEYEAR") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->forceyear)
                gp->forceyear = calloc(2, sizeof *gp->forceyear);
            gp->forceyear[forcing] = read_int(buf);
        }
        else if (strcasecmp(key, "FORCEMONTH") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->forcemonth)
                gp->forcemonth = calloc(2, sizeof *gp->forcemonth);
            gp->forcemonth[forcing] = read_int(buf);
        }
        else if (strcasecmp(key, "FORCEDAY") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->forceday)
                gp->forceday = calloc(2, sizeof *gp->forceday);
            gp->forceday[forcing] = read_int(buf);
        }
        else if (strcasecmp(key, "FORCESEC") == 0) {
            if (forcing < 0 || forcing > 1)
                continue;

            if (!gp->forcesec)
                gp->forcesec = calloc(2, sizeof *gp->forcesec);
            gp->forcesec[forcing] = read_int(buf);
        }
        else if (strcasecmp(key, "GRID_DECIMAL") == 0)
            gp->grid_decimal = read_int(buf);
        else if (strcasecmp(key, "WIND_H") == 0)
            gp->wind_h = read_float(buf);
        else if (strcasecmp(key, "CANOPY_LAYERS") == 0)
            gp->canopy_layers = read_int(buf);
        else if (strcasecmp(key, "DOMAIN") == 0)
            gp->domain = read_string(buf);
        else if (strcasecmp(key, "DOMAIN_TYPE") == 0) {
            int domain_type = gp->n_domain_types++;

            gp->domain_type =
                realloc(gp->domain_type,
                        gp->n_domain_types * sizeof *gp->domain_type);
            gp->domain_type[domain_type] = read_domain_type(buf);
        }
        else if (strcasecmp(key, "SOIL") == 0)
            gp->soil = read_string(buf);
        else if (strcasecmp(key, "PARAMETERS") == 0)
            gp->parameters = read_string(buf);
        else if (strcasecmp(key, "BASEFLOW") == 0)
            gp->baseflow = read_baseflow(buf);
        else if (strcasecmp(key, "JULY_TAVG_SUPPLIED") == 0)
            gp->july_tavg_supplied = read_bool(buf);
        else if (strcasecmp(key, "ORGANIC_FRACT") == 0)
            gp->organic_fract = read_bool(buf);
        else if (strcasecmp(key, "VEGLIB") == 0)
            gp->veglib = read_string(buf);
        else if (strcasecmp(key, "VEGPARAM") == 0)
            gp->vegparam = read_string(buf);
        else if (strcasecmp(key, "ROOT_ZONES") == 0)
            gp->root_zones = read_int(buf);
        else if (strcasecmp(key, "VEGPARAM_ALB") == 0)
            gp->vegparam_alb = read_bool(buf);
        else if (strcasecmp(key, "ALB_SRC") == 0)
            gp->alb_src = read_veg_src(buf);
        else if (strcasecmp(key, "VEGPARAM_LAI") == 0)
            gp->vegparam_lai = read_bool(buf);
        else if (strcasecmp(key, "LAI_SRC") == 0)
            gp->lai_src = read_veg_src(buf);
        else if (strcasecmp(key, "VEGLIB_FCAN") == 0)
            gp->veglib_fcan = read_bool(buf);
        else if (strcasecmp(key, "VEGPARAM_FCAN") == 0)
            gp->vegparam_fcan = read_bool(buf);
        else if (strcasecmp(key, "FCAN_SRC") == 0)
            gp->fcan_src = read_veg_src(buf);
        else if (strcasecmp(key, "SNOW_BAND") == 0)
            gp->snow_band = read_snow_band(buf);
        else if (strcasecmp(key, "CONSTANTS") == 0)
            gp->constants = read_string(buf);
        else if (strcasecmp(key, "LAKES") == 0)
            gp->lakes = read_string(buf);
        else if (strcasecmp(key, "LAKE_PROFILE") == 0)
            gp->lake_profile = read_bool(buf);
        else if (strcasecmp(key, "EQUAL_AREA") == 0)
            gp->equal_area = read_bool(buf);
        else if (strcasecmp(key, "RESOLUTION") == 0)
            gp->resolution = read_float(buf);
        else if (strcasecmp(key, "LAKE_NODES") == 0)
            gp->lake_nodes = read_int(buf);
        else if (strcasecmp(key, "LOG_DIR") == 0)
            gp->log_dir = read_string(buf);
        else if (strcasecmp(key, "RESULT_DIR") == 0)
            gp->result_dir = read_string(buf);
        else if (strcasecmp(key, "OUTFILE") == 0) {
            int outfile = gp->n_outfiles++;

            gp->outfile =
                realloc(gp->outfile, gp->n_outfiles * sizeof *gp->outfile);
            gp->outfile[outfile] = read_string(buf);
            gp->aggfreq =
                realloc(gp->aggfreq, gp->n_outfiles * sizeof *gp->aggfreq);
            gp->histfreq =
                realloc(gp->histfreq, gp->n_outfiles * sizeof *gp->histfreq);
            gp->compress =
                realloc(gp->compress, gp->n_outfiles * sizeof *gp->compress);
            gp->out_format =
                realloc(gp->out_format,
                        gp->n_outfiles * sizeof *gp->out_format);
            gp->n_outvars =
                realloc(gp->n_outvars,
                        gp->n_outfiles * sizeof *gp->n_outvars);
            gp->outvar =
                realloc(gp->outvar, gp->n_outfiles * sizeof *gp->outvar);
        }
        else if (strcasecmp(key, "AGGFREQ") == 0) {
            int outfile = gp->n_outfiles - 1;

            if (outfile < 0)
                continue;

            gp->aggfreq[outfile] = read_freq(buf);
        }
        else if (strcasecmp(key, "HISTFREQ") == 0) {
            int outfile = gp->n_outfiles - 1;

            if (outfile < 0)
                continue;

            gp->histfreq[outfile] = read_freq(buf);
        }
        else if (strcasecmp(key, "COMPRESS") == 0) {
            int outfile = gp->n_outfiles - 1;

            if (outfile < 0)
                continue;

            gp->compress[outfile] = read_compress(buf);
        }
        else if (strcasecmp(key, "OUT_FORMAT") == 0) {
            int outfile = gp->n_outfiles - 1;

            if (outfile < 0)
                continue;

            gp->out_format[outfile] = read_file_format(buf);
        }
        else if (strcasecmp(key, "OUTVAR") == 0) {
            int outfile = gp->n_outfiles - 1;

            if (outfile < 0)
                continue;

            gp->n_outvars[outfile]++;

            gp->outvar[outfile] =
                realloc(gp->outvar[outfile],
                        gp->n_outvars[outfile] * sizeof **gp->outvar);
            gp->outvar[outfile][gp->n_outvars[outfile] - 1] =
                read_outvar(buf);
        }
        else
            error("Invalid global parameter line: %s\n", buf);
    }

    fclose(fp);

    return gp;
}

void free_global_params(struct global_params_s *gp)
{
    int i, j;

    free(gp->init_state);
    free(gp->statename);

    free(gp->forcing1);
    free(gp->forcing2);
    for (i = 0; i < 2; i++) {
        for (j = 0; j < gp->n_types[i]; j++) {
            free(gp->force_type[i][j]->nc_name);
            free(gp->force_type[i][j]);
        }
        free(gp->force_type[i]);
    }

    free(gp->force_type);
    free(gp->n_types);
    free(gp->force_steps_per_day);
    free(gp->forceyear);
    free(gp->forcemonth);
    free(gp->forceday);
    free(gp->forcesec);

    free(gp->domain);
    for (i = 0; i < gp->n_domain_types; i++) {
        free(gp->domain_type[i]->nc_name);
        free(gp->domain_type[i]);
    }
    free(gp->domain_type);

    free(gp->soil);
    free(gp->parameters);
    free(gp->veglib);
    free(gp->vegparam);
    if (gp->snow_band) {
        free(gp->snow_band->file);
        free(gp->snow_band);
    }
    free(gp->constants);

    free(gp->lakes);

    free(gp->log_dir);
    free(gp->result_dir);
    for (i = 0; i < gp->n_outfiles; i++) {
        free(gp->outfile[i]);
        free(gp->aggfreq[i]);
        free(gp->histfreq[i]);
        for (j = 0; j < gp->n_outvars[i]; j++) {
            free(gp->outvar[i][j]->name);
            free(gp->outvar[i][j]->format);
            free(gp->outvar[i][j]);
        }
        free(gp->outvar[i]);
    }
    free(gp->outfile);
    free(gp->aggfreq);
    free(gp->histfreq);
    free(gp->compress);
    free(gp->out_format);
    free(gp->n_outvars);
    free(gp->outvar);

    free(gp);
}

int is_classic(struct global_params_s *gp)
{
    return gp->soil != NULL;
}

int is_image(struct global_params_s *gp)
{
    return gp->parameters != NULL;
}

void populate_image_global_params(struct global_params_s *gp,
                                  const char *image_prefix)
{
    int i, j;

    if (is_image(gp))
        error("Already image global parameters structure\n");

    for (i = 0; i < 2; i++) {
        for (j = 0; j < gp->n_types[i]; j++) {
            char *p = NULL;

            switch (gp->force_type[i][j]->force_type) {
            case AIR_TEMP:
                p = "air_temp";
                break;
            case ALBEDO:
                p = "albedo";
                break;
            case CATM:
                p = "catm";
                break;
            case CHANNEL_IN:
                p = "channel_in";
                break;
            case FCANOPY:
                p = "fcanopy";
                break;
            case FDIR:
                p = "fdir";
                break;
            case LAI:
                p = "lai";
                break;
            case LWDOWN:
                p = "lwdown";
                break;
            case PAR:
                p = "par";
                break;
            case PREC:
                p = "prec";
                break;
            case PRESSURE:
                p = "pressure";
                break;
            case VP:
                p = "vp";
                break;
            case SWDOWN:
                p = "swdown";
                break;
            case WIND:
                p = "wind";
                break;
            case SKIP:
                continue;
            }
            gp->force_type[i][j]->nc_name = malloc(strlen(p) + 1);
            strcpy(gp->force_type[i][j]->nc_name, p);
        }
    }

    if (image_prefix) {
        gp->domain = malloc(strlen(image_prefix) + strlen(DOMAIN) + 1);
        sprintf(gp->domain, "%s%s", image_prefix, DOMAIN);
    }
    else {
        gp->domain = malloc(strlen(DOMAIN) + 1);
        strcpy(gp->domain, DOMAIN);
    }

    /* TODO: LAT, LON, MASK, AREA, FRAC, YDIM, XDIM */
    gp->n_domain_types = FRAC + 1;
    gp->domain_type = realloc(gp->domain_type,
                              gp->n_domain_types * sizeof *gp->domain_type);

    for (i = 0; i < gp->n_domain_types; i++) {
        char *p = NULL;

        gp->domain_type[i] = malloc(sizeof *gp->domain_type[i]);
        switch ((gp->domain_type[i]->variable = i)) {
        case LAT:
            p = "lat";
            break;
        case LON:
            p = "lon";
            break;
        case MASK:
            p = "mask";
            break;
        case AREA:
            p = "area";
            break;
        case FRAC:
            p = "frac";
            break;
        case YDIM:
            p = "ydim";
            break;
        case XDIM:
            p = "xdim";
            break;
        }
        gp->domain_type[i]->nc_name = malloc(strlen(p) + 1);
        strcpy(gp->domain_type[i]->nc_name, p);
    }

    if (image_prefix) {
        gp->parameters =
            malloc(strlen(image_prefix) + strlen(PARAMETERS) + 1);
        sprintf(gp->parameters, "%s%s", image_prefix, PARAMETERS);
    }
    else {
        gp->parameters = malloc(strlen(PARAMETERS) + 1);
        strcpy(gp->parameters, PARAMETERS);
    }
}

static int read_int(const char *buf)
{
    int ret;

    if (sscanf(buf, "%*s %d", &ret) != 1)
        error("Invalid integer: %s\n", buf);

    return ret;
}

static float read_float(const char *buf)
{
    float ret;

    if (sscanf(buf, "%*s %f", &ret) != 1)
        error("Invalid float: %s\n", buf);

    return ret;
}

static bool read_bool(const char *buf)
{
    bool ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid bool: %s\n", buf);

    if (strcasecmp(str, "TRUE") == 0)
        ret = true;
    else if (strcasecmp(str, "FALSE") == 0)
        ret = false;
    else
        error("Invalid bool: %s\n", str);

    return ret;
}

static char *read_string(const char *buf)
{
    char *ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) == 1) {
        ret = malloc(strlen(str) + 1);
        strcpy(ret, str);
    }
    else
        error("Invalid string: %s\n", buf);

    return ret;
}

static enum calendar read_calendar(const char *buf)
{
    enum calendar ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid CALENDAR: %s\n", buf);

    if (strcasecmp(str, "STANDARD") == 0)
        ret = STANDARD;
    else if (strcasecmp(str, "GREGORIAN") == 0)
        ret = GREGORIAN;
    else if (strcasecmp(str, "PROLEPTIC_GREGORIAN") == 0)
        ret = PROLEPTIC_GREGORIAN;
    else if (strcasecmp(str, "NOLEAP") == 0)
        ret = NOLEAP;
    else if (strcasecmp(str, "365_DAY") == 0)
        ret = DAY_365;
    else if (strcasecmp(str, "360_DAY") == 0)
        ret = DAY_360;
    else if (strcasecmp(str, "JULIAN") == 0)
        ret = JULIAN;
    else if (strcasecmp(str, "ALL_LEAP") == 0)
        ret = ALL_LEAP;
    else if (strcasecmp(str, "366_DAY") == 0)
        ret = DAY_366;
    else
        error("Invalid CALENDAR: %s\n", str);

    return ret;
}

static enum time_units read_time_units(const char *buf)
{
    enum time_units ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid TIME_UNITS: %s\n", buf);

    if (strcasecmp(str, "SECONDS") == 0)
        ret = SECONDS;
    else if (strcasecmp(str, "MINUTES") == 0)
        ret = MINUTES;
    else if (strcasecmp(str, "HOURS") == 0)
        ret = HOURS;
    else if (strcasecmp(str, "DAYS") == 0)
        ret = DAYS;
    else
        error("Invalid TIME_UNITS: %s\n", str);

    return ret;
}

static enum grnd_flux_type read_grnd_flux_type(const char *buf)
{
    enum grnd_flux_type ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid GRND_FLUX_TYPE: %s\n", buf);

    if (strcasecmp(str, "GF_406") == 0)
        ret = GF_406;
    else if (strcasecmp(str, "GF_410") == 0)
        ret = GF_410;
    else
        error("Invalid GRND_FLUX_TYPE: %s\n", str);

    return ret;
}

static enum snow_density read_snow_density(const char *buf)
{
    enum snow_density ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid SNOW_DENSITY: %s\n", buf);

    if (strcasecmp(str, "DENS_BRAS") == 0)
        ret = DENS_BRAS;
    else if (strcasecmp(str, "DENS_SNTHRM") == 0)
        ret = DENS_SNTHRM;
    else
        error("Invalid SNOW_DENSITY: %s\n", str);

    return ret;
}

static enum aero_resist_cansnow read_aero_resist_cansnow(const char *buf)
{
    enum aero_resist_cansnow ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid AERO_RESIST_CANSNOW: %s\n", buf);

    if (strcasecmp(str, "AR_406") == 0)
        ret = AR_406;
    else if (strcasecmp(str, "AR_406_LS") == 0)
        ret = AR_406_LS;
    else if (strcasecmp(str, "AR_406_FULL") == 0)
        ret = AR_406_FULL;
    else if (strcasecmp(str, "AR_410") == 0)
        ret = AR_410;
    else
        error("Invalid AERO_RESIST_CANSNOW: %s\n", str);

    return ret;
}

static enum rc_mode read_rc_mode(const char *buf)
{
    enum rc_mode ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid RC_MODE: %s\n", buf);

    if (strcasecmp(str, "RC_JARVIS") == 0)
        ret = RC_JARVIS;
    else if (strcasecmp(str, "RC_PHOTO") == 0)
        ret = RC_PHOTO;
    else
        error("Invalid RC_MODE: %s\n", str);

    return ret;
}

static enum file_format read_file_format(const char *buf)
{
    enum file_format ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid FILE_FORMAT: %s\n", buf);

    if (strcasecmp(str, "BINARY") == 0)
        ret = BINARY;
    else if (strcasecmp(str, "ASCII") == 0)
        ret = ASCII;
    else if (strcasecmp(str, "NETCDF3_CLASSIC") == 0)
        ret = NETCDF3_CLASSIC;
    else if (strcasecmp(str, "NETCDF3_64BIT_OFFSET") == 0)
        ret = NETCDF3_64BIT_OFFSET;
    else if (strcasecmp(str, "NETCDF4_CLASSIC") == 0)
        ret = NETCDF4_CLASSIC;
    else if (strcasecmp(str, "NETCDF4") == 0)
        ret = NETCDF4;
    else
        error("Invalid FILE_FORMAT: %s\n", str);

    return ret;
}

static enum endian read_endian(const char *buf)
{
    enum endian ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid ENDIAN: %s\n", buf);

    if (strcasecmp(str, "BIG") == 0)
        ret = BIG;
    else if (strcasecmp(str, "LITTLE") == 0)
        ret = LITTLE;
    else
        error("Invalid ENDIAN: %s\n", str);

    return ret;
}

static struct force_type_s *read_force_type(const char *buf)
{
    struct force_type_s *ret = calloc(1, sizeof *ret);
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid FORCE_TYPE: %s\n", buf);

    if (strcasecmp(str, "AIR_TEMP") == 0)
        ret->force_type = AIR_TEMP;
    else if (strcasecmp(str, "ALBEDO") == 0)
        ret->force_type = ALBEDO;
    else if (strcasecmp(str, "CATM") == 0)
        ret->force_type = CATM;
    else if (strcasecmp(str, "CHANNEL_IN") == 0)
        ret->force_type = CHANNEL_IN;
    else if (strcasecmp(str, "FCANOPY") == 0)
        ret->force_type = FCANOPY;
    else if (strcasecmp(str, "FDIR") == 0)
        ret->force_type = FDIR;
    else if (strcasecmp(str, "LAI") == 0)
        ret->force_type = LAI;
    else if (strcasecmp(str, "LWDOWN") == 0)
        ret->force_type = LWDOWN;
    else if (strcasecmp(str, "PAR") == 0)
        ret->force_type = PAR;
    else if (strcasecmp(str, "PREC") == 0)
        ret->force_type = PREC;
    else if (strcasecmp(str, "PRESSURE") == 0)
        ret->force_type = PRESSURE;
    else if (strcasecmp(str, "VP") == 0)
        ret->force_type = VP;
    else if (strcasecmp(str, "SWDOWN") == 0)
        ret->force_type = SWDOWN;
    else if (strcasecmp(str, "WIND") == 0)
        ret->force_type = WIND;
    else if (strcasecmp(str, "SKIP") == 0)
        ret->force_type = SKIP;
    else
        error("Invalid FORCE_TYPE: %s\n", str);

    if (ret->force_type == SKIP) {
        ret->is_signed = false;
        ret->multiplier = 1;
    }
    else if (sscanf(buf, "%*s %*s %s %lf", str, &ret->multiplier) == 2) {
        if (strcasecmp(str, "SIGNED"))
            ret->is_signed = true;
        else if (strcasecmp(str, "UNSIGNED"))
            ret->is_signed = false;
        else
            error("Invalid SIGNED/UNSIGNED: %s\n", str);
    }
    else if (sscanf(buf, "%*s %*s %s", str) == 1) {
        ret->nc_name = malloc(strlen(str) + 1);
        strcpy(ret->nc_name, str);
    }

    return ret;
}

static struct domain_type_s *read_domain_type(const char *buf)
{
    struct domain_type_s *ret = calloc(1, sizeof *ret);
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid DOMAIN_TYPE: %s\n", buf);

    if (strcasecmp(str, "LAT") == 0)
        ret->variable = LAT;
    else if (strcasecmp(str, "LON") == 0)
        ret->variable = LON;
    else if (strcasecmp(str, "MASK") == 0)
        ret->variable = MASK;
    else if (strcasecmp(str, "AREA") == 0)
        ret->variable = AREA;
    else if (strcasecmp(str, "FRAC") == 0)
        ret->variable = FRAC;
    else if (strcasecmp(str, "YDIM") == 0)
        ret->variable = YDIM;
    else if (strcasecmp(str, "XDIM") == 0)
        ret->variable = XDIM;
    else
        error("Invalid DOMAIN_TYPE: %s\n", str);

    if (sscanf(buf, "%*s %*s %s", str) == 1) {
        ret->nc_name = malloc(strlen(str) + 1);
        strcpy(ret->nc_name, str);
    }
    else
        error("Missing NC_NAME for DOMAIN_TYPE: %s\n", buf);

    return ret;
}

static enum baseflow read_baseflow(const char *buf)
{
    enum baseflow ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid BASEFLOW: %s\n", buf);

    if (strcasecmp(str, "ARNO") == 0)
        ret = ARNO;
    else if (strcasecmp(str, "NIJSSEN2001") == 0)
        ret = NIJSSEN2001;
    else
        error("Invalid BASEFLOW: %s\n", str);

    return ret;
}

static enum veg_src read_veg_src(const char *buf)
{
    enum veg_src ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid VEG_SRC: %s\n", buf);

    if (strcasecmp(str, "FROM_DEFAULT") == 0)
        ret = FROM_DEFAULT;
    else if (strcasecmp(str, "FROM_VEGLIB") == 0)
        ret = FROM_VEGLIB;
    else if (strcasecmp(str, "FROM_VEGPARAM") == 0)
        ret = FROM_VEGPARAM;
    else if (strcasecmp(str, "FROM_VEGHIST") == 0)
        ret = FROM_VEGHIST;
    else
        error("Invalid VEG_SRC: %s\n", str);

    return ret;
}

static struct snow_band_s *read_snow_band(const char *buf)
{
    struct snow_band_s *ret = calloc(1, sizeof *ret);
    char str[BUF_SIZE];
    int i;

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid SNOW_BAND: %s\n", buf);

    if (strcasecmp(str, "TRUE") == 0)
        ret->enabled = true;
    else if (strcasecmp(str, "FALSE") == 0)
        ret->enabled = false;
    else if (sscanf(buf, "%*s %d %s", &ret->bands, str) == 2) {
        ret->file = malloc(strlen(str) + 1);
        strcpy(ret->file, str);
        ret->enabled = true;
    }
    else if (sscanf(buf, "%*s %d", &i) == 1 && i == 1) {
        ret->bands = 1;
        ret->enabled = false;
    }
    else
        error("Invalid SNOW_BAND: %s\n", str);

    return ret;
}

static struct freq_s *read_freq(const char *buf)
{
    struct freq_s *ret = calloc(1, sizeof *ret);
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid FREQ: %s\n", buf);

    if (strcasecmp(str, "NEVER") == 0)
        ret->frequency = NEVER;
    else if (strcasecmp(str, "NSTEPS") == 0)
        ret->frequency = NSTEPS;
    else if (strcasecmp(str, "NSECONDS") == 0)
        ret->frequency = NSECONDS;
    else if (strcasecmp(str, "NMINUTES") == 0)
        ret->frequency = NMINUTES;
    else if (strcasecmp(str, "NHOURS") == 0)
        ret->frequency = NHOURS;
    else if (strcasecmp(str, "NDAYS") == 0)
        ret->frequency = NDAYS;
    else if (strcasecmp(str, "NMONTHS") == 0)
        ret->frequency = NMONTHS;
    else if (strcasecmp(str, "NYEARS") == 0)
        ret->frequency = NYEARS;
    else if (strcasecmp(str, "DATE") == 0)
        ret->frequency = DATE;
    else if (strcasecmp(str, "END") == 0)
        ret->frequency = END;
    else
        error("Invalid FREQ: %s\n", str);

    if (strcasecmp(str, "DATE") == 0 &&
        sscanf(buf, "%*s %*s %4d-%2d-%2d-%5d", &ret->year, &ret->month,
               &ret->day, &ret->second) >= 3)
        ret->count = 0;
    else if (sscanf(buf, "%*s %*s %d", &ret->count) != 1)
        ret->count = 1;

    return ret;
}

static int read_compress(const char *buf)
{
    int ret;
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) != 1)
        error("Invalid COMPRESS: %s\n", buf);

    if (strcasecmp(str, "TRUE") == 0)
        ret = 1;
    else if (strcasecmp(str, "FALSE") == 0)
        ret = 0;
    else if (sscanf(buf, "%*s %d", &ret) != 1 || ret < 1 || ret > 9)
        error("Invalid COMPRESS: %s\n", buf);

    return ret;
}

static struct outvar_s *read_outvar(const char *buf)
{
    struct outvar_s *ret = calloc(1, sizeof *ret);
    char str[BUF_SIZE];

    if (sscanf(buf, "%*s %s", str) == 1) {
        ret->name = malloc(strlen(str) + 1);
        strcpy(ret->name, str);
    }
    else
        error("Invalid OUTVAR: %s\n", str);

    if (sscanf(buf, "%*s %*s %s", str) == 1) {
        ret->format = malloc(strlen(str) + 1);
        strcpy(ret->format, str);
    }

    if (sscanf(buf, "%*s %*s %*s %s", str) == 1) {
        if (strcasecmp(str, "OUT_TYPE_DEFAULT") == 0)
            ret->out_type = OUT_TYPE_DEFAULT;
        else if (strcasecmp(str, "OUT_TYPE_CHAR") == 0)
            ret->out_type = OUT_TYPE_CHAR;
        else if (strcasecmp(str, "OUT_TYPE_SINT") == 0)
            ret->out_type = OUT_TYPE_SINT;
        else if (strcasecmp(str, "OUT_TYPE_USINT") == 0)
            ret->out_type = OUT_TYPE_USINT;
        else if (strcasecmp(str, "OUT_TYPE_INT") == 0)
            ret->out_type = OUT_TYPE_INT;
        else if (strcasecmp(str, "OUT_TYPE_FLOAT") == 0)
            ret->out_type = OUT_TYPE_FLOAT;
        else if (strcasecmp(str, "OUT_TYPE_DOUBLE") == 0)
            ret->out_type = OUT_TYPE_DOUBLE;
        else
            error("Invalid OUT_TYPE for OUTVAR: %s\n", str);
    }
    else
        ret->out_type = OUT_TYPE_DEFAULT;

    if (sscanf(buf, "%*s %*s %*s %*s %d", &ret->multiplier) != 1)
        ret->multiplier = 1;

    if (sscanf(buf, "%*s %*s %*s %*s %*d %s", str) == 1) {
        if (strcasecmp(str, "AGG_TYPE_DEFAULT") == 0)
            ret->agg_type = AGG_TYPE_DEFAULT;
        else if (strcasecmp(str, "AGG_TYPE_AVG") == 0)
            ret->agg_type = AGG_TYPE_AVG;
        else if (strcasecmp(str, "AGG_TYPE_BEG") == 0)
            ret->agg_type = AGG_TYPE_BEG;
        else if (strcasecmp(str, "AGG_TYPE_END") == 0)
            ret->agg_type = AGG_TYPE_END;
        else if (strcasecmp(str, "AGG_TYPE_MAX") == 0)
            ret->agg_type = AGG_TYPE_MAX;
        else if (strcasecmp(str, "AGG_TYPE_MIN") == 0)
            ret->agg_type = AGG_TYPE_MIN;
        else if (strcasecmp(str, "AGG_TYPE_SUM") == 0)
            ret->agg_type = AGG_TYPE_SUM;
        else
            error("Invalid AGG_TYPE for OUTVAR: %s\n", str);
    }
    else
        ret->agg_type = AGG_TYPE_DEFAULT;

    return ret;
}
