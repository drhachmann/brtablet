#ifndef _IO_H_
#define _IO_H_


#define OP_MTX 1
#define OP_POINT 2
#define OP_CALIBRATED 3

/*Data from user to driver*/
#define PATH_SYSFS_DATA "/sys/devices/platform/brtablet/data"

/*Operation from user to driver*/
#define PATH_SYSFS_OPERATION "/sys/devices/platform/brtablet/operation"

/*Current point from driver*/
#define PATH_SYSFS_POINT "/sys/devices/platform/brtablet/point"

#define PATH_FILE_DEVICE "/etc/brtablet/device"

/*Coeficients matrix tranformation file*/
#define PATH_FILE_MTX "/etc/brtablet/coef"

/*uvz point file*/
#define PATH_FILE_UVZ "/etc/brtablet/uvz"

/*Rows of matrix calibration*/
#define N_ROW  5

/*Columns of matrix calibration*/
#define N_COLUMN 5

/*Shift to transform double <-> int*/
#define N_SHIFT 13

/**/
#define PATH_EXEC_ATTACH "/etc/brtablet/brtablet-attach"

#define PATH_LOCK_ATTACH "/etc/brtablet/lock_attack"


/**/

#endif


