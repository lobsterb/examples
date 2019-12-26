// examples.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include "headers/core_header.hpp"

#include "openssl/cbc_256.hpp"

int main()
{
    unsigned char key[EVP_MAX_KEY_LENGTH] = "hehehahei";
    unsigned char iv[EVP_MAX_IV_LENGTH] = { 0 };

    cipher_aes_cbc_256 cbc_256;

    std::string origin = "11112222333344445555666677778888";
    std::string cipher = cbc_256.cbc_256_string(key, iv, origin, 1);
    std::string plant = cbc_256.cbc_256_string(key, iv, cipher, 0);

    if (plant == origin)
    {
        std::cout << "ok" << std::endl;
    }

    bool ret = cbc_256.cbc_256_file(key, iv, "D:\\NSI_DriverInstall.log", "D:\\test_cbc_256", 1);
    ret = cbc_256.cbc_256_file(key, iv, "D:\\test_cbc_256", "D:\\test_cbc_256.log", 0);
    return 0;
}