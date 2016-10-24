#include <iostream>
using namespace std;

class A
{
public:
	virtual void printA()
	{
		cout << "Hello A!" << endl;
		cout << "this -> " << hex << this << endl;
	}
};

class B
{
public:
	virtual void printB()
	{
		cout << "Hello B!" << endl;
		cout << "this -> " << hex << this << endl;
	}
};

class C : public A,public B
{
	virtual void printA()
	{
		cout << "C,Hello A!" << endl;
		cout << "this -> " << hex << this << endl;
	}

	virtual void printB()
	{
		cout << "C,Hello B!" << endl;
		cout << "this -> " << hex << this << endl;
	}
};

int main(int argc, char const *argv[])
{
	B* b_p = new C();
	// cout << hex << b_p << endl;
	C* c_p = dynamic_cast<C*>(b_p);
	cout << hex <<"bp="<< b_p << endl;
	cout << hex <<"cp="<< c_p << endl;
	// b_p->printB();

	unsigned long* base = (unsigned long*)b_p;
	unsigned long*vptr = (unsigned long*)(*base);

	for(int i=0;i<2;i++)
	{
//		cout << hex << *(vptr + i) << endl;
	}



	return 0;
}
