#ifndef _IO_H_
#define _IO_H_

/*Data from user to driver*/
#define PATH_SYSFS_DATA "/sys/devices/platform/vms/sysfs_data"

/*Set operation in driver*/
#define PATH_SYSFS_OPERATION "/sys/devices/platform/vms/sysfs_operation"

/*Current point from driver*/
#define PATH_SYSFS_POINT "/sys/devices/platform/vms/point"

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

#endif

