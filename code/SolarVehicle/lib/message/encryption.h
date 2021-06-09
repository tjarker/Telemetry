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
#include <ACAN.h>

class Security {
    public:
        int pub_key = 7;                                       // Public key
        int prv_key = 13;                                      // Private key

        int t = (pub_key - 1)*(prv_key - 1);                   // Totient function
        int n = pub_key * prv_key;                             // Modulus of prime numbers
        uint64_t i, flag;
        uint64_t e[256], d[256], temp[256], j;
        //cmd_t cmd_temp[256];

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
            int k = 0;
            for (i = 2; i < t; i++){
                if (t % i == 0){
                    continue;
                }
                flag = prime(i);
                if (flag == 1 && i != pub_key && i != prv_key){
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

        uint64_t encrypter(uint64_t data){
            uint64_t pt, ct, key = e[0], k;
            pt = data;
            pt -= 96;
            k = 1;
            for (j = 0; j < key; j++){
                k = k * pt;
                k = k % n;
            }
            temp[i] = k;                       // Array used for encryption and decryption
            ct = k + 96;
            return ct;
        }

        /**
         * @brief Encrypts input message
         * @param Message as char pointer array
        */
        CanTelemetryMsg encrypt(CanTelemetryMsg message, int len){
            //cmd_t cmd_pt, cmd_ct, cmd_k;
            uint64_t pt, ct, key = e[0], k;

            // cmd
            /* Virker ikke helt efter hensigten...
            cmd_pt = message.cmd;
            //cmd_pt -= 96;
            k = 1;
            for (j = 0; j < key; j++){
                cmd_k = cmd_k * cmd_pt;
                cmd_k = cmd_k % n;
            }
            cmd_temp[i] = cmd_k;                       // Array used for encryption and decryption
            cmd_ct = cmd_k + 96;
            message.cmd = cmd_ct;
            */

            // id
            pt = message.id;
            pt -= 96;
            k = 1;
            for (j = 0; j < key; j++){
                k = k * pt;
                k = k % n;
            }
            temp[i] = k;                       // Array used for encryption and decryption
            ct = k + 96;
            message.id = ct;

            // rtr
            pt = message.rtr;
            pt -= 96;
            k = 1;
            for (j = 0; j < key; j++){
                k = k * pt;
                k = k % n;
            }
            temp[i] = k;                       // Array used for encryption and decryption
            ct = k + 96;
            message.rtr = ct;

            // len
            pt = message.len;
            pt -= 96;
            k = 1;
            for (j = 0; j < key; j++){
                k = k * pt;
                k = k % n;
            }
            temp[i] = k;                       // Array used for encryption and decryption
            ct = k + 96;
            message.len = ct;

            // data[8]
            i = 0;
            while(i != len){
                pt = message.data[i];
                pt -= 96;
                k = 1;
                for (j = 0; j < key; j++){
                    k = k * pt;
                    k = k % n;
                }
                temp[i] = k;                       // Array used for encryption and decryption
                ct = k + 96;
                message.data[i] = ct;                   // Encrypted message
                i++;
            }

            // data64
            encrypter(message.data64);

            // s
            encrypter(message.s);

            // m
            encrypter(message.m);

            // h
            encrypter(message.h);

            return message;
        }

        uint64_t decrypter(uint64_t data){
            uint64_t pt, ct, key = d[0], k;
            ct = temp[i];                      // Array used for encryption and decryption
            k = 1;
            for (j = 0; j < key; j++){
                k = k * ct;
                k = k % n;
            }
            pt = k + 96;
            data = pt;
        }

        /**
         * @brief Decrypts input message
         * @param Message as char pointer array
        */
        CanTelemetryMsg decrypt(CanTelemetryMsg message, int len){
            cmd_t cmd_pt, cmd_ct;
            uint64_t pt, ct, key = d[0], k;

            // cmd
            /* Virker ikke helt efter hensigten...
            cmd_ct = temp[i];                      // Array used for encryption and decryption
            k = 1;
            for (j = 0; j < key; j++){
                k = k * cmd_ct;
                k = k % n;
            }
            pt = k;
            message.cmd = cmd_pt;
            */

            // id
            ct = temp[i];                      // Array used for encryption and decryption
            k = 1;
            for (j = 0; j < key; j++){
                k = k * ct;
                k = k % n;
            }
            pt = k + 96;
            message.id = pt;

            // rtr
            ct = temp[i];                      // Array used for encryption and decryption
            k = 1;
            for (j = 0; j < key; j++){
                k = k * ct;
                k = k % n;
            }
            pt = k + 96;
            message.rtr = pt;

            // len
            ct = temp[i];                      // Array used for encryption and decryption
            k = 1;
            for (j = 0; j < key; j++){
                k = k * ct;
                k = k % n;
            }
            pt = k + 96;
            message.len = pt;

            // data[8]
            i = 0;
            while(i != len){
                ct = temp[i];                      // Array used for encryption and decryption
                k = 1;
                for (j = 0; j < key; j++){
                    k = k * ct;
                    k = k % n;
                }
                pt = k + 96;
                message.data[i] = pt;                   // Decrypted message
                i++;
            }

            // data64
            decrypter(message.data64);

            // s
            decrypter(message.s);

            // m
            decrypter(message.m);

            // h
            decrypter(message.h);

            return message;
        }
};

#endif