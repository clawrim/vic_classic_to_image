#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#define REALLOC_INCREMENT 1024
#define BUF_SIZE 2048

#define error(format, ...) \
    do { \
        fprintf(stderr, format, ##__VA_ARGS__); \
        exit(EXIT_FAILURE); \
    } while(0)

#define nc_check(nc_status, format, ...) \
    do { \
	int status = nc_status; \
        if(status != NC_NOERR) \
            error("Error %d at line %d: %s\n" format, status, __LINE__, \
                  nc_strerror(status), ##__VA_ARGS__); \
    } while(0)

#endif
