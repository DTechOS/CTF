## Challenge Descrition
On our regular checkups of our secret flag storage server we found out that we were hit by ransomware! The original flag data is nowhere to be found, but luckily we not only have the encrypted file but also the encryption program itself.

--------------------


**Static Analysis**

The files content has been encrypted so we need to analyze the ransomware's encryption routine and find a way to decrypt the strings.
$ cat flag.enc 
Z5�b�>�����u���9�K�!�C#qe�'K  



**Encryption:** 

The program first takes a text file, and uses seek() to im guessing, select the offset at which to start encrypting data.

```
  file_pointer = *(long *)(in_FS_OFFSET + 40);
  file_to_enc = fopen("flag","rb");
  fseek(file_to_enc,0,2);
  local_28 = ftell(file_to_enc);
  fseek(file_to_enc,0,0);
  local_20 = malloc(local_28);
  fread(local_20,local_28,1,file_to_enc);
  fclose(file_to_enc);
  tVar2 = time((time_t *)0x0);
  local_40 = (uint)tVar2;
  srand(local_40);
```

**A quick example of fseek():**


This program will overwrite the data at offset 7 with the string "C programming language"
The output of this program prints "This is C Programming Language"
```
#include <stdio.h>

int main () {
   FILE *fp;

   fp = fopen("file.txt","w+");
   fputs("This is tutorialspoint.com", fp);
  
   fseek( fp, 7, SEEK_SET );
   fputs(" C Programming Language", fp);
   fclose(fp);
   
   return(0);
}
```
Notice the first and second call to fseek has a different value in the 3rd paramter. there are 3 options for the 3rd parameter of fseek()
and the third paramter corresponds to the position from where offset is added.

1 - SEEK_SET - Beginning of file

2 - SEEK_CUR - Current position of the file pointer

3 - SEEK_END - End of file


It allocates enough memory to read from the file.
```
  bytes_from_file = ftell(file_to_enc);
  fseek(file_to_enc,0,0);
  alloc_bytes = malloc(bytes_from_file);
  fread(alloc_bytes,bytes_from_file,1,file_to_enc);
  fclose(file_to_enc);
```
It Initializes a random seed for encryption:

```
tVar1 = time((time_t *)0x0);
rand_seed = (uint)tVar1;
srand(maybe_RandTime); // seeds the random number generator used by the function rand
```

**Reversing the Encryption Algorithm:**

It's not really an encryption routine, it's just a few bitwise operations and a random seed that we need to run in reverse in order to extract the original contents of the file, if this were real ransomware, it would more of an effort to defeat the encryption, which may not even be possible if you can't find some vulnerability in the cryptographic algorithm, it happens, but rarely. Here's one example of researches finding a flaw in the encryption routine to defeat "Hive" ransomware https://thehackernews.com/2022/02/master-key-for-hive-ransomware.html

Encryption Routine:
```
  for (i = 0; i < (long)bytes_from_file; i = i + 1) {
    rand_xor_key = rand();
    *(byte *)((long)alloc_bytes + i) = *(byte *)((long)alloc_bytes + i) ^ (byte)rand_xor_key;
    places_to_shift = rand();
    places_to_shift = places_to_shift & 7;
    *(byte *)((long)alloc_bytes + i) =
         *(byte *)((long)alloc_bytes + i) << (sbyte)places_to_shift |
         *(byte *)((long)alloc_bytes + i) >> 8 - (sbyte)places_to_shift;
  }

```

To understand how we can extract the original value, lets break down the above bitwise operation into a more simple program.
in this example, the second shift operation just reverses the first, and returns the original value. In a bitwise shift operator The left operands value is moved left or right by the number of bits specified by the right operand. So to decrypt the files contents, we just need to run the ransomwares algorithm in reverse.

 ```
 int main() 
{
  //  int rand_xor_key;
   // int *alloc_bytes;
   int num;
   int places_to_shift;
   int shifted_val;
   int shifted_val2;
   
   num_to_shift = 2;
   num = 5;
   
   shifted_val = num << places_to_shift;
   shifted_val2 = shifted_val >> palces_to_shift;
   
    
   
    printf("original value: %d\nshifted bits: %d\nreversed shifted value: %d ", num, shifted_val, shifted_val2);
   // printf("some num: %d ", rand_xor_key);
    return 0;
}

```
Output:
```
original value: 5
shifted bits: 20
reversed shifted value: 5 
```
 
**Coding a solution in C:**

all we need to do is extract the pseudo random seed with fread(&random_seed,1, 4, file_to_dec).
Then finally just run the same bitwise shift arithmetic in reverse and write the contents out into a new file.
```
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
```
Finally compile and run the program, then cat the flag.dec file to get the flag.










