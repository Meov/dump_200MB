# dump_200MB
To dump the LOG file from DDR.  In  gohigh
1. modify the addr into 32 bit (long unsigned int) 
2. change the tar format from:

	tar -czpf file.tar.gz  
	to tar _-cjvf file.tar.bz2 file   //压缩率大
