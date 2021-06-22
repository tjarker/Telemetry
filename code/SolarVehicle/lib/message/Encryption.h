/****************************************************************************************************
 * @file    Encryption.h                                                                            *
 * @author  Victor Alexander Hansen                                                                 *
 * @brief   Encryption and decryption of message array of type message using the RSA algorithm.     *
 *                                                                                                  *
 * Two prime numbers must be chosen and set in the public class value. Suitable combinations of     *
 * prime numbers can be seen marked with green fields in the figure at                              *
 * https://bitbucket.org/dtucar/ecocar-solar/src/master/Telemetry/prime_tabel.png                   *
 ****************************************************************************************************/


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
        long int e[256], d[256];
        bool activate = false; 
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
         * @param message message pointer to be encrypted
         * @param array outgoing encrypted message pointer
         * @param len length of message
        */
        void encrypt(uint8_t *message, uint16_t *array, int len){
            long int pt, k, enkey = e[0];
            uint16_t ct;
            i = 0;
            while(i < len){
                pt = message[i] - 96;
                k = 1;
                for (int j = 0; j < enkey; j++){
                    k = k * pt;
                    k = k % n;
                }
                ct = k + 2*96;
                array[i] = ct;
                i++;
            }
        }

        /**
         * @brief Decrypts input message
         * @param array incoming encrypted message pointer
         * @param message the decrypted message pointer
         * @param len length of message
        */
        void decrypt(uint16_t *array, uint8_t *message, int len){
            long int ct, dekey = d[0], k;
            uint8_t pt;
            i = 0;
            while(i < len){
                ct = array[i] - 2*96;
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