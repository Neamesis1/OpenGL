#include <iostream>

int x = 1;

void print_x()
{
	std::cout << x << '\n';
	x += 1;
}

int main()
{
	print_x();

	std::cout << x << '\n';
}
