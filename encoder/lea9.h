#ifndef LEA9_H_INCLUDED
#define LEA9_H_INCLUDED

#include <cstdint>

void crypt(uint8_t* raw, uint8_t key[]);
void lock(uint8_t* raw, uint8_t key[], uint8_t* lfsr_state);
void unlock(uint8_t* raw, uint8_t key[], uint8_t* lfsr_state);
int modInverse(int t, int m) ;
long lfsr(uint8_t* lfsr_state);

#endif // LEA9_H_INCLUDED
