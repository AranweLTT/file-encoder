// System libraries
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <cstdlib>

// User libraries
#include "lea9.h"

using namespace std;


void parseHexString(const string& hexString, uint8_t key[9]){
	for (uint8_t i = 0; i < 9; i++) {
		string byteString = hexString.substr(i*2, 2);
		uint8_t byte = (uint8_t) strtol(byteString.c_str(), NULL, 16);
		key[i] = byte;
	}
}

int main(int argc, char* argv[]){
    // Local variables
    uint8_t buffer[3];
    uint8_t key[9];
    uint8_t lfsr_state[16];


    // Init lfsr
    for (uint8_t i = 16; i > 0; i--) lfsr_state[i-1] = 0;
    lfsr_state[0] = 0B00000001;
    lfsr_state[15] = 0B10000000;
    for(uint16_t i = 0; i < 1000; i++) lfsr(lfsr_state);


	// Get arguments
	if(argc != 5){
		cerr << "arguments: <input file> <output file> <mode> <key>" << endl;
		return -1;
	}
	
	string filename1 = argv[1]; 	// input file
	string filename2 = argv[2]; 	// output file
	uint8_t mode = atoi(argv[3]); 	// mode
	string hexString = argv[4]; 	// key
	
	if (mode > 3) {
		cerr << "Available modes are [0, 1, 2]" << endl;
		return -1;
	}
	if (hexString.length() != 18) {
		cerr << "Hex key must be exactly 18 characters long." << endl;
		return -1;
	} else {
		parseHexString(hexString, key);
	}


	// Open files
	ifstream f1 (filename1, ios::in | ios::binary);
	ofstream f2 (filename2, ios::out | ios::binary);


	// Run encryption
	if(f1 && f2){
		f1.seekg(0,f1.end);
		uint64_t file_size = f1.tellg();
		f1.seekg(0,f1.beg);
		cout << "File is " << file_size << " bytes" << endl;

		// Encrypt
		uint64_t max_index = (file_size - (file_size % 3))/3;
		cout << "Processing file..." << endl;
		for(uint64_t i = 0; i < max_index; i++){
			if(f1 && f1.read((char*)buffer,3)){
				if(mode == 0)
					crypt(buffer, key);
				else if(mode == 1)
					lock(buffer, key, lfsr_state);
				else if (mode == 2)
					unlock(buffer, key, lfsr_state);
				f2.write((char*)buffer,3);
			}else{
				cerr << "Error writing output file" << endl;
				return -1;
			}
		}

		// Write last bytes
		for(uint64_t i = 0; i < (file_size%3); i++){
			if(f1 && f1.read((char*)buffer,1)){
				f2.write((char*)buffer,1);
			}else{
				cerr << "Error writing output file" << endl;
				return -1;
			}
		}

		// Closing files
		f1.close();
		f2.close();
		cout << "Done." << endl;
	} else {
		cerr << "Error opening file" << endl;
		return -1;
	}


    // Exit success
    return (0);
}
