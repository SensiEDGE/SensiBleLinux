#include <assert.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "gattlib.h"
#include "deviceclient.h"

// Uncomment this to have debug output on notifications
#define NOTIFICATION_DEBUG

// Period of sending data from sensors
#define PERIOD_MSEC 1000

// UUID of Characteristics, used in SensiBLE                                                                                                                                              
#define LED_STATE       "20000000-0001-11e1-ac36-0002a5d5c51b"
#define LIGHT_SENSOR    "01000000-0001-11e1-ac36-0002a5d5c51b"
#define BATTERY_STATUS  "00020000-0001-11e1-ac36-0002a5d5c51b"
#define COMPAS          "00000040-0001-11e1-ac36-0002a5d5c51b"
#define CARRY_POSITION  "00000008-0001-11e1-ac36-0002a5d5c51b"
#define ACTIVITY_REC    "00000010-0001-11e1-ac36-0002a5d5c51b"
#define GESTURE_RECOGN  "00000002-0001-11e1-ac36-0002a5d5c51b"
#define ORIENT_ESTIM    "00000100-0001-11e1-ac36-0002a5d5c51b"
#define AUDIO_LVL       "04000000-0001-11e1-ac36-0002a5d5c51b"
#define ACC_GYRO_MAG    "00e00000-0001-11e1-ac36-0002a5d5c51b"
#define ACCEL_EV        "00000400-0001-11e1-ac36-0002a5d5c51b"

// Nuber of characteristics
#define CHARACTERISTICS_COUNT 11

// Timeout for scanning
#define BLE_SCAN_TIMEOUT 4

static const char* uuids[CHARACTERISTICS_COUNT] = {
    LED_STATE,
    LIGHT_SENSOR,
    BATTERY_STATUS,
    COMPAS,
    CARRY_POSITION,
    ACTIVITY_REC,
    GESTURE_RECOGN,
    ORIENT_ESTIM,
    AUDIO_LVL,
    ACC_GYRO_MAG,
    ACCEL_EV,
};

static iotfclient client;

static const char* acc_ev_flag_desc[] = {
    "ACC_NOT_USED\n",
    "ACC_6D_OR_TOP\n",
    "ACC_6D_OR_LEFT\n",
    "ACC_6D_OR_BOTTOM\n",
    "ACC_6D_OR_RIGHT\n",
    "ACC_6D_OR_UP\n",
    "ACC_6D_OR_DOWN\n"
    "ACC_ERROR\n",
    "ACC_TILT\n",
    "ACC_FREE_FALL\n",
    "ACC_SINGLE_TAP\n",
	"ACC_ERROR\n",
    "ACC_DOUBLE_TAP\n",
	"ACC_ERROR\n",
	"ACC_ERROR\n",
	"ACC_ERROR\n",
    "ACC_WAKE_UP\n",
};

const uint8_t acc_ev_flag_desc_err_index = 7;

static const char* gesture_flag_desc[] = {
    "MGR_NOGESTURE\n",
    "MGR_PICKUP\n",
    "MGR_GLANCE\n",
    "MGR_WAKEUP\n",
    "MGR_ERROR\n",
};

const uint8_t gesture_flag_desc_err_index = 4;

static const char* activity_flag_desc[] = {
    "MAR_NOACTIVITY\n",
    "MAR_STATIONARY\n",
    "MAR_WALKING\n",
    "MAR_FASTWALKING\n",
    "MAR_JOGGING\n",
    "MAR_BIKING\n",
    "MAR_DRIVING\n",
    "MAR_ERROR\n",
};

const uint8_t activity_flag_desc_err_index = 7;

static const char* carry_position_flag_desc[] = {
    "MCP_UNKNOWN\n",
    "MCP_ONDESK\n",
    "MCP_INHAND\n",
    "MCP_NEARHEAD\n",
    "MCP_SHIRTPOCKET\n",
    "MCP_TROUSERPOCKET\n",
    "MCP_ARMSWING\n",
    "MCP_JACKETPOCKET\n",
};

const uint8_t carry_position_flag_desc_err_index = 0;

static const char* battery_status_flag_desc[] = {
    "Status of Battery is discharging.\n",
    "Status of Battery is charging.\n",
    "Error status of Battery\n",
    "Status of Battery is unknown.\n",
};

const uint8_t battery_status_flag_desc_err_index = 2;

void iot_publish_variable(const char* name, int32_t value){
    char pdata[64];
    snprintf(pdata, 64, "{\"d\" : {\"%s\" : %d }}", name, value);
    if (publishEvent(&client, "status", "json", pdata, QOS0) != 0){
        printf("Error while publishing the event stat %s\n", name);
    }
}

void get_led_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t led_timestamp_prev = 0;
    uint16_t led_timestamp_current = (data[1] << 8) | (data[0] & 0xff);
    uint8_t led_state = (data[2] << 8);
    
    if (led_timestamp_current >= led_timestamp_prev && (led_timestamp_current - led_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    led_timestamp_prev = led_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Led State Notification Handler: \n");
    printf("Timestamp is:= %05dmS\n", led_timestamp_current);
    if (led_state == 0x00){
        printf("Led state is OFF\n");
    }
    else{
        printf("Led state is ON\n");
    }
    printf("============================\n");
#endif
    iot_publish_variable("led", 0x0000FFFF & led_state);
}

void get_light_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data) {
    static uint16_t light_timestamp_prev = 0;
    uint16_t light_timestamp_current = (data[1] << 8) | (data[0] & 0xff);
    uint16_t light_sens_val = (data[3] << 8) | (data[2] & 0xff);
    
    if (light_timestamp_current >= light_timestamp_prev && (light_timestamp_current - light_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    light_timestamp_prev = light_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Light Notification Handler: \n");
    printf("Timestamp is:= %05dmS\n", light_timestamp_current);
    printf("Light Sensor Value is: = %5dLux\n", light_sens_val);
    printf("============================\n");
#endif
    iot_publish_variable("light", 0x0000FFFF & light_sens_val);
}

void get_battery_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t battery_timestamp_prev = 0;
    uint16_t battery_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint16_t battery_soc = ((data[3] << 8) | (data[2] & 0xff));
    uint16_t battery_voltage = ((data[5] << 8) | (data[4] & 0xff));
    uint16_t battery_current = ((data[7] << 8) | (data[6] & 0xff));
    uint8_t battery_status = data[8];
    
    if (battery_timestamp_current >= battery_timestamp_prev && (battery_timestamp_current - battery_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    battery_timestamp_prev = battery_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Battery State Notification Handler: \n");
    printf("Timestamp is:= %05dmS\n", battery_timestamp_current);
    printf("Battery soc is:= %05d%%\n", battery_soc);
    printf("Battery voltage is:= %05dmV\n", battery_voltage);
    printf("Battery current is:= %05dmA\n", battery_current);
    uint8_t index = battery_status - 1;
    if (battery_status == 0x00 || battery_status > 0x04){
        index = battery_status_flag_desc_err_index;
    }
    printf("%s", battery_status_flag_desc[index]);
    printf("============================\n");
#endif
    iot_publish_variable("batt_soc", 0x0000FFFF & battery_soc);
    iot_publish_variable("batt_volt", 0x0000FFFF & battery_voltage);
    iot_publish_variable("batt_curr", 0x0000FFFF & battery_current);
    iot_publish_variable("batt_stat", 0x0000FFFF & battery_status);
}

void get_compas_notification_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t compass_timestamp_prev = 0;
    uint16_t compass_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint16_t compass_angle = ((data[3] << 8) | (data[2] & 0xff));
    
    if (compass_timestamp_current >= compass_timestamp_prev && (compass_timestamp_current - compass_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    compass_timestamp_prev = compass_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Compas Notification Handler: \n");
    printf("Timestamp is:= %05dmS\n", compass_timestamp_current);
    printf("Angle between the board and north direction is:= %3d*\n", (compass_angle/100));
    printf("============================\n");
#endif
    iot_publish_variable("compass", 0x0000FFFF & compass_angle);
}

void get_carry_position_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t carry_timestamp_prev = 0;
    uint16_t carry_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint8_t carry_position_flag = data[2];
    
    if (carry_timestamp_current >= carry_timestamp_prev && (carry_timestamp_current - carry_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    carry_timestamp_prev = carry_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Carry Position Handler: \n");
    printf("Timestamp is:= %05dmS\n", carry_timestamp_current);
    uint8_t index = carry_position_flag;
    if(carry_position_flag > 0x06){
        index = carry_position_flag_desc_err_index;
    }
    printf("%s", carry_position_flag_desc[index]);
#endif
    iot_publish_variable("carry", 0x0000FFFF & carry_position_flag);
}

void get_activity_recognition_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t activity_timestamp_prev = 0;
    uint16_t activity_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint8_t activity_flag = data[2];
    
    if (activity_timestamp_current >= activity_timestamp_prev && (activity_timestamp_current - activity_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    activity_timestamp_prev = activity_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Activity Recognition Handler: \n");
    printf("Timestamp is:= %05dmS\n", activity_timestamp_current);
    uint8_t index = activity_flag;
    if(activity_flag > 0x06){
        index = activity_flag_desc_err_index;
    }
    printf("%s", activity_flag_desc[index]);
    printf("============================\n");
#endif
    iot_publish_variable("activity", 0x0000FFFF & activity_flag);
}

void get_gesture_recognition_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t gesture_timestamp_prev = 0;
    uint16_t gesture_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint8_t gesture_flag = data[2];
    
    if (gesture_timestamp_current >= gesture_timestamp_prev && (gesture_timestamp_current - gesture_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    gesture_timestamp_prev = gesture_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Gesture Recognition Handler: \n");
    printf("Timestamp is:= %05dmS\n", gesture_timestamp_current);
    uint8_t index = gesture_flag;
    if(gesture_flag > 0x03){
        index = gesture_flag_desc_err_index;
    }
    printf("%s", gesture_flag_desc[index]);
    printf("============================\n");
#endif
    iot_publish_variable("gesture", 0x0000FFFF & gesture_flag);
}

void get_orientation_estimation_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t orient_timestamp_prev = 0;
    uint16_t orient_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint16_t quat0_X = ((data[3] << 8) | (data[2] & 0xff));
    uint16_t quat0_Y = ((data[5] << 8) | (data[4] & 0xff));
    uint16_t quat0_Z = ((data[7] << 8) | (data[6] & 0xff));
    uint16_t quat1_X = ((data[9] << 8) | (data[8] & 0xff));
    uint16_t quat1_Y = ((data[11] << 8) | (data[10] & 0xff));
    uint16_t quat1_Z = ((data[13] << 8) | (data[12] & 0xff));
    uint16_t quat2_X = ((data[15] << 8) | (data[14] & 0xff));
    uint16_t quat2_Y = ((data[17] << 8) | (data[16] & 0xff));
    uint16_t quat2_Z = ((data[19] << 8) | (data[18] & 0xff));
    
    if (orient_timestamp_current >= orient_timestamp_prev && (orient_timestamp_current - orient_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    orient_timestamp_prev = orient_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Orientation Estimation Handler: \n");
    printf("Timestamp is:= %05dmS\n", orient_timestamp_current);
    printf("quat0_X is:= %05d\n", (quat0_X/1000));
    printf("quat0_Y is:= %05d\n", (quat0_Y/1000));
    printf("quat0_Z is:= %05d\n", (quat0_Z/1000));
    printf("quat1_X is:= %05d\n", (quat1_X/1000));
    printf("quat1_Y is:= %05d\n", (quat1_Y/1000));
    printf("quat1_Z is:= %05d\n", (quat1_Z/1000));
    printf("quat2_X is:= %05d\n", (quat2_X/1000));
    printf("quat2_Y is:= %05d\n", (quat2_Y/1000));
    printf("quat2_Z is:= %05d\n", (quat2_Z/1000));

    printf("============================\n");
#endif
    iot_publish_variable("quat0_X", 0x0000FFFF & quat0_X);
    iot_publish_variable("quat0_Y", 0x0000FFFF & quat0_Y);
    iot_publish_variable("quat0_Z", 0x0000FFFF & quat0_Z);
    iot_publish_variable("quat1_X", 0x0000FFFF & quat1_X);
    iot_publish_variable("quat1_Y", 0x0000FFFF & quat1_Y);
    iot_publish_variable("quat1_Z", 0x0000FFFF & quat1_Z);
    iot_publish_variable("quat2_X", 0x0000FFFF & quat2_X);
    iot_publish_variable("quat2_Y", 0x0000FFFF & quat2_Y);
    iot_publish_variable("quat2_Z", 0x0000FFFF & quat2_Z);
}

void get_audio_level_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t audio_timestamp_prev = 0;
    uint16_t audio_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint8_t audio_level = data[2];
    
    if (audio_timestamp_current >= audio_timestamp_prev && (audio_timestamp_current - audio_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    audio_timestamp_prev = audio_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Audio Level Handler: \n");
    printf("Timestamp is:= %05dmS\n", audio_timestamp_current);
    printf("Audio Level is:= %03d\n", audio_level);
    printf("============================\n");
#endif
    iot_publish_variable("audio", 0x0000FFFF & audio_level);
}

void get_acc_gyro_mag_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t acc_timestamp_prev = 0;
    uint16_t acc_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    int16_t acc_x = ((data[3] << 8) | (data[2] & 0xff));
    int16_t acc_y = ((data[5] << 8) | (data[4] & 0xff));
    int16_t acc_z = ((data[7] << 8) | (data[6] & 0xff));
    int16_t gyro_x = ((data[9] << 8) | (data[8] & 0xff));
    int16_t gyro_y = ((data[11] << 8) | (data[10] & 0xff));
    int16_t gyro_z = ((data[13] << 8) | (data[12] & 0xff));
    int16_t mag_x = ((data[15] << 8) | (data[14] & 0xff));
    int16_t mag_y = ((data[17] << 8) | (data[16] & 0xff));
    int16_t mag_z = ((data[19] << 8) | (data[18] & 0xff));
    
    if (acc_timestamp_current >= acc_timestamp_prev && (acc_timestamp_current - acc_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    acc_timestamp_prev = acc_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Timestamp is:= %05dmS\n", acc_timestamp_current);
    printf("ACCx is:= %05dmg\n", acc_x);
    printf("ACCy is:= %05dmg\n", acc_y);
    printf("ACCz is:= %05dmg\n", acc_z);
    printf("GYROx is:= %05dmdps/100\n", gyro_x);
    printf("GYROy is:= %05dmdps/100\n", gyro_y);
    printf("GYROz is:= %05dmdps/100\n", gyro_z);
    printf("MAGx is:= %05dmGa\n", mag_x);
    printf("MAGy is:= %05dmGa\n", mag_y);
    printf("MAGz is:= %05dmGa\n", mag_z);
    printf("============================\n");
#endif
    iot_publish_variable("acc_x", acc_x);
    iot_publish_variable("acc_y", acc_y);
    iot_publish_variable("acc_z", acc_z);
    iot_publish_variable("gyro_x", gyro_x);
    iot_publish_variable("gyro_y", gyro_y);
    iot_publish_variable("gyro_z", gyro_z);
    iot_publish_variable("mag_x", mag_x);
    iot_publish_variable("mag_y", mag_y);
    iot_publish_variable("mag_z", mag_z);
}

void get_acc_events_handler(const uuid_t* uuid, const uint8_t* data, size_t data_length, void* user_data){
    static uint16_t acc_ev_timestamp_prev = 0;
    uint16_t acc_ev_timestamp_current = ((data[1] << 8) | (data[0] & 0xff));
    uint8_t acc_ev_flag_high = data[2];
    uint16_t acc_steps_count = 0;
    uint8_t acc_ev_flag_low = 0;
    if (acc_ev_flag_high == 0)
        acc_steps_count = ((data[4] << 8) | (data[3] & 0xff));
    else
        acc_ev_flag_low = data[3];
    
    if (acc_ev_timestamp_current >= acc_ev_timestamp_prev && (acc_ev_timestamp_current - acc_ev_timestamp_prev) < PERIOD_MSEC){
        return;
    }
    acc_ev_timestamp_prev = acc_ev_timestamp_current;
    
#if defined(NOTIFICATION_DEBUG)
    printf("============================\n");
    printf("Timestamp is:= %05dmS\n", acc_ev_timestamp_current);
    if (acc_ev_flag_high != 0){
        uint8_t index = acc_ev_flag_low;
        if (index > 0x08 && index < 0x10){ 
            index = acc_ev_flag_desc_err_index;
        }
        else if (index >= 0x10){
            if (index != 0x10 && index != 0x20 && index != 0x40 && index != 0x80){
                index = acc_ev_flag_desc_err_index;
            }else{
                index = (index >> 4) + 0x08;
            }
        }
        printf("%s", acc_ev_flag_desc[index]);
    }
    printf("============================\n");
#endif
    iot_publish_variable("acc_ev_fl_1", 0x0000FFFF & acc_ev_flag_high);
    iot_publish_variable("acc_ev_fl_2", 0x0000FFFF & acc_ev_flag_low);
    iot_publish_variable("acc_steps", 0x0000FFFF & acc_steps_count);
}

static void (*notification_handlers[CHARACTERISTICS_COUNT])(const uuid_t*, const uint8_t*, size_t, void*) = {
    get_led_notification_handler,
    get_light_notification_handler,
    get_battery_notification_handler,
    get_compas_notification_handler,
    get_carry_position_handler,
    get_activity_recognition_handler,
    get_gesture_recognition_handler,
    get_orientation_estimation_handler,
    get_audio_level_handler,
    get_acc_gyro_mag_handler,
    get_acc_events_handler,
};

static void ble_discovered_device(const char* addr, const char* name){
    if (name){
        printf("Discovered %s - '%s'\n", addr, name);
    }else{
        printf("Discovered %s\n", addr);
    }
}

int main(void){

    const char* adapter_name;
    void* adapter;
    gatt_connection_t* connections[CHARACTERISTICS_COUNT] = {NULL};
    int ret;
    adapter_name = NULL;
    char bluetooth_address[30];
    uint8_t i;
    uuid_t characteristic_rx_uuid;
    int rc = -1;

    rc = initialize(&client, "quickstart", "internetofthings.ibmcloud.com", "HummingBoard_SensiEdge",
                    "HB_SE_1", "token", "hb-se-1-t", "iot-embeddedc/IoTFoundation.pem", 
                    0, NULL, NULL, NULL, 0);

    if(rc != SUCCESS){
        printf("Initialize returned rc = %d.\n Quitting..\n", rc);
        return 0;
    }

    rc = connectiotf(&client);

    if(rc != SUCCESS){
        printf("Connection returned rc = %d.\n Quitting..\n", rc);
        return 0;
    }

    printf("Connection Successful. Press Ctrl+C to quit\n");
    
    ret = gattlib_adapter_open(adapter_name, &adapter);
    if (ret){
        fprintf(stderr, "ERROR: Failed to open adapter.\n");
        return 1;
    }

    ret = gattlib_adapter_scan_enable(adapter, ble_discovered_device, BLE_SCAN_TIMEOUT);
    if (ret){
        fprintf(stderr, "ERROR: Failed to scan.\n");
        return 1;
    }

    gattlib_adapter_scan_disable(adapter);

    puts("Scan completed. Now please type your BLE MAC-adress:");
    scanf("%s", bluetooth_address);

    for (i = 0; i < CHARACTERISTICS_COUNT; i++){

        connections[i] = gattlib_connect(NULL, bluetooth_address, BDADDR_LE_PUBLIC, BT_SEC_LOW, 0, 0);
        if (connections[i] == NULL){
            fprintf(stderr, "Fail to connect to the bluetooth device for characteric number %hhu.\n", i);
            continue;
        }

        if (gattlib_string_to_uuid(uuids[i], strlen(uuids[i]) + 1, &characteristic_rx_uuid) < 0) {
            printf("Error func gattlib_string_to_uuid for characteristic number %hhu\n", i);
            continue;
        }

        gattlib_register_notification(connections[i], notification_handlers[i], NULL);

        ret = gattlib_notification_start(connections[i], &characteristic_rx_uuid);
        if (ret){
            fprintf(stderr, "Fail to start notification\n.");
            continue;
        }
        ret = gattlib_notification_start(connections[i], &characteristic_rx_uuid);
        if (ret){
            fprintf(stderr, "Fail to start notification\n.");
            continue;
        }
    }

    GMainLoop *loop = g_main_loop_new(NULL, 0);
    g_main_loop_run(loop);

    g_main_loop_unref(loop);

    for (i = 0; i < CHARACTERISTICS_COUNT; i++){
        if (connections[i] != NULL){
            gattlib_disconnect(connections[i]);
        }
    }

    printf("Quitting!!\n");

    disconnect(&client);

    return 0;
}

