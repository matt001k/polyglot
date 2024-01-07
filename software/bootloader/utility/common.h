#ifndef __BL_COMMON_H
#define __BL_COMMON_H

typedef enum
{
    BL_OK       = 0U,
    BL_ERR      = 1U,
    BL_ENOENT   = 2U,
    BL_EIO      = 5U,
    EL_ENXIO    = 6U,
    BL_ENOMEM   = 12U,
    BL_EACCES   = 13U,
    BL_EBUSY    = 16U,
    BL_ENODEV   = 19U,
    BL_EINVAL   = 22U,
    BL_ENOSYS   = 38U,
    BL_ENOMSG   = 41U,
    BL_ENODATA  = 61U,
} BL_Err_t;

#endif // __BL_COMMON_H