#include "src/JSONpp.h"
#include <math.h>
//#include <chrono>
#include <fstream>

void Log(json::let val)
{
	std::cout << val << std::endl;
}
int main()
{

	json::JSON handler;
	long int size = 0;
	char* buffer = readFile("tests/test.json", &size);
	Log(handler.Parse(buffer));
	// handler.readJSON(buffer);
	 //std::cout << handler << std::endl;

	 //Vec<int> Any = {3, 02, 50, 90};
	 //Any.forEach([](auto item) {
	 //	std::cout << std::to_string(item) << std::endl;
	 //});
	 //vec2 a({3,4});
	 //std::cout << a.x<< std::endl;
	 //system("pause");
	return 0;
}
