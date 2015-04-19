all:
	c++ -Wall -Wno-sign-compare -std=c++98 bdf.cc -o bdf
print:
	c++ -Wall -Wno-sign-compare -std=c++98 bdf.cc -o bdf && ./bdf tom-thumb.bdf "Hello, world!"
