#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef unsigned char byte;
  
int main(void) {
    int rand_xor_key;
    uint rand_seed;
    uint places_to_shift;
    long i;
    FILE *file_to_dec;
    size_t bytes_from_file;
    byte *alloc_bytes;
    FILE *write_new_file;
    
    file_to_dec = fopen("flag.enc", "rb");
    fread(&rand_seed, 1, 4, file_to_dec);
    srand(rand_seed);
    fseek(file_to_dec, 0, 2);
    
    bytes_from_file = ftell(file_to_dec) - 4;
    fseek(file_to_dec, 4, 0);
    alloc_bytes = malloc(bytes_from_file);
    
    fread(alloc_bytes, bytes_from_file, 1, file_to_dec);
    fclose(file_to_dec);
    
    
   for (i=0; i < bytes_from_file; i++) {
       
       rand_xor_key = rand();
       places_to_shift = rand();
       places_to_shift = places_to_shift & 7;
       
       alloc_bytes[i] = (alloc_bytes[i] >> places_to_shift) | (alloc_bytes[i] << (8 - places_to_shift)); 
       alloc_bytes[i] = alloc_bytes[i] ^ rand_xor_key;
   }
   
   write_new_file = fopen("flag.dec", "wb");
   fwrite(alloc_bytes,1,bytes_from_file,write_new_file);
   fclose(write_new_file);
   free(alloc_bytes);
   
   return 0;
}
