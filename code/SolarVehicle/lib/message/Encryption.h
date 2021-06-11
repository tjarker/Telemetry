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
#include <stdint.h>

class Security {
    public:
        int pub_key = 11;                                       // Public key
        int prv_key = 17;                                      // Private key

        int t = (pub_key - 1)*(prv_key - 1);                   // Totient function
        int n = pub_key * prv_key;                             // Modulus of prime numbers
        long int i, flag;                             
        long int e[256], d[256], temp[256];

    /**
     * @brief Function to check for prime number
     * @param Integer under test
    */
    private: int prime(int pr){
        int j = sqrt(pr);
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
                    if (k == 9999){
                        break;
                    }
                }
            }
        }

        /**
         * @brief Encrypts input message
         * @param Message as char pointer array
        */
        void encrypt(uint16_t *message, int len){
            long int pt, ct, k, enkey = e[0];
            i = 0;
            while(i < len){
                pt = message[i];
                pt -= 96;
                k = 1;
                for (int j = 0; j < enkey; j++){
                    k = k * pt;
                    k = k % n;
                }
                //temp[i] = k;                       // Array used for encryption and decryption
                //ct = k + 96;
                ct = k + 2*96;
                /*while (ct < 0){
                    ct += 96;
                }*/
                message[i] = ct;
                i++;
            }
        }

        /**
         * @brief Decrypts input message
         * @param Message as char pointer array
        */
        void decrypt(uint16_t *message, int len){
            long int pt, ct, dekey = d[0], k;
            i = 0;
            while(i < len){
                //ct = temp[i];                      // Array used for encryption and decryption
                ct = message[i] - 2*96;
                /*while (ct > 255){
                    ct -= 96;
                }*/
                k = 1;
                for (int j = 0; j < dekey; j++){
                    k = k * ct;
                    k = k % n;
                }
                pt = k + 96;
                message[i] = pt;
                i++;
            }
        }
};

#endif