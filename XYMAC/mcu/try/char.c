#include "stdio.h"

int main()
{
	if((char)0xBE < (char)0xBF)
		printf("yes\n%d\t%d\n", (char)0xBE, (char)0xBF);

	return 0;
}
