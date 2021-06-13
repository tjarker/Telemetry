#ifndef __RSA_H__
#define __RSA_H__

#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <stdint.h>

class RSA {
    private : 
        double p = 11, q = 17;                 // Choose two prime numbers p & q
        double n = p * q, phi = (p-1) * (q-1); // Calculate n = p * q and phi(n) = (p-1) * (q-1)
        double e = 7;                          // Select e such that 1 < e < phi(n), and gcd(e, phi(n)) = 1
        double d = fmod(1/e, phi);          // Compute d as the multiplicative inverse of e (mod phi(n))

        /**
         * 
         * 
        */
        int gcd(int a, int b)               // Helper function to compute the greatest common denominator of two integers a and b
        {
            if (!(a%b)){
                return b; 
            } else {
                return gcd(b, a%b); 
            }
        }

    public: 

        /**
         *  
         *  
        */
        void encrypt(uint8_t *arr, int len)  
        {
            for (int i = 0; i < len; i++){
                Serial.println(fmod(arr[i]*e,n), HEX);
                arr[i] = (uint8_t) fmod(arr[i]*e,n);  
            }
        }

        void decrypt(uint8_t *arr, int len)
        {
            for (int i = 0; i < len; i++){
                Serial.println(fmod(arr[i]*d,n), HEX);
                arr[i] = (uint8_t) fmod(arr[i]*d,n); 
            }
        }

};

#endif