//
// AnyLedger - Created by Bogdan Djukic on 11-04-2018. Inspired by Okada Takahiro, https://github.com/kopanitsa/web3-arduino.
// Licensed under GNU GENERAL PUBLIC LICENSE.
//

#include <string>
#include <cstring>
#include <stdint.h>
#include <vector>
#include "spark_wiring_string.h"

using namespace std;

class ByteConverter {
  public:
    static vector<uint8_t> numberToBytes(uint32_t val);
    static vector<uint8_t> charStrToBytes(const uint8_t *in);
    static vector<uint8_t> stringToBytes(const string* str);
    static String bytesToString(const vector<uint8_t> buf);
};
