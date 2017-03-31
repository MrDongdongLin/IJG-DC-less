all:
	make -C jpeg-9/src all
	make -C jpeg-compression/src all
	make -C jpeg-decompression/src all
	
clean:
	make -C jpeg-9/src clean
	make -C jpeg-compression/src clean
	make -C jpeg-decompression/src clean