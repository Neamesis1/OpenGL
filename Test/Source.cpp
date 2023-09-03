#include <iostream>
#include <vector>
#include <string>



void test_func(int* int_array)
{	
	for (int i = 0; i < 3; i++)
	{
		std::cout << int_array[i] << '\n';
	}
}


int main()
{
	int A_ray[3] = { 1, 2, 3 };

	test_func(A_ray);
}
