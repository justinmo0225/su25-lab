#include <time.h>
#include <stdio.h>
#include <x86intrin.h>
#include "ex1.h"

long long int sum(int vals[NUM_ELEMS]) {
    clock_t start = clock();

    long long int sum = 0;
    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS; i++) {
            if(vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    long long int sum = 0;

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4) {
            if(vals[i] >= 128) sum += vals[i];
            if(vals[i + 1] >= 128) sum += vals[i + 1];
            if(vals[i + 2] >= 128) sum += vals[i + 2];
            if(vals[i + 3] >= 128) sum += vals[i + 3];
        }

        // TAIL CASE, for when NUM_ELEMS isn't a multiple of 4
        // NUM_ELEMS / 4 * 4 is the largest multiple of 4 less than NUM_ELEMS
        // Order is important, since (NUM_ELEMS / 4) effectively rounds down first
        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++) {
            if (vals[i] >= 128) {
                sum += vals[i];
            }
        }
    }
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return sum;
}

long long int sum_simd(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127); // This is a vector with 127s in it... Why might you need this?
    long long int result = 0; // This is where you should put your final result!
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */

        __m128i vector_sum = _mm_setzero_si128();   // initializes vec to 0 to store 4 ints in 1 go
        for(unsigned int i = 0; i < NUM_ELEMS / 4 * 4; i += 4){       // blocks of 4 for mem alignment
            __m128i temp = _mm_loadu_si128((__m128i *)(vals + i));    // loads 4 ints of 32 bits
            __m128i greaterThan = _mm_cmpgt_epi32(temp, _127);        // if > 127, we have 0xFFFFFFFF
            temp = _mm_and_si128(temp, greaterThan);             // anything > 127 just becomes 0
            vector_sum = _mm_add_epi32(vector_sum, temp);          // adds the final values to the sum
        }

        int temp_arr[4];      // will be used to grab sum from SIMD reg
        _mm_storeu_si128((__m128i *)temp_arr, vector_sum);       // stores it
        result = result + temp_arr[0] + temp_arr[1] + temp_arr[2] + temp_arr[3];    // adds it all up yo
    

        /* Hint: you'll need a tail case. */
    
        // adds any element to res if it is larger than 127

        for(unsigned int i = NUM_ELEMS / 4 * 4; i < NUM_ELEMS; i++){
            if(vals[i] > 127){
                result += vals[i];
            }
        }

    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}

long long int sum_simd_unrolled(int vals[NUM_ELEMS]) {
    clock_t start = clock();
    __m128i _127 = _mm_set1_epi32(127);
    long long int result = 0;
    /* DO NOT MODIFY ANYTHING ABOVE THIS LINE (in this function) */

    for(unsigned int w = 0; w < OUTER_ITERATIONS; w++) {
        /* YOUR CODE GOES HERE */
        /* Copy your sum_simd() implementation here, and unroll it */
        
       // literally the same thing as earlier but just 3 of them set to 0 
       __m128i vec1 = _mm_setzero_si128();
        __m128i vec2 = _mm_setzero_si128();
        __m128i vec3 = _mm_setzero_si128();
        // 3 x 4 = 12 total. again, same thing as earlier
        for(unsigned int i = 0; i < NUM_ELEMS / 12 * 12; i += 12) {
            // loads respective 4 blocks into temp
            __m128i temp1 = _mm_loadu_si128((__m128i *) (vals + i));
            __m128i temp2 = _mm_loadu_si128((__m128i *) (vals + i + 4));
            __m128i temp3 = _mm_loadu_si128((__m128i *) (vals + i + 8));
            // compares each with 127 -> can create masks
            __m128i compare1 = _mm_cmpgt_epi32(temp1, _127);
            __m128i compare2 = _mm_cmpgt_epi32(temp2, _127);
            __m128i compare3 = _mm_cmpgt_epi32(temp3, _127);
            // zero outs any value less than 128 since we dont want it using AND (masks)
            temp1 = _mm_and_si128(temp1, compare1);
            temp2 = _mm_and_si128(temp2, compare2);
            temp3 = _mm_and_si128(temp3, compare3);
            // add values to each vector
            vec1 = _mm_add_epi32(vec1, temp1);
            vec2 = _mm_add_epi32(vec2, temp2);
            vec3 = _mm_add_epi32(vec3, temp3);
        }

        int temp_arr1[4] = {0};     // 3 arrays like earlier for sum total
        int temp_arr2[4] = {0};
        int temp_arr3[4] = {0};
        _mm_storeu_si128((__m128i *) temp_arr1, vec1);
        _mm_storeu_si128((__m128i *) temp_arr2, vec2);
        _mm_storeu_si128((__m128i *) temp_arr3, vec3);
        // adds it all to result
        result = result + temp_arr1[0] + temp_arr1[1] + temp_arr1[2] + temp_arr1[3];
        result = result + temp_arr2[0] + temp_arr2[1] + temp_arr2[2] + temp_arr2[3];
        result = result + temp_arr3[0] + temp_arr3[1] + temp_arr3[2] + temp_arr3[3];

        /* Hint: you'll need 1 or maybe 2 tail cases here. */

        // just gets anything that isnt within the 12 block

        for(unsigned int i = NUM_ELEMS / 12 * 12; i < NUM_ELEMS; i++) {
            if (vals[i] > 127){

                result += vals[i];        // add the elem if its > 127
            }
        }

    }

    /* DO NOT MODIFY ANYTHING BELOW THIS LINE (in this function) */
    clock_t end = clock();
    printf("Time taken: %Lf s\n", (long double)(end - start) / CLOCKS_PER_SEC);
    return result;
}
