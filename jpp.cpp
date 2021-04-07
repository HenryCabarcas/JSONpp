#include "jpp.h"
#include "jpp_validator.h"
#include <math.h>
#include <chrono>
#include <fstream>
std::chrono::steady_clock::time_point hClock()
{
	return std::chrono::high_resolution_clock::now();
}

std::uint32_t TimeDuration(std::chrono::steady_clock::time_point Time)
{
	return (std::chrono::high_resolution_clock::now() - Time) / std::chrono::nanoseconds(1);
}
void log(const std::let& val) {
	std::cout << &val << std::endl;
}
int main()
{

	std::JSON handler;
	handler.readJSON("test.json");
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
