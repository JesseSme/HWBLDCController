#pragma once

// read and write bits
#define READ    0x80
#define WRITE   0x00

// Test registers
#define DEVID   0x00
#define DEVID_DEF 0b11100101

#define BW_RATE 0x2C
#define BW_RATE_READ (READ | BW_RATE)
#define BW_RATE_DEF 0b00001010