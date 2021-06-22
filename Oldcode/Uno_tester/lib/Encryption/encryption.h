/** 
 * Encryption and decryption of char array message using the RSA algorithm.
 * Two prime numbers must be chosen, one for the public key and one for the private key.
 * For more information, see http://www.trytoprogram.com/cpp-examples/cplusplus-program-encrypt-decrypt-string/
*/

#ifndef __ENCRYPTION_H__
#define __ENCRYPTION_H__

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <Arduino.h>


class Security {
    public:
        int pub_key = 7;                                       // Public key
        int prv_key = 13;                                      // Private key

        int t = (pub_key - 1)*(prv_key - 1);                   // Totient function
        int n = pub_key * prv_key;                             // Modulus of prime numbers
        long int i, flag;
        long int e[256], d[256], temp[256], j;

    /**
     * @brief Function to check for prime number
     * @param Integer under test
    */
    private: int prime(int pr){
        j = sqrt(pr);
        for (int l = 2; l <= j; l++){
            if (pr % l == 0){
                return 0;
            }
        }
        return 1;
    }

    /**
     * @brief Returns first possible integer ratio for totient function and parameter integer
     * @param Integer for which the corresponding modulus with the totient function isn't 0
    */
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
            Serial.print(String(""));
            int enk = 0;
            for (i = 2; i < t; i++){
                if (t % i == 0){
                    continue;
                }
                flag = prime(i);
                if (flag == 1 && i != pub_key && i != prv_key){
                    Serial.println("Should not be called");
                    e[enk] = i;
                    flag = cd(e[enk]);
                    if (flag > 0){
                        d[enk] = flag;
                        enk++;
                    }
                    if (enk == 99){
                        break;
                    }
                }
            }
            
            Serial.print("e = ");
            Serial.println(e[0]);
            Serial.println();
        }

        /**
         * @brief Encrypts input message
         * @param Message as char pointer array
        */
        char* encrypt(char *message, int len){
            int pt, ct, k;
            const int enkey = e[0];
            i = 0;

            while(i != len){
                pt = message[i];
                pt -= 96;
                k = 1;
                Serial.print("encrypt: key = ");
                Serial.println(e[0]);
                for (j = 0; j < enkey; j++){
                    k = k * pt;
                    k = k % n;
                }
                temp[i] = k;                       // Array used for encryption and decryption
                ct = k + 96;
                message[i] = ct;                   // Encrypted message
                i++;
            }

            return message;
        }

        /**
         * @brief Decrypts input message
         * @param Message as char pointer array
        */
        char* decrypt(char* message, int len){
            int pt, ct, k;
            const int dekey = d[0];
            i = 0;
            Serial.print("decrypt: key = ");
            Serial.println(e[0]);
            while(i != len){
                ct = temp[i];                      // Array used for encryption and decryption
                k = 1;
                for (j = 0; j < dekey; j++){
                    k = k * ct;
                    k = k % n;
                }
                pt = k + 96;
                message[i] = pt;                   // Decrypted message
                i++;
            }

            return message;
        }
};

#endif