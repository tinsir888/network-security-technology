# Lab 3 File Integrity Check Program Based on MD5 Algorithm

## compile
`g++ *.cpp -o md5`

## command manual

+ `./md5 -h`
  help info
+ `./md5 -t`
  print test information
+ `./md5 -c \[filename.txt\]`  
  calculate the MD5 digest of the file under test and print it
+ `./md5 -v \[filename.txt\]` 
  the user inputs the MD5 digest of the tested file, then recalculates the MD5 digest of the tested file, and compares the two digests by bit.
+ `./md5 -f \[filename.txt\] \[filename.md5\]`  
  The program reads the digest of nankai.md5, recalculates the MD5 of the tested file nankai.txt, and finally compares the two by bit.
