/** Encryption using RSA algorithm
 *
 *  For more information, see http://www.trytoprogram.com/cpp-examples/cplusplus-program-encrypt-decrypt-string/
*/


#ifndef __BLACKBOX_H__
#define __BLACKBOX_H__

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <Arduino.h>
using namespace std;

class security {
    public:
        int pub_key = 7;                            // Public key
        int prv_key = 13;                           // Private key
        int t = (7 - 1)*(13 - 1);                   // Totient function
        int n = pub_key * prv_key;                  // Modulus of prime numbers
        int i, flag;
        long int e[200], d[200], temp[200], j;
        char en[200], m[200];

    /**
     * @brief Function to check for prime number
    */
    private: int prime(int pr){
        j = sqrt(pr);
        for (int l = 2; l <= j; l++){
            if (pr % l == 0){
                return 0; // false
            }
        }
        return 1; // true
    }
    private: long int cd(long int a){
        long int k = 1;
        while (1){
            k += t;
            if (k % a == 0){
                return (k/a);
            }
        }
    }

    public:
        /**
         * @brief Function to generate encryption key 
        */
        void encryption_key(){
            int k = 0;
            for (i = 2; i < t; i++){
                if (t % i == 0){
                    continue; //Eksekveres 3 gange
                }
                flag = prime(i);
                if (flag == 1 && i != pub_key && i != prv_key){
                flag = prime(i);
                    e[k] = i;
                    flag = cd(e[k]);
                    if (flag > 0){
                        d[k] = flag;
                        k++;
                    }
                    if (k == 99){
                        break;
                    }
                }
            }
        }

        char* encrypt(char *message){
            long int pt, ct, key = e[0], k, len;
            i = 0;
            len = strlen(message);

            while(i != len){
                pt = message[i];
                pt -= 96;
                k = 1;
                for (j = 0; j < key; j++){
                    k = k * pt;
                    k = k % n;
                }
                temp[i] = k;
                ct = k + 96;
                en[i] = ct;
                i++;
            }
            en[i] = -1;
            // Encrypted message
            for (i = 0; en[i] != -1; i++){
                message[i] = en[i];
            }
            return message;
        }

        char* decrypt(char* message){
            long int pt, ct, key = d[0], k;
            i = 0;
            while(message[i] != -1){
                Serial.print(String("Debug"));
                Serial.println(message[i]);
                Serial.println(temp[i]);
                ct = temp[i];
                k = 1;
                for (j = 0; j < key; j++){
                    k = k * ct;
                    k = k % n;
                }
                pt = k + 96;
                Serial.println(pt);
                message[i] = pt;
                i++;
            }
            m[i] = -1;
            // Decrypted message
            for (i = 0; m[i] != -1; i++){
                message[i] = m[i];
                Serial.print(String("Debug"));
                Serial.print(m[i]);
            }
            Serial.println();
            return message;
        }
};

#endif