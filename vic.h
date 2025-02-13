#ifndef _VIC_H_
#define _VIC_H_

#include <stdbool.h>
#include "global.h"

#define MAX_LAKE_NODES 20
/* VIC/vic/vic_run/include/vic_physical_constants.h */
#define CONST_REARTH 6.37122e6  /* radius of the Earth in m */
#define AREA_UNITS "m2"

enum calendar
{
    STANDARD,                   /* default */
    GREGORIAN,
    PROLEPTIC_GREGORIAN,
    NOLEAP,
    DAY_365,                    /* 365_DAY */
    DAY_360,                    /* 360_DAY */
    JULIAN,
    ALL_LEAP,
    DAY_366                     /* 366_DAY */
};

enum time_units
{
    SECONDS,
    MINUTES,
    HOURS,
    DAYS                        /* default */
};

enum grnd_flux_type
{
    GF_406,
    GF_410                      /* default */
};

enum snow_density
{
    DENS_BRAS,                  /* default */
    DENS_SNTHRM
};

enum aero_resist_cansnow
{
    AR_406,
    AR_406_LS,
    AR_406_FULL,                /* default */
    AR_410
};

enum rc_mode
{
    RC_JARVIS,                  /* default */
    RC_PHOTO
};

enum file_format
{
    /* for classic driver */
    BINARY,
    ASCII,
    /* else for image driver */
    NETCDF3_CLASSIC,
    NETCDF3_64BIT_OFFSET,
    NETCDF4_CLASSIC,
    NETCDF4
};

enum endian
{
    BIG,
    LITTLE
};

enum force_type
{
    AIR_TEMP,
    ALBEDO,
    CATM,
    CHANNEL_IN,
    FCANOPY,
    FDIR,
    LAI,
    LWDOWN,
    PAR,
    PREC,
    PRESSURE,
    VP,
    SWDOWN,
    WIND,
    SKIP
};

enum baseflow
{
    ARNO,                       /* default */
    NIJSSEN2001
};

enum veg_src
{

    FROM_DEFAULT,               /* default for fcan_src;
                                 * not for alb_src and lai_src */
    FROM_VEGLIB,                /* default for alb_src and lai_src */
    FROM_VEGPARAM,
    FROM_VEGHIST
};

enum freq
{
    NEVER,
    NSTEPS,
    NSECONDS,
    NMINUTES,
    NHOURS,
    NDAYS,                      /* default */
    NMONTHS,
    NYEARS,
    DATE,
    END
};

enum out_type
{
    OUT_TYPE_DEFAULT,
    OUT_TYPE_CHAR,
    OUT_TYPE_SINT,
    OUT_TYPE_USINT,
    OUT_TYPE_INT,
    OUT_TYPE_FLOAT,
    OUT_TYPE_DOUBLE
};

enum agg_type
{
    AGG_TYPE_DEFAULT,
    AGG_TYPE_AVG,
    AGG_TYPE_BEG,
    AGG_TYPE_END,
    AGG_TYPE_MAX,
    AGG_TYPE_MIN,
    AGG_TYPE_SUM
};

enum domain_variable
{
    LAT,
    LON,
    MASK,
    AREA,
    FRAC,
    YDIM,
    XDIM
};

struct force_type_s
{
    enum force_type force_type;
    /* for classic driver */
    bool is_signed;
    double multiplier;
    /* else for image driver */
    char *nc_name;
};

struct domain_type_s
{
    enum domain_variable variable;
    char *nc_name;
};

struct snow_band_s
{
    /* for classic driver */
    int bands;                  /* default 1 (false) */
    char *file;                 /* default NULL (off) */
    /* else for image driver */
    bool enabled;               /* default false */
};

struct freq_s
{
    enum freq frequency;        /* default NDAYS */
    int count;                  /* default 1 */
    /* YYYY-MM-DD[-SSSSS] for DATE */
    int year;
    int month;
    int day;
    int second;
};

struct outvar_s
{
    char *name;
    char *format;
    enum out_type out_type;
    int multiplier;
    enum agg_type agg_type;
};

/* https://vic.readthedocs.io/en/master/Documentation/Drivers/Classic/GlobalParam/ */
struct global_params_s
{
    /* main simulation parameters */
    int nlayer;
    int nodes;
    int model_steps_per_day;
    int snow_steps_per_day;
    int runoff_steps_per_day;
    int startyear;
    int startmonth;
    int startday;
    int startsec;               /* default 0 */
    /* either */
    int nrecs;
    /* or */
    int endyear;
    int endmonth;
    int endday;
    /* end of either */
    enum calendar calendar;     /* default STANDARD */
    enum time_units out_time_units;     /* default DAYS */

    /* energy balance parameters */
    bool full_energy;           /* default false */
    bool close_energy;          /* default false */

    /* soil termperature parameters */
    bool frozen_soil;           /* default false */
    bool quick_flux;            /* default false */
    bool implicit;              /* default false */
    bool quick_solve;           /* default false */
    bool noflux;                /* default false */
    bool exp_trans;             /* default true */
    enum grnd_flux_type grnd_flux_type; /* default GF_410 */
    bool tfallback;             /* default true */
    bool share_layer_moist;     /* default true */
    bool spatial_frost;         /* default false */

    /* precipitation (rain and snow) parameters */
    enum snow_density snow_density;     /* default DENS_BRAS */
    bool blowing;               /* default false */
    bool blowing_var_threshold; /* default true */
    bool blowing_calc_prob;     /* default true */
    bool blowing_simple;        /* default false */
    bool blowing_fetch;         /* default true */
    bool blowing_spatial_wind;  /* default true */
    int compute_treeline;       /* false or veg_class id; default false */
    bool corrprec;              /* default false */
    /* MAX_SNOW_TEMP and MIN_RAIN_TEMP from the image driver documentation are
     * deprecated with a warning message in the classic driver, and they are
     * not read in the image driver at all */
    bool spatial_snow;          /* default false */

    /* turbulent flux parameters */
    float min_wind_speed;       /* default 0.1 m/s */
    enum aero_resist_cansnow aero_resist_cansnow;       /* default AR_406_FULL */

    /* carbon parameters */
    bool carbon;                /* default false */
    enum rc_mode rc_mode;       /* default RC_JARVIS */
    bool veglib_photo;          /* default false */

    /* miscellaneous parameters */
    bool continueonerror;       /* default true */

    /* state files */
    char *init_state;           /* input text/NetCDF */
    char *statename;            /* prefix for output text/NetCDF */
    int stateyear;
    int statemonth;
    int stateday;
    int statesec;
    enum file_format state_format;

    /* meteorological and vegetation forcing files */
    char *forcing1;             /* input text/NetCDF */
    char *forcing2;             /* input text/NetCDF */
    /* for classic driver */
    enum file_format force_format;
    enum endian force_endian;
    /* XXX: unused in the VIC code; instead, FORCE_TYPEs are counted */
    int *n_types;
    /* for image driver as well */
    struct force_type_s ***force_type;  /* image driver requires AIR_TEMP, PREC,
                                         * PRESSURE, SWDOWN, LWDOWN, VP, WIND */
    /* end of image driver */
    int *force_steps_per_day;
    int *forceyear;
    int *forcemonth;
    int *forceday;
    int *forcesec;              /* default 0 */
    int grid_decimal;
    /* end of classic driver */
    float wind_h;               /* default 10m */
    int canopy_layers;          /* default 3 */

    /* for image driver */
    char *domain;               /* input NetCDF */
    int n_domain_types;         /* internal */
    struct domain_type_s **domain_type;
    /* end of image driver */

    /* parameter files */
    /* for classic driver */
    char *soil;
    /* else for image driver */
    char *parameters;           /* input NetCDF */
    /* end of image driver */
    enum baseflow baseflow;
    bool july_tavg_supplied;    /* default false */
    bool organic_fract;         /* default false */
    /* for classic driver */
    char *veglib;
    char *vegparam;
    int root_zones;
    bool vegparam_alb;          /* default false */
    /* end of classic driver */
    enum veg_src alb_src;       /* default FROM_VEGLIB */
    /* for classic driver */
    bool vegparam_lai;          /* default false */
    /* end of classic driver */
    enum veg_src lai_src;       /* default FROM_VEGLIB */
    /* for classic driver */
    bool veglib_fcan;           /* default false */
    bool vegparam_fcan;         /* default false */
    /* end of classic driver */
    enum veg_src fcan_src;      /* default FROM_DEFAULT */
    struct snow_band_s *snow_band;      /* default disabled */
    char *constants;

    /* lake parameters */
    char *lakes;                /* default NULL (off) */
    bool lake_profile;          /* default false */
    bool equal_area;            /* default false */
    float resolution;
    int lake_nodes;             /* default MAX_LAKE_NODES */

    /* output files */
    char *log_dir;              /* default stdout */
    char *result_dir;
    int n_outfiles;             /* internal */
    char **outfile;             /* prefix for output text/NetCDF */
    struct freq_s **aggfreq;    /* default NDAYS */
    /* for image driver */
    struct freq_s **histfreq;   /* default to one single file */
    /* end of image driver */
    int *compress;              /* 0 for no compression;
                                 * 1-9 for compression level */
    enum file_format *out_format;       /* default ASCII */
    int *n_outvars;             /* internal */
    struct outvar_s ***outvar;
};

struct domain_s
{
    struct double_stack_s *lat;
    struct double_stack_s *lon;
    int *mask;
    double *area;
    double *frac;
};

struct soil_cell_s
{
    bool run_cell;
    int gridcel;
    double lat;
    double lon;
    double infilt;
    double Ds;
    double Dsmax;
    double Ws;
    double c;
    double *expt;
    double *Ksat;
    double *phi_s;
    double *init_moist;
    double elev;
    double *depth;
    double avg_T;
    double dp;
    double *bubble;
    double *quartz;
    double *bulk_density;
    double *soil_density;

    /* if global_params->organic_fract */
    double *organic;
    double *bulk_dens_org;
    double *soil_dens_org;
    /* end if */

    double off_gmt;
    double *Wcr_FRACT;
    double *Wpwp_FRACT;
    double rough;
    double snow_rough;
    double annual_prec;
    double *resid_moist;
    bool fs_active;

    /* if global_params->spatial_frost */
    double frost_slope;
    double max_snow_distrib_slope;
    /* end if */

    /* if global_params->july_tavg_supplied */
    double July_Tavg;
    /* end if */
};

struct soil_s
{
    struct domain_s *domain;
    int n_cells;
    struct soil_cell_s **cells;
};

struct veg_class_s
{
    int veg_class;
    bool overstory;
    double rarc;
    double rmin;
    double *LAI;
    double *FCANOPY;
    double *albedo;
    double *rough;
    double *displacement;
    double wind_h;
    double RGL;
    double rad_atten;
    double wind_atten;
    double trunk_ratio;
    int Ctype;
    double MaxCarboxRate;
    double MaxETransport;
    double LightUseEff;
    bool NscaleFlag;
    double Wnpp_inhib;
    double NPPfactor_sat;
    char *comment;
};

struct veg_lib_s
{
    int n_classes;
    struct veg_class_s **classes;
};

struct veg_cell_s
{
    int gridcel;
    int Nveg;
    int *veg_class;
    double *Cv;
    double **root_depth;
    double **root_fract;
    double sigma_slope;
    double lag_one;
    double fetch;
    double **LAI;
    double **FCANOPY;
    double **ALBEDO;
};

struct veg_params_s
{
    int root_zones;
    int n_cells;
    struct veg_cell_s **cells;
};

/* global_params.c */
struct global_params_s *read_global_params(char *);
void free_global_params(struct global_params_s *);
int is_classic(struct global_params_s *);
int is_image(struct global_params_s *);
void populate_image_global_params(struct global_params_s *, const char *);

/* soil.c */
struct soil_s *read_classic_soil(struct global_params_s *);
void free_soil(struct soil_s *soil);

/* veg_lib.c */
struct veg_lib_s *read_classic_veg_lib(struct global_params_s *);
void free_veg_lib(struct veg_lib_s *);

/* veg_params.c */
struct veg_params_s *read_classic_veg_params(struct global_params_s *);
void free_veg_params(struct veg_params_s *);

/* image_domain.c */
void create_image_domain(struct global_params_s *, struct domain_s *);

/* image_params.c */
void create_image_params(struct global_params_s *, struct soil_s *,
                         struct veg_lib_s *, struct veg_params_s *);

#endif
