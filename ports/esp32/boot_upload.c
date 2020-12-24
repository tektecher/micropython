#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "esp_ota_ops.h"
#include "driver/uart.h"

#include "lib/littlefs/lfs2.h"

#if MICROPY_ESP_IDF_4
#include "esp32/rom/uart.h"
#else
#include "rom/uart.h"
#endif

#include "genhdr/mpversion.h"
#include "mpconfigboard.h"

#define WAIT_BOOT_TIMEOUT 300

#define PARTITION_SIZE   (0x200000)
#define BLOCK_SIZE_BYTES (4096)
#define BLOCK_COUNT      (PARTITION_SIZE / BLOCK_SIZE_BYTES)

const esp_partition_t *part;

int block_read(const struct lfs2_config *c, lfs2_block_t block, lfs2_off_t off, void *buffer, lfs2_size_t size) {
    esp_err_t err = esp_partition_read(part, (block * BLOCK_SIZE_BYTES) + off, buffer, size);

    return 0;
}

int block_prog(const struct lfs2_config *c, lfs2_block_t block, lfs2_off_t off, const void *buffer, lfs2_size_t size) {
    esp_err_t err = esp_partition_write(part, (block * BLOCK_SIZE_BYTES) + off, buffer, size);

    return 0;
}

int block_erase(const struct lfs2_config *c, lfs2_block_t block) {
    esp_partition_erase_range(part, (block * BLOCK_SIZE_BYTES), BLOCK_SIZE_BYTES);

    return 0;
}

int block_sync(const struct lfs2_config *c) {
    return 0;
}

// configuration of the filesystem is provided by this struct
const struct lfs2_config cfg = {
    // block device operations
    .read  = block_read,
    .prog  = block_prog,
    .erase = block_erase,
    .sync  = block_sync,

    // block device configuration
    .read_size = 16,
    .prog_size = 16,
    .block_size = BLOCK_SIZE_BYTES,
    .block_count = BLOCK_COUNT,
    .cache_size = 16,
    .lookahead_size = 16,
    .block_cycles = -1,
};

void config_uart_to_upload_mode() {
    uart_driver_install(UART_NUM_0, 1024 * 10, 0, 0, NULL, 0);
    // uart_set_baudrate(UART_NUM_0, 921600); // Change baud to 921600
    uart_set_baudrate(UART_NUM_0, 115200); // Change baud to 115200
}

void config_uart_to_run_mode() {
    // uart_set_baudrate(UART_NUM_0, 115200); // Change baud to 115200
    // uart_enable_rx_intr(UART_NUM_0);
    uart_driver_delete(UART_NUM_0);
}

int uart_rx_bytes(uint8_t *buff, int len, int timeout) {
    return uart_read_bytes(UART_NUM_0, buff, len, timeout);
}

uint8_t uart_rx_byte() {
    static uint8_t c;
    uart_read_bytes(UART_NUM_0, &c, 1, portMAX_DELAY);
    return c;
}

void uart_rx_clear() {
    uart_flush(UART_NUM_0);
}

int uart_tx_byte(uint8_t c) {
    // return uart_write_bytes(UART_NUM_0, (const char *) &c, 1);
    uart_tx_one_char(c);
    return 1;
}

int uart_tx_str(char *c) {
    // return uart_write_bytes(UART_NUM_0, c, strlen(c));
    for (int i=0;i<strlen(c);i++)
        uart_tx_one_char(c[i]);
    return 1;
}

bool check_REQ_byte(int timeout) {
    uint64_t t0 = esp_timer_get_time();
    uint8_t state = 0;
    bool foundFlag = 0;
    while((esp_timer_get_time() - t0) < (timeout * 1000)) {
        uint8_t c = 0;
        if (uart_rx_bytes(&c, 1, 0) >= 1) {
            t0 = esp_timer_get_time();
            if (state == 0) {
                if (c == 0x1F) {
                    state = 1;
                }
            } else if (state == 1) {
                if (c == 0xF1) {
                    state = 2;
                } else {
                    state = 0;
                }
            } else if (state == 2) {
                if (c == 0xFF) {
                    foundFlag = true;
                    break;
                } else {
                    state = 0;
                }
            }
        } else {
            vTaskDelay(5 / portTICK_PERIOD_MS);
        }
    }
    return foundFlag;
}

void uploadFlow() {
    if (!check_REQ_byte(WAIT_BOOT_TIMEOUT)) { // Check receive REQ byte
        return;
    }

    part = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_ANY, "vfs");
    if (!part) {
        printf("ERROR: partition not found\n"); // ERROR byte
        return;
    }

    lfs2_t lfs;
    int err = lfs2_mount(&lfs, &cfg);
    if (err) {
        printf("ERROR: lfs2 mount fail, code: %d\n", err); // ERROR byte
        return;
    }
            
    uart_rx_clear();
    printf("upload mode\n");

    char *fPath = NULL;
    bool firstTimeFlag = true;

    for (;;) {
        uint8_t cmd = 0;
        int len = 0;

        len = uart_rx_bytes(&cmd, 1, 10 / portTICK_PERIOD_MS);
        if (len != 1) {
            continue;
        }

        if (cmd == 0xFF) { // exit
            printf("exit upload mode\n");
            break;
        } else if (cmd == 0x01) {
            printf("MicroPython " MICROPY_GIT_TAG " on " MICROPY_BUILD_DATE "; " MICROPY_HW_BOARD_NAME " with " MICROPY_HW_MCU_NAME "\n");
            continue;
        } else if (cmd == 0x12) { // close
            if (fPath) {
                free(fPath);
                fPath = NULL;
            }
            printf("close\n");
            continue;
        }

        if (cmd != 0x10 && cmd != 0x11) {
            printf("ERROR: CMD 0x%02x NOT SUPPORT\n", cmd);
            continue;
        }

        uint8_t dataLenBuff[2];
        len = uart_rx_bytes(dataLenBuff, 2, 100 / portTICK_PERIOD_MS);
        if (len != 2) {
            printf("ERROR: DATA LEN TIMEOUT\n");
            continue;
        }
        uint16_t dataLen = (((uint16_t)(dataLenBuff[0])) << 8) | dataLenBuff[1];

        uint8_t *dataBuffer = (uint8_t *)malloc(dataLen);
        len = uart_rx_bytes(dataBuffer, dataLen, 2000 / portTICK_PERIOD_MS);
        if (len != dataLen) {
            printf("ERROR: DATA TIMEOUT\n");
            continue;
        }

        uint8_t checkSUM = 0;
        len = uart_rx_bytes(&checkSUM, 1, 100 / portTICK_PERIOD_MS);
        if (len != 1) {
            printf("ERROR: CHECKSUM TIMEOUT\n");
            continue;
        }

        uint8_t dataSum = 0;
        for (uint16_t index=0;index<dataLen;index++) {
            dataSum += dataBuffer[index];
        }
        if (dataSum != checkSUM) {
            printf("ERROR: CHECKSUM ERROR, CALC %d BUT SEND %d\n", dataSum, checkSUM);
            continue;
        }

        if (cmd == 0x10) { // set path
            if (fPath) {
                free(fPath);
                fPath = NULL;
            }
            firstTimeFlag = true;

            fPath = (char*)malloc(dataLen + 1);
            memset(fPath, 0, dataLen + 1);
            memcpy(fPath, dataBuffer, dataLen);
            printf("set path to %s\n", fPath);
        } else if (cmd == 0x11) { // write content
            if (fPath) {
                printf("writing %d bytes\n", dataLen);

                lfs2_file_t file;
                lfs2_file_open(&lfs, &file, fPath, LFS2_O_RDWR | LFS2_O_CREAT | (firstTimeFlag ? LFS2_O_TRUNC : LFS2_O_APPEND));
                lfs2_file_write(&lfs, &file, dataBuffer, dataLen);
                lfs2_file_close(&lfs, &file);

                printf("write end\n");

                firstTimeFlag = false;
            } else {
                printf("ERROR: NOT SET PATH\n");
                continue;
            }
        }

        free(dataBuffer);
    }

    lfs2_unmount(&lfs);
}

void boot_upload_run() {
    config_uart_to_upload_mode();

    printf("wait upload\n");
    uploadFlow();
    printf("run main program\n");

    config_uart_to_run_mode();
}


