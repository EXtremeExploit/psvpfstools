#include <b64/cencode.h>
#include <zRIF/keyflate.h>
#include <zRIF/licdec.h>
#include <zRIF/rif.h>
#include <iostream>
#include <fstream>
#include <string>
#include "rif2zrif.h"

std::string rif2zrif(std::string& drmlicpath) {
    constexpr auto MAX_KEY_SIZE = 2048;
    constexpr auto MIN_KEY_SIZE = 512;
    std::streampos size;
    char key[MAX_KEY_SIZE];

    std::ifstream binfile(drmlicpath, std::ios::in | std::ios::binary | std::ios::ate);
    size = binfile.tellg();
    binfile.seekg(0, std::ios::beg);
    binfile.read(key, size);
    int len = size;
    unsigned char out[MAX_KEY_SIZE];
    memset(out, 0, MAX_KEY_SIZE);
    if ((len = deflateKey((unsigned char *)key, len, out, MAX_KEY_SIZE)) < 0) {
        std::cout << "Error: license failed to compress." << std::endl;
    } else {
        std::cout << "Compressed key to " << len << " bytes." << std::endl;
        //bypass b64 padding
        if ((len % 3) > 0)
            len += 3 - (len % 3);

        memset(key, 0, MAX_KEY_SIZE);
        base64_encodestate state;
        base64_init_encodestate(&state);
        int enc_len = base64_encode_block((char *)out, len, key, &state);
        enc_len += base64_encode_blockend(key + enc_len, &state);

        std::cout << "rif2zrif sanity check: " << key << std::endl;

    }

	return key;

    binfile.close();
}


unsigned char* get_temp_klicensee(std::string& zrif) {
    unsigned char temp_klicensee[0x10] = { 0 };
    std::shared_ptr<SceNpDrmLicense> slic = decode_license_np(zrif);
    memcpy(temp_klicensee, slic->key, 0x10);

	return temp_klicensee;
}