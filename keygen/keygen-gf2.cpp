#include <iostream>
#include <iomanip>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>
#include <cstring>
#include <stdexcept>
#include <stdint.h>

using namespace std;

// Hexspeak dictionary
vector<string> hexspeak = {"cafe", "deadbeef", "decafbad", "deafbabe", "cafebabe"};

// ----------------------
// GF(2) helpers (8x8)
// ----------------------
void byte_to_vec(uint8_t b, uint8_t *v) {
    for (int i = 0; i < 8; i++) {
        v[i] = (b >> (7 - i)) & 1;
    }
}

uint8_t vec_to_byte(const uint8_t *v) {
    uint8_t val = 0;
    for (int i = 0; i < 8; i++) {
        val = (val << 1) | (v[i] & 1);
    }
    return val;
}

void mat_mult(const uint8_t (*A)[8], const uint8_t (*B)[8], uint8_t (*C)[8]) {
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            int sum = 0;
            for (int k = 0; k < 8; k++) {
                sum ^= (A[i][k] & B[k][j]);
            }
            C[i][j] = sum & 1;
        }
    }
}

void mat_mult_vec(const uint8_t (*A)[8], const uint8_t *v, uint8_t *out) {
    for (int i = 0; i < 8; i++) {
        int sum = 0;
        for (int k = 0; k < 8; k++) {
            sum ^= (A[i][k] & v[k]);
        }
        out[i] = sum & 1;
    }
}


// ----------------------
// Gauss-Jordan inverse over GF(2)
// ----------------------
bool gf2_inv(const uint8_t (*A)[8], uint8_t (*invA)[8]) {
    uint8_t aug[8][16];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            aug[i][j] = A[i][j];
            aug[i][j+8] = (i == j) ? 1 : 0;
        }
    }
    int row = 0;
    for (int col = 0; col < 8; col++) {
        int pivot = -1;
        for (int r = row; r < 8; r++) {
            if (aug[r][col] == 1) {
                pivot = r;
                break;
            }
        }
        if (pivot == -1) return false; // not invertible
        if (pivot != row) {
            for (int c = 0; c < 16; c++)
                swap(aug[pivot][c], aug[row][c]);
        }
        for (int r = 0; r < 8; r++) {
            if (r != row && aug[r][col] == 1) {
                for (int c = 0; c < 16; c++)
                    aug[r][c] ^= aug[row][c];
            }
        }
        row++;
    }
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            invA[i][j] = aug[i][j+8];
    return true;
}

bool is_invertible(const uint8_t (*A)[8]) {
    uint8_t temp[8][8];
    return gf2_inv(A, temp);
}

// ----------------------
// Matrix/byte conversion
// ----------------------
string mat_to_hex(const uint8_t (*M)[8]) {
    stringstream ss;
    ss << hex << setfill('0');
    for (int r = 0; r < 8; r++) {
        uint8_t val = 0;
        for (int bit = 0; bit < 8; bit++) {
            val = (val << 1) | (M[r][bit] & 1);
        }
        ss << setw(2) << (int)val;
    }
    return ss.str();
}

bool hex_to_mat(const string &h, uint8_t (*M)[8]) {
    if (h.size() != 16) return false;
    for (int i = 0; i < 8; i++) {
        string byteStr = h.substr(i*2, 2);
        uint8_t byteVal = (uint8_t) strtol(byteStr.c_str(), nullptr, 16);
        for (int bit = 0; bit < 8; bit++) {
            M[i][bit] = (byteVal >> (7 - bit)) & 1;
        }
    }
    return true;
}

// ----------------------
// Key generation
// ----------------------
void random_matrix(uint8_t (*M)[8]) {
    for (int i = 0; i < 8; i++)
        for (int j = 0; j < 8; j++)
            M[i][j] = rand() % 2;
}

void random_invertible_matrix(uint8_t (*M)[8]) {
    do {
        random_matrix(M);
    } while (!is_invertible(M));
}

void fixed_G(uint8_t (*G)[8], uint8_t (*G_inv)[8]) {
    uint8_t temp[8][8] = {
        {1,0,1,0,0,1,0,1},
        {0,1,1,0,1,0,1,0},
        {1,1,0,1,0,0,1,0},
        {0,0,1,1,1,0,0,1},
        {1,0,0,1,0,0,1,0},
        {0,1,0,0,1,1,0,1},
        {1,0,1,0,1,0,1,0},
        {0,1,0,1,0,1,0,1}
    };
    memcpy(G, temp, sizeof(temp));
    if (!gf2_inv(G, G_inv)) {
        throw runtime_error("Chosen G not invertible");
    }
}

void generate_keypair(uint8_t (*G)[8], const uint8_t (*G_inv)[8],
                      string &pubhex, string &privhex) {
    uint8_t S[8][8], S_inv[8][8], temp[8][8], P[8][8];
    random_invertible_matrix(S);
    gf2_inv(S, S_inv);
    mat_mult(S, G, temp);
    mat_mult(temp, S_inv, P);
    pubhex = mat_to_hex(P);
    privhex = mat_to_hex(S);
}

// ----------------------
// Encrypt / Decrypt
// ----------------------
uint8_t encrypt_byte(const string &pubhex, uint8_t pt) {
    uint8_t P[8][8], pvec[8], cvec[8];
    hex_to_mat(pubhex, P);
    byte_to_vec(pt, pvec);
    mat_mult_vec(P, pvec, cvec);
    return vec_to_byte(cvec);
}

uint8_t decrypt_byte(const string &privhex, const uint8_t (*G_inv)[8], uint8_t ct) {
    uint8_t S[8][8], S_inv[8][8], cvec[8], y[8], u[8], pvec[8];
    hex_to_mat(privhex, S);
    gf2_inv(S, S_inv);
    byte_to_vec(ct, cvec);
    mat_mult_vec(S_inv, cvec, y);
    mat_mult_vec(G_inv, y, u);
    mat_mult_vec(S, u, pvec);
    return vec_to_byte(pvec);
}

// ----------------------
// Vanity search
// ----------------------
bool starts_with(const string &s, const string &prefix) {
    return s.rfind(prefix, 0) == 0;
}

void find_vanity_key(uint8_t (*G)[8], const uint8_t (*G_inv)[8],
    string &pubhex, string &privhex) {
    while(1){
        generate_keypair(G, G_inv, pubhex, privhex);
        for (const auto &substring : hexspeak) {
            if (pubhex.rfind(substring, 0) == 0) return;
        }
    }
}

// ----------------------
// Main
// ----------------------
int main(int argc, char* argv[]) {
    srand(time(NULL));

    string pubhex, privhex;
    uint8_t G[8][8], G_inv[8][8];
    fixed_G(G, G_inv);

    cout << "Searching for vanity key from hexspeak dictionary..." << endl;
    cout << "Output format: <private-key>,<public-key>" << endl << endl;

    while(1){
        find_vanity_key(G, G_inv, pubhex, privhex);

        // quick encrypt/decrypt test
        bool valid = true;
        uint8_t ct, dec;
        uint8_t data[3] = {0x73, 0x4f, 0xe0};

        for (int i = 0; i < 3; ++i) {
            ct = encrypt_byte(pubhex, data[i]);
            dec = decrypt_byte(privhex, G_inv, ct);
            if (dec != data[i]) valid = false;
        }

        if(valid) cout << pubhex << "," << privhex << endl;
    }
    
    return 0;
}
