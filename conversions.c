#include <stdbool.h>

int strToBinArr(char *string, bool* out, int length);
void binArrToStr(bool *arr, char *out, int length);
int binToDec(bool *bin, int length);
int decToBin(int dec, bool* out, int length);

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
    if(length < 1) return 0; //strtol returns 0 on error

    int out = 0;
    int first_bit = 1 << (length - 1);
    for(int i = 0; i < length; ++i) {
        if(bin[i]) out |= first_bit >> i; 
    }
    return out;
}

int decToBin(int dec, bool* out, int length) {
    if(length < 1) return 1; //but it's more common to return 1 on error

    int first_bit = 1 << (length - 1);
    for(int i = 0; i < length; ++i) {
       out[i] = dec & first_bit >> i;
    }
    return 0;
}
