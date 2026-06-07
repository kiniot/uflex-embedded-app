//
// Created by Salim Ramirez Mestanza on 6/06/26.
//
#include "../../common/wokwi-api.h"
#include <stdio.h>
#include <stdlib.h>

const int ADDRESS = 0x70; // Default I2C address for TCA9548A

typedef struct {
    pin_t pin_reset;
    pin_t pins_scl_sda[8][2]; // Each channel has an SCL and SDA pair
    uint8_t channel_enable;   // Bitmask to track enabled channels
} chip_state_t;

static bool on_i2c_connect(void *user_data, uint32_t address, bool connect);
static uint8_t on_i2c_read(void *user_data);
static bool on_i2c_write(void *user_data, uint8_t data);
static void on_i2c_disconnect(void *user_data);

void chip_init() {
    chip_state_t *chip = malloc(sizeof(chip_state_t));
    chip->pin_reset = pin_init("RESET", INPUT_PULLUP);

    // Initialize SCL and SDA pins for each channel
    for (int i = 0; i < 8; i++) {
        char label[10];
        sprintf(label, "SCL%d", i);
        chip->pins_scl_sda[i][0] = pin_init(label, INPUT);
        sprintf(label, "SDA%d", i);
        chip->pins_scl_sda[i][1] = pin_init(label, INPUT);
    }
    chip->channel_enable = 0;

    const i2c_config_t i2c_config = {
        .user_data = chip,
        .address = ADDRESS,
        .scl = pin_init("SCL", INPUT_PULLUP),
        .sda = pin_init("SDA", INPUT_PULLUP),
        .connect = on_i2c_connect,
        .read = on_i2c_read,
        .write = on_i2c_write,
        .disconnect = on_i2c_disconnect
    };
    i2c_init(&i2c_config);
    printf("TCA9548A chip initialized at address 0x%02X\n", ADDRESS);
}

bool on_i2c_connect(void *user_data, uint32_t address, bool connect) {
    printf("I2C connect: address=0x%02X read=%s\n", address, connect ? "true" : "false");
    return address == ADDRESS;
}

uint8_t on_i2c_read(void *user_data) {
    // Return the currently active channels
    chip_state_t *chip = (chip_state_t *)user_data;
    printf("I2C read: returning 0x%02X\n", chip->channel_enable);
    return chip->channel_enable;
}

bool on_i2c_write(void *user_data, uint8_t data) {
    chip_state_t *chip = (chip_state_t *)user_data;
    // Update channel enables based on data
    chip->channel_enable = data;
    printf("I2C write: data=0x%02X\n", data);
    // Update pin modes based on enabled channels
    for (int i = 0; i < 8; i++) {
        if (data & (1 << i)) {
            pin_mode(chip->pins_scl_sda[i][0], OUTPUT); // Enable SCL
            pin_mode(chip->pins_scl_sda[i][1], OUTPUT); // Enable SDA
        } else {
            pin_mode(chip->pins_scl_sda[i][0], INPUT); // Disable SCL
            pin_mode(chip->pins_scl_sda[i][1], INPUT); // Disable SDA
        }
    }
    return true;
}

void on_i2c_disconnect(void *user_data) {
    printf("I2C disconnect\n");
}
