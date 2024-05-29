#ifndef SEI_DATA_H
#define SEI_DATA_H

#include <QMetaType>

typedef struct	  // 10 байт
{
    unsigned char track;
    unsigned char type;
    unsigned short x;
    unsigned short y;
    unsigned short width;
    unsigned short height;
} strob_struct;

typedef struct	  // 10 байт
{
    float fov_h;
    float fov_v;
    unsigned char type;
} camera_struct;

typedef struct
{
    unsigned int id;
    unsigned int sys_time;
    unsigned char wmode;
    unsigned char trkstat;
    strob_struct strob[5];
    float yaw_bla;
    float pitch_bla;
    float roll_bla;
    float latitude_bla;
    float longitude_bla;
    float altitude_bla;
    unsigned int ld_distance;
    float yaw;
    float pitch;
    float roll;
    float latitude;
    float longitude;
    float altitude;
    camera_struct camera;
    unsigned char rezerv[7];
} Data_sei_str;

Q_DECLARE_METATYPE(Data_sei_str)

#pragma pack(pop)

#endif
