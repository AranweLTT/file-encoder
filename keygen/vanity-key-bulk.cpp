#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <random>

using namespace std;

// Hexspeak dictionary
vector<string> hexspeak = {"deadbeef", "decafbad", "deafbabe", "cafebabe"};


// Function for extended Euclidean Algorithm
// from [https://www.geeksforgeeks.org/multiplicative-inverse-under-modulo-m/]
int gcdExtended(int a, int b, int* x, int* y);


// Function to find modulo inverse of a
int mod_inverse_euclid(int A, int M){
    int x, y;
    int g = gcdExtended(A, M, &x, &y);
    if (g != 1) return -1;

    int res = (x % M + M) % M;
    return res;
}


// Function for extended Euclidean Algorithm
int gcdExtended(int a, int b, int* x, int* y){
    // Base Case
    if (a == 0) {
        *x = 0, *y = 1;
        return b;
    }

    // To store results of recursive call
    int x1, y1;
    int gcd = gcdExtended(b % a, a, &x1, &y1);

    // Update x and y using results of recursive call
    *x = y1 - (b / a) * x1;
    *y = x1;

    return gcd;
}


// Function to compute modular inverse (Brute-force method)
int mod_inverse(int A, int M) {
    for (int X = 1; X < M; X++) {
        if (((A % M) * (X % M)) % M == 1) return X;
    }
    return -1; // No inverse exists
}


// Compute determinant of a 3x3 matrix
int determinant(int matrix[3][3]) {
    return matrix[0][0] * (matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1]) -
           matrix[0][1] * (matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[2][0]) +
           matrix[0][2] * (matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0]);
}


// Compute inverse of a 3x3 matrix modulo 256
bool inverse_matrix(int input[3][3], int output[3][3], int mod) {
    int det = determinant(input);
    int det_inv = mod_inverse_euclid(det, mod);
    if (det_inv == -1) return false; // No inverse exists

    // Compute adjugate (cofactor) matrix
    int adj[3][3] = {
        {(input[1][1] * input[2][2] - input[1][2] * input[2][1]), -(input[0][1] * input[2][2] - input[0][2] * input[2][1]), (input[0][1] * input[1][2] - input[0][2] * input[1][1])},
        {-(input[1][0] * input[2][2] - input[1][2] * input[2][0]), (input[0][0] * input[2][2] - input[0][2] * input[2][0]), -(input[0][0] * input[1][2] - input[0][2] * input[1][0])},
        {(input[1][0] * input[2][1] - input[1][1] * input[2][0]), -(input[0][0] * input[2][1] - input[0][1] * input[2][0]), (input[0][0] * input[1][1] - input[0][1] * input[1][0])}
    };

    // Compute modular inverse matrix
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            output[i][j] = ((adj[i][j] * det_inv) % mod + mod) % mod;
        }
    }
    return true;
}


// Convert matrix to hex string
string keytohex(int key[3][3]) {
    stringstream ss;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            ss << hex << setw(2) << setfill('0') << key[i][j];
        }
    }
    return ss.str();
}


// Generate a random 3x3 matrix with values between 1 and 255
void generate_random_matrix(int key[3][3], mt19937 &rng) {
    uniform_int_distribution<int> dist(1, 255);
    for (int i = 0; i < 3; ++i)
        for (int j = 0; j < 3; ++j)
            key[i][j] = dist(rng);
}


// Generate a valid key pair
void generate_key_pair(int key_a[3][3], int key_b[3][3], mt19937 &rng) {
    int detb, detb_inv;
    while (true) {
        generate_random_matrix(key_a, rng);
        if (inverse_matrix(key_a, key_b, 256)){
            detb = determinant(key_b);
            if(mod_inverse_euclid(detb, 256) == -1) return; // Found valid key pair
        }
    }
}


// Find a key pair that starts with a hexspeak string
void find_vanity_key_pair(int key_a[3][3], int key_b[3][3], mt19937 &rng) {
    while (true) {
        generate_key_pair(key_a, key_b, rng);
        string public_key_hex = keytohex(key_b);
        for (const auto &substring : hexspeak) {
            if (public_key_hex.rfind(substring, 0) == 0) { // Check if it starts with substring
                return;
            }
        }
    }
}


// Multiply a 3x3 matrix with a 3x1 vector modulo 256
void matrix_vector_multiply(int matrix[3][3], int vector[3], int result[3]) {
    for (int i = 0; i < 3; ++i) {
        result[i] = 0;
        for (int j = 0; j < 3; ++j) {
            result[i] = (result[i] + matrix[i][j] * vector[j]) % 256;
        }
    }
}


// Check if product of matrix is identity matrix
bool is_identity_matrix(int key_a[3][3], int key_b[3][3], int mod) {
    int result[3][3] = {0}; // Initialize result matrix

    // Matrix multiplication: result = (key_a * key_b) % mod
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            result[i][j] = 0;
            for (int k = 0; k < 3; ++k) {
                result[i][j] = (result[i][j] + key_a[i][k] * key_b[k][j]) % mod;
            }
        }
    }

    // Check if result is the identity matrix
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            if ((i == j && result[i][j] != 1) || (i != j && result[i][j] != 0)) {
                return false; // Not an identity matrix
            }
        }
    }
    return true; // It's an identity matrix
}


// Main function
int main() {
    random_device rd;
    mt19937 rng(rd());

    cout << "Searching for vanity key from hexspeak dictionary..." << endl;
    cout << "Output format: <private-key>,<public-key>" << endl << endl;
    while (true) {
        int key_a[3][3], key_b[3][3];
        find_vanity_key_pair(key_a, key_b, rng);

        // Testing key pair
        bool valid = true;
        int data[3];
        int encrypted[3], decrypted[3];

        valid = is_identity_matrix(key_a, key_b, 256);

        for (int test = 0; test < 1024; ++test) {
            for (int n = 0; n < 3; ++n) data[n]=rng() % 256;
            matrix_vector_multiply(key_a, data, encrypted);
            matrix_vector_multiply(key_b, encrypted, decrypted);

            // Check if decryption was successful
            for (int i = 0; i < 3; ++i) {
                if (data[i] != decrypted[i]) {
                    valid = false;
                    break;
                }
            }
            if(valid == false) break;
        }

        if (valid) cout << keytohex(key_a) << "," << keytohex(key_b) << endl;
    }

    return 0;
}
