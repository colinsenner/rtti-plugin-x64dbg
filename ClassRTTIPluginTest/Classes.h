#pragma once

class ClassA {
public:
	int aInt = 0xA1AAAAAA;
	virtual void A_virt1();
	virtual void A_virt2();
};

class ClassB {
public:
	int b1 = 0xB1BBBBBB;
	int b2 = 0xB2BBBBBB;
	virtual void B_virt1();
	virtual void B_virt2();
};

class ClassC : public virtual ClassA, public virtual ClassB {
public:
	int c1 = 0xC1CCCCCC;
	int c2 = 0xC2CCCCCC;
	int c3 = 0xC3CCCCCC;

	virtual void A_virt2() override;
	virtual void B_virt2() override;
	virtual void C_virt1();
};

// Multiple Inheritance example 
// /Docs/bh-dc-07-Sabanal_Yason-WP.pdf
class Ex2 
{
	int var1 = 0x01E20000;

public:
	virtual int get_sum(int x, int y);
	virtual void reset_values();
};

class Ex4
{
	int var1 = 0x01E40000;
	int var2 = 0x02E40000;

public:
	virtual void func1();
	virtual void func2();
};

class Ex5 : public Ex2, Ex4
{
	int var1 = 0x01E50000;

public:
	void func1();
	virtual void v_ex5();
};