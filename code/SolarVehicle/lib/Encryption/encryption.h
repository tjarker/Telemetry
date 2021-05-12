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

class security {
    public:
        int pub_key = 7;                                       // Public key
        int prv_key = 13;                                      // Private key

        int t = (pub_key - 1)*(prv_key - 1);                   // Totient function
        int n = pub_key * prv_key;                             // Modulus of prime numbers
        int i, flag;
        long int e[200], d[200], temp[200], j;
        char en[200], m[200];

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

        /**
         * @brief Encrypts input message
         * @param Message as char pointer array
        */
        char* encrypt(char *message, long int len){
            long int pt, ct, key = e[0], k;
            i = 0;

            while(i != len){
                pt = message[i];
                pt -= 96;
                k = 1;
                for (j = 0; j < key; j++){
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
        char* decrypt(char* message, long int len){
            long int pt, ct, key = d[0], k;
            i = 0;
            while(i != len){
                ct = temp[i];                      // Array used for encryption and decryption
                k = 1;
                for (j = 0; j < key; j++){
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