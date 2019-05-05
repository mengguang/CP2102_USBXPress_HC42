#include <cstdio>
#include "windows.h"
#include "SiUSBXp.h"
#include <cstdint>
#include <cstdbool>

DWORD device = 0;
HANDLE m_hUSBDevice;
char hc42_buffer[64];

void HAL_Delay(uint32_t ms) {
    Sleep(ms);
}

uint32_t HAL_GetTick() {
    return (uint32_t) GetTickCount();
}

bool hc42_read_and_print_string() {
    SI_STATUS status;

    memset(hc42_buffer, 0, sizeof(hc42_buffer));
    DWORD nread = 0;
    status = SI_Read(m_hUSBDevice, hc42_buffer, sizeof(hc42_buffer), &nread);

    if (status != SI_SUCCESS ) {
        if(status == SI_READ_TIMED_OUT) {
            printf("no data available.\n");
            return true;
        } else {
            printf("SI_Read error: %d\n", status);
            return false;
        }
    } else {
        printf("hc42 read: %d\t%s\n", nread, hc42_buffer);
        return true;
    }
}

bool hc42_read_command(uint32_t timeout = 10000) {
    SI_STATUS status;
    DWORD nread = 0;
    uint32_t start;
    start = HAL_GetTick();
    memset(hc42_buffer, 0, sizeof(hc42_buffer));
    //make some special char as command end mark.
    while(hc42_buffer[strlen(hc42_buffer)-1] != 'x') {
        status = SI_Read(m_hUSBDevice, hc42_buffer + strlen(hc42_buffer), sizeof(hc42_buffer) - strlen(hc42_buffer), &nread);
        if (status != SI_SUCCESS ) {
            if(status == SI_READ_TIMED_OUT) {
                printf("no data available.\n");
            } else {
                printf("SI_Read error: %d\n", status);
                return false;
            }
        } else {
            printf("hc42 read: %d\t%s\n", nread, hc42_buffer);
        }
        if(HAL_GetTick() - start >= timeout) {
            printf("hc42_read_command timeout.\n");
            return false;
        }
    }
    return true;
}


int main() {

    DWORD dwNumDevices = 0;
    SI_STATUS status;
    SI_DEVICE_STRING devStr;
    status = SI_GetNumDevices(&dwNumDevices);
    if (status != SI_SUCCESS) {
        printf("SI_GetNumDevices error: %d\n", status);
        return status;
    }

    if (dwNumDevices == 0) {
        printf("No device found.\n");
        return 0;
    }
    for (DWORD d = 0; d < dwNumDevices; d++) {
        status = SI_GetProductString(d, devStr, SI_RETURN_SERIAL_NUMBER);

        if (status == SI_SUCCESS) {
            printf("Found device: %d : %s\n", d, devStr);
        }
    }


    status = SI_Open(device, &m_hUSBDevice);
    if (status != SI_SUCCESS) {
        printf("SI_Open error: %d\n", status);
        return status;
    }
    printf("Open device %d success.\n",device);

    status = SI_FlushBuffers(m_hUSBDevice, TRUE, TRUE);
    if (status != SI_SUCCESS) {
        printf("SI_FlushBuffers error: %d\n", status);
        return status;
    }

    status = SI_SetBaudRate(m_hUSBDevice, 9600);
    if (status != SI_SUCCESS) {
        printf("SI_SetBaudRate error: %d\n", status);
        return status;
    }

    status = SI_SetTimeouts(5000,5000);
    if (status != SI_SUCCESS) {
        printf("SI_SetTimeouts error: %d\n", status);
        return status;
    }

    while(true) {
        hc42_read_command();
        if(strncmp(hc42_buffer,"exit", 4) == 0) {
            break;
        }
    }

    status = SI_Close(m_hUSBDevice);
    if (status != SI_SUCCESS) {
        printf("SI_Close error: %d\n", status);
        return status;
    }
    return 0;
}