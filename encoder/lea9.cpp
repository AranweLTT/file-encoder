//------------------//
//---[ INCLUDES ]---//
//------------------//
//#include <cstdint>
#include <iostream>
#include "lea9.h"

using namespace std;

#define INDEX(x, y) ((x*N_LEA)+y)
#define N_LEA   3
#define true    1
#define false   0

void crypt(uint8_t* raw, uint8_t key[])
{
    uint32_t buffer[3];

    buffer[0] = (uint32_t)(key[0]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[1]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[2]) * (uint32_t)(raw[2]);
    buffer[1] = (uint32_t)(key[3]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[4]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[5]) * (uint32_t)(raw[2]);
    buffer[2] = (uint32_t)(key[6]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[7]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[8]) * (uint32_t)(raw[2]);

    // Copy output
    raw[0] = (uint8_t)(buffer[0] % 256);
    raw[1] = (uint8_t)(buffer[1] % 256);
    raw[2] = (uint8_t)(buffer[2] % 256);
}

void lock(uint8_t* raw, uint8_t key[], uint8_t* lfsr_state)
{
    uint32_t buffer[3];
    uint32_t sub;
    uint8_t state = 0;

    for(uint8_t r = 0; r < 3; r++)
    {
        while(modInverse(state, 256) == 0) state = lfsr(lfsr_state);
        sub = (uint32_t)(raw[r]) * (uint32_t)(state%256);
        raw[r] = (uint8_t)(sub%256);
    }

    buffer[0] = (uint32_t)(key[0]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[1]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[2]) * (uint32_t)(raw[2]);
    buffer[1] = (uint32_t)(key[3]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[4]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[5]) * (uint32_t)(raw[2]);
    buffer[2] = (uint32_t)(key[6]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[7]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[8]) * (uint32_t)(raw[2]);

    // Copy output
    raw[0] = (uint8_t)(buffer[0] % 256);
    raw[1] = (uint8_t)(buffer[1] % 256);
    raw[2] = (uint8_t)(buffer[2] % 256);
}

void unlock(uint8_t* raw, uint8_t key[], uint8_t* lfsr_state)
{
    uint32_t buffer[3];
    uint32_t sub;
    uint8_t state = 0;

    for(uint8_t r = 0; r < 3; r++)
    {
        while(modInverse(state, 256) == 0) state = lfsr(lfsr_state);
        sub = (uint32_t)(raw[r]) * (uint32_t)(modInverse(state, 256)%256);
        raw[r] = (uint8_t)(sub%256);
    }

    buffer[0] = (uint32_t)(key[0]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[1]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[2]) * (uint32_t)(raw[2]);
    buffer[1] = (uint32_t)(key[3]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[4]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[5]) * (uint32_t)(raw[2]);
    buffer[2] = (uint32_t)(key[6]) * (uint32_t)(raw[0]) +
                (uint32_t)(key[7]) * (uint32_t)(raw[1]) +
                (uint32_t)(key[8]) * (uint32_t)(raw[2]);

    // Copy output
    raw[0] = (uint8_t)(buffer[0] % 256);
    raw[1] = (uint8_t)(buffer[1] % 256);
    raw[2] = (uint8_t)(buffer[2] % 256);
}

int modInverse(int t, int m)
{
    int index = 0 ;
    t = t % m;
    for (int x = 1; x < m; x++)
        if (((t * x) % m == 1) && index == 0)
            index = x ;
    return index ;
}

long lfsr(uint8_t* lfsr_state)
{
    // Local variables
    static long result = 0;
    uint8_t newbit, carry, mem;

    for (uint8_t counter = 0; counter < 32; counter++)
    {
        newbit = (lfsr_state[0] ^ (lfsr_state[0] >> 1) ^ (lfsr_state[0] >> 2) ^ (lfsr_state[0] >> 7)) & 1;
        carry = 0;
        mem = newbit;

        // Shift register
        for (uint8_t i = 16; i > 0; i--)
        {
            carry = lfsr_state[i-1] & 0B00000001;
            lfsr_state[i-1] = lfsr_state[i-1] >> 1;
            lfsr_state[i-1] = lfsr_state[i-1] | (mem << 7);
            mem = carry;
        }
    }

    // Get output
    result = (long)lfsr_state[0] + ((long)lfsr_state[1] << 8);
    return result;
}
