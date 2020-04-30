//g++ -std=c++11 -ggdb `pkg-config --cflags --libs opencv` -static-libstdc++ AITest2.cpp camera.cpp humanFilter.cpp -o AITest2

#include "camera.hpp"

int main(int argc, char** argv)
{
	Camera cam(0);
	cam.record();
	return 0;
}
