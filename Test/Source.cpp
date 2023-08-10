#include <iostream>
#include <vector>

int main()
{
	std::vector<int> vec_A = {1, 2, 3, 4, 5};
	std::vector<int> vec_B = { 10, 2, 33, 4, 5};

	std::vector<int> intersection;

	for (int i = 0; i < vec_A.size(); i++)
	{
		int A = vec_A[i];
		int B = vec_B[i];

		if (A == B) { intersection.push_back(A); }
	}
}
