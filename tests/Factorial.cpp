#include "Factorial.h"

int Factorial::fact(const int x)
{
	int p = 1;
	int d = x;
	while (d != 0)
	{
		p = p * d;
		d = d - 1;
	}
	return p;
}
