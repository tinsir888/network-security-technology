# File Integrity Check Program Based on MD5 Algorithm

## command manual

+ ./md5 -h  
  help info
+ ./md5 -t  
  print test information
+ ./md5 -c \[filename.txt\]  
  calculate the MD5 digest of the file under test and print it
+ ./md5 -v \[filename.txt\]  
  the user inputs the MD5 digest of the tested file, then recalculates the MD5 digest of the tested file, and compares the two digests by bit.
+ ./md5 -f \[filename.txt\] \[filename.md5\]  
  The program reads the digest of nankai.md5, recalculates the MD5 of the tested file nankai.txt, and finally compares the two by bit.

# 网络安全技术 实验3 基于 MD5 算法的文件完整性校验程序

## 运行命令
+ ./md5 -h  
查看帮助
+ ./md5 -t  
打印程序的测试信息
+ ./md5 -c nankai.txt  
计算出的被测文件的MD5摘要并打印
+ ./md5 -v nankai.txt  
验证文件完整性1：用户输入被测文件的MD5摘要，然后重新计算被测文件nankai.txt的MD5摘要，将两个摘要逐位比较
+ ./md5 -f nankai.txt nankai.md5  
验证文件完整性2：程序读取nankai.md5摘要，重新计算被测文件nankai.txt的MD5，最后将两者逐位比较