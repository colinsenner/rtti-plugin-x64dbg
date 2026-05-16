#include "Classes.h"
#include <stdio.h>

void ClassA::A_virt1()
{
	printf("A_virt1()\n");
}

void ClassA::A_virt2()
{
	printf("A_virt2()\n");
}

void ClassB::B_virt1()
{
	printf("B_virt1()\n");
}

void ClassB::B_virt2()
{
	printf("B_virt2()\n");
}

void ClassC::A_virt2()
{
	printf("Overridden A_virt2 by ClassC\n");
}

void ClassC::B_virt2()
{
	printf("Overridden B_virt2 by ClassC\n");
}

void ClassC::C_virt1()
{

}

/* /Docs/bh-dc-07-Sabanal_Yason-WP.pdf */
int Ex2::get_sum(int x, int y)
{
	return x + y;
}

void Ex2::reset_values()
{
	var1 = 0;
}

void Ex4::func1()
{
}

void Ex4::func2()
{
}

void Ex5::func1()
{
}

void Ex5::v_ex5()
{
}
