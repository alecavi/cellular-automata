#include <stdbool.h>

int strToBinArr(char *string, bool* out, int length);
void binArrToStr(bool *arr, char *out, int length);
int binToDec(bool *bin, int length);
void decToBin(int dec, bool* out);

int strToBinArr(char *string, bool* out, int length) {
    for(int i = 0; i < length; ++i) {
        switch (string[i]) {
            case '0':
                out[i] = false;
                break;
            case '1':
                out[i] = true;
                break;
            default:
                return 1;
        }
    }
    return 0;
}

void binArrToStr(bool *arr, char *out, int length) {
    for(int i = 0; i < length; ++i) {
        out[i] = arr[i] ? '1' : '0';
    }
}

int binToDec(bool *bin, int length) {
    int out = 0;
    for(int i = 0; i < length; ++i) {
        if(bin[i]) out |= 1 << i;
    }
    return out;
}

void decToBin(int dec, bool* out) {
    int firstBit = 1 << (sizeof(int) * 8 - 1);

    for(int i = 0; i < (int) sizeof(int) * 8; ++i) {
        out[i] = dec & (firstBit >> i );
    }
}
