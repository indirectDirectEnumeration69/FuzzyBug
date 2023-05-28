#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/bio.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <fstream>
#include <sstream>

RSA* createRSAKeyPair(int bits) {
    RSA* rsa = RSA_new();
    BIGNUM* bn = BN_new();
    BN_set_word(bn, RSA_F4);

    if (RSA_generate_key_ex(rsa, bits, bn, NULL) != 1) {
        RSA_free(rsa);
        BN_free(bn);
        return NULL;
    }

    BN_free(bn);
    return rsa;
}

void writeRSAKeyToFile(const char* filename, RSA* rsa, bool isPublic) {
    BIO* bio = BIO_new_file(filename, "w+");

    if (isPublic) {
        PEM_write_bio_RSAPublicKey(bio, rsa);
    }
    else {
        PEM_write_bio_RSAPrivateKey(bio, rsa, NULL, NULL, 0, NULL, NULL);
    }

    BIO_free(bio);
}

void generateRSAKeyPair(const char* publicFilename, const char* privateFilename, int bits = 2048) {
    RSA* rsa = createRSAKeyPair(bits);

    if (rsa != NULL) {
        writeRSAKeyToFile(publicFilename, rsa, true);
        writeRSAKeyToFile(privateFilename, rsa, false);
        RSA_free(rsa);
    }
}

std::string readKeyFromFile(const char* filename) {
    std::ifstream file(filename);
    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();
}
