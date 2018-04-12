//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <vector>
#include "spark_wiring_string.h"

using namespace std;

class Transaction {

public:
    typedef struct {
        char from[80];
        char to[80];
        char gasPrice[20];
        long gas;
    } Options;

    Options options;

public:
    Transaction();

    void setPrivateKey(const uint8_t *key);

    String getRaw(uint32_t nonceVal,
                  uint32_t gasPriceVal,
                  uint32_t  gasLimitVal,
                  string *toStr,
                  string *valueStr,
                  string *dataStr,
                  uint32_t chainId);

private:
    const uint8_t * privateKey;

private:
    string GenerateBytesForInt(const int32_t value);
    string GenerateBytesForUint(const uint32_t value);
    string GenerateBytesForAddress(const string *value);
    string GenerateBytesForString(const string *value);
    string GenerateBytesForBytes(const char* value, const int len);

    void Sign(uint8_t* hash, uint8_t* sig, int* recid);

    void GenerateSignature(uint8_t* signature,
                           int* recoveryId,
                           uint32_t nonceVal,
                           uint32_t gasPriceVal,
                           uint32_t  gasLimitVal,
                           string* toStr,
                           string* valueStr,
                           string* dataStr,
                           uint32_t chainId);

    vector<uint8_t> RlpEncode(uint32_t nonceVal,
                              uint32_t gasPriceVal,
                              uint32_t  gasLimitVal,
                              string* toStr,
                              string* valueStr,
                              string* dataStr,
                              uint32_t chainId);

    vector<uint8_t> RlpEncodeForRawTransaction(uint32_t nonceVal,
                                               uint32_t gasPriceVal,
                                               uint32_t  gasLimitVal,
                                               string* toStr,
                                               string* valueStr,
                                               string* dataStr,
                                               uint32_t chainId,
                                               uint8_t* sig,
                                               uint8_t recoveryId);
};
