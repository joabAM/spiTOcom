#include <stdint.h>

struct Ro_header{
    uint8_t rocket_id;  //= 128
    uint8_t ground_id;  //= 255
    uint8_t msg_type;
};

struct Ro_message_status {
    uint16_t mode;
    //flags = sd, gps, baro, imu, batt, cal, safe
  uint8_t flags;
};

struct Ro_message_navigation{
    uint16_t mode;
    int16_t speed1;
    uint16_t speed2;
    int16_t altitude1;
    uint16_t altitude2;
};

struct Ro_message_battery{
    uint16_t mode;
    uint16_t voltage1;
    uint16_t voltage2;
};

struct Ro_message_gps{
    uint16_t mode;
    int32_t longitude;
    int32_t latitude;
    //uint16_t longitude;
    //uint8_t longitude2;
    //uint16_t latitude;
    //uint8_t latitude2;
    uint16_t altitude;
    uint16_t speed;
    uint8_t sats;
    uint16_t date;
};

struct Ro_message_drogue{
    uint16_t mode;
    uint8_t ejection; //255 = true  
};


struct Ro_message_level{
    uint16_t mode;
    uint8_t level; //255 = true  
};
