## Challenge Descrition
On our regular checkups of our secret flag storage server we found out that we were hit by ransomware! The original flag data is nowhere to be found, but luckily we not only have the encrypted file but also the encryption program itself.

--------------------


** Static Analysis **

The files content has been encrypted so we need to analyze the ransomwares encryption routine and find a way to decrpyt the files.
$ cat flag.enc 
Z5�b�>�����u���9�K�!�C#qe�'K  



** Encryption: ** 
The program first creates a file flag.txt, and uses seek() to im guessing, select the offset at which to start encrpyting data.

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
