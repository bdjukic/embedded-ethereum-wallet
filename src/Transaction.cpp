//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include "Transaction.h"

#include "./helpers/ByteConverter.h"
#include "./helpers/CryptoHelper.h"
#include "./helpers/RlpEncoder.h"

#include "./crypto/secp256k1_recovery.h"
#include "./crypto/secp256k1_c.h"
#include "./crypto/main_impl.h"

#include <vector>
#include <cstdlib>
#include <stdarg.h>

#define SIGNATURE_LENGTH 64

static secp256k1_context *ctx;

Transaction::Transaction() {
    options.gas=0;
    strcpy(options.from,"");
    strcpy(options.to,"");
    strcpy(options.gasPrice,"0");

    ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN);
}

void Transaction::setPrivateKey(const uint8_t *key) {
    privateKey = key;
}

String Transaction::getRaw(uint32_t nonceVal, uint32_t gasPriceVal, uint32_t  gasLimitVal,
                                 string *toStr, string *valueStr, string *dataStr, uint32_t chainId) {
    uint8_t signature[SIGNATURE_LENGTH];
    memset(signature, 0, SIGNATURE_LENGTH);
    int recoveryId[1] = {0};

    GenerateSignature(signature, recoveryId, nonceVal, gasPriceVal, gasLimitVal, toStr, valueStr, dataStr, chainId);

    vector<uint8_t>param = RlpEncodeForRawTransaction(nonceVal,
                                                      gasPriceVal,
                                                      gasLimitVal,
                                                      toStr,
                                                      valueStr,
                                                      dataStr,
                                                      chainId,
                                                      signature,
                                                      recoveryId[0]);

    return ByteConverter::bytesToString(param);
}

void Transaction::GenerateSignature(uint8_t* signature,
                                 int* recoveryId,
                                 uint32_t nonceVal,
                                 uint32_t gasPriceVal,
                                 uint32_t gasLimitVal,
                                 string* toStr,
                                 string* valueStr,
                                 string* dataStr,
                                 uint32_t chainId) {
    // Encode
    vector<uint8_t> rlpEncoded = RlpEncode(nonceVal, gasPriceVal, gasLimitVal, toStr, valueStr, dataStr, chainId);

    // Hash
    vector<uint8_t> transactionHash = CryptoHelper::keccak256(&rlpEncoded[0], rlpEncoded.size());

    // Sign
    Sign(&transactionHash[0], signature, recoveryId);
}

string Transaction::GenerateBytesForUint(const uint32_t value) {
    char output[70];
    memset(output, 0, sizeof(output));

    // Check number of digits
    int digits = String::format("%x", (uint32_t)value).length();

    // Fill 0 and copy number to string
    for(int i = 2; i < 2 + 64 - digits; i++){
        String::format("%s%s", output, "0").toCharArray(output, 70);
    }

    String::format("%s%x", output, (uint32_t)value).toCharArray(output, 70);

    return string(output);
}

string Transaction::GenerateBytesForInt(const int32_t value) {
    char output[70];
    memset(output, 0, sizeof(output));

    // Check number of digits
    int digits = String::format("%x", (uint32_t)value).length();

    // Fill 0 and copy number to string
    char fill[2];
    if (value >= 0) {
        String::format("%s","0").toCharArray(fill, 2);
    } else {
        String::format("%s","f").toCharArray(fill, 2);
    }
    for(int i = 2; i < 2+64-digits; i++){
      String::format("%s%s", output, fill).toCharArray(output, 70);
    }
    String::format("%s%x", output, (uint32_t)value).toCharArray(output, 70);

    return string(output);
}

string Transaction::GenerateBytesForAddress(const string* value) {
    size_t digits = value->size() - 2;

    string zeros = "";
    for(int i = 2; i < 2+64-digits; i++){
        zeros = zeros + "0";
    }
    string tmp = string(*value);
    tmp.erase(tmp.begin(), tmp.begin() + 2);
    return zeros + tmp;
}

string Transaction::GenerateBytesForString(const string* value) {
    string zeros = "";
    size_t remain = 32 - ((value->size()-2) % 32);
    for(int i = 0; i < remain + 32; i++){
        zeros = zeros + "0";
    }

    return *value + zeros;
}

string Transaction::GenerateBytesForBytes(const char* value, const int len) {
    char output[70];
    memset(output, 0, sizeof(output));

    for(int i = 0; i < len; i++){
      String::format("%s%x", output, value[i]).toCharArray(output, 70);
    }
    size_t remain = 32 - ((strlen(output)-2) % 32);
    for(int i = 0; i < remain + 32; i++){
      String::format("%s%s", output, "0").toCharArray(output, 70);
    }

    return string(output);
}

vector<uint8_t> Transaction::RlpEncode(uint32_t nonceVal, uint32_t gasPriceVal, uint32_t  gasLimitVal,
                                    string* toStr, string* valueStr, string* dataStr, uint32_t chainIdStr) {
    vector<uint8_t> nonce = ByteConverter::numberToBytes(nonceVal);
    vector<uint8_t> gasPrice = ByteConverter::numberToBytes(gasPriceVal);
    vector<uint8_t> gasLimit = ByteConverter::numberToBytes(gasLimitVal);
    vector<uint8_t> to = ByteConverter::stringToBytes(toStr);
    vector<uint8_t> value = ByteConverter::stringToBytes(valueStr);
    vector<uint8_t> data = ByteConverter::stringToBytes(dataStr);

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(data);

    vector<uint8_t> encoded = RlpEncoder::encodeWholeHeaderWithVector(
      outputNonce.size() +
      outputGasPrice.size() +
      outputGasLimit.size() +
      outputTo.size() +
      outputValue.size() +
      outputData.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());

    return encoded;
}

void Transaction::Sign(uint8_t* hash, uint8_t* sig, int* recid) {
  secp256k1_nonce_function noncefn = secp256k1_nonce_function_rfc6979;
  void* data_ = NULL;

  secp256k1_ecdsa_recoverable_signature signature;
  memset(&signature, 0, sizeof(signature));

  if (secp256k1_ecdsa_sign_recoverable(ctx, &signature, hash,  privateKey, noncefn, data_) == 0) {
      return;
  }

  secp256k1_ecdsa_recoverable_signature_serialize_compact(ctx, &sig[0], &recid[0], &signature);
}

vector<uint8_t> Transaction::RlpEncodeForRawTransaction(
                             uint32_t nonceVal, uint32_t gasPriceVal, uint32_t gasLimitVal,
                             string* toStr, string* valueStr, string* dataStr, uint32_t chainIdStr, uint8_t* sig, uint8_t recoveryId) {

    vector<uint8_t> signature;
    for (int i = 0; i < SIGNATURE_LENGTH; i++) {
        signature.push_back(sig[i]);
    }

    vector<uint8_t> nonce = ByteConverter::numberToBytes(nonceVal);
    vector<uint8_t> gasPrice = ByteConverter::numberToBytes(gasPriceVal);
    vector<uint8_t> gasLimit = ByteConverter::numberToBytes(gasLimitVal);
    vector<uint8_t> to = ByteConverter::stringToBytes(toStr);
    vector<uint8_t> value = ByteConverter::stringToBytes(valueStr);
    vector<uint8_t> data = ByteConverter::stringToBytes(dataStr);
    vector<uint8_t> chainId = ByteConverter::numberToBytes(chainIdStr);

    vector<uint8_t> outputNonce = RlpEncoder::encodeItemWithVector(nonce);
    vector<uint8_t> outputGasPrice = RlpEncoder::encodeItemWithVector(gasPrice);
    vector<uint8_t> outputGasLimit = RlpEncoder::encodeItemWithVector(gasLimit);
    vector<uint8_t> outputTo = RlpEncoder::encodeItemWithVector(to);
    vector<uint8_t> outputValue = RlpEncoder::encodeItemWithVector(value);
    vector<uint8_t> outputData = RlpEncoder::encodeItemWithVector(data);

    vector<uint8_t> R;
    R.insert(R.end(), signature.begin(), signature.begin() + (SIGNATURE_LENGTH / 2));
    vector<uint8_t> S;
    S.insert(S.end(), signature.begin() + (SIGNATURE_LENGTH / 2), signature.end());
    vector<uint8_t> V;
    V.push_back((uint8_t)(recoveryId + 27));

    vector<uint8_t> outputR = RlpEncoder::encodeItemWithVector(R);
    vector<uint8_t> outputS = RlpEncoder::encodeItemWithVector(S);
    vector<uint8_t> outputV = RlpEncoder::encodeItemWithVector(V);

    vector<uint8_t> encoded = RlpEncoder::encodeWholeHeaderWithVector(
                                        outputNonce.size() +
                                        outputGasPrice.size() +
                                        outputGasLimit.size() +
                                        outputTo.size() +
                                        outputValue.size() +
                                        outputData.size() +
                                        outputV.size() +
                                        outputR.size() +
                                        outputS.size());

    encoded.insert(encoded.end(), outputNonce.begin(), outputNonce.end());
    encoded.insert(encoded.end(), outputGasPrice.begin(), outputGasPrice.end());
    encoded.insert(encoded.end(), outputGasLimit.begin(), outputGasLimit.end());
    encoded.insert(encoded.end(), outputTo.begin(), outputTo.end());
    encoded.insert(encoded.end(), outputValue.begin(), outputValue.end());
    encoded.insert(encoded.end(), outputData.begin(), outputData.end());

    encoded.insert(encoded.end(), outputV.begin(), outputV.end());
    encoded.insert(encoded.end(), outputR.begin(), outputR.end());
    encoded.insert(encoded.end(), outputS.begin(), outputS.end());

    return encoded;
}
