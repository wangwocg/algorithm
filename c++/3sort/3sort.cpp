#include <iostream>  
#include <ctime>
#include <vector>
#include <string>
using namespace std;  
static int sa = 0 ;
int length=0;
template <typename T>  
void sawp(T a[],int first,int second){  
	sa++;
    T temp;  
    temp = a[first];  
    a[first] = a[second];  
    a[second] = temp;  

	cout<<endl;
	for(int i = 0 ; i<length ;++i){
		cout<<a[i]<<" ";
	}
	cout<<endl;
}  
template <typename T>  
int sort(T a[],int low,int high){  
    if(low < high){  
		sort1(a,low,high);
        int lt = low,i=low+1,gt = high;  
        int temp = a[low];  
        while(i <= gt){  
            if(a[i] < temp){  
                sawp(a,lt++,i++);  
            }else if(a[i] > temp){  
                sawp(a,i,gt--);  
            }else{  
                i++;  
            }  
        }  
        sort(a,low,lt-1);  
        sort(a,gt+1,high);  
		
    }  
	return 0 ;
}  
void sort1(int a[],int left, int right){
	if( left >= right ){
		return ;
	}
	int size = right - left+ 1 ;
	srand(time(NULL));
	int index = rand()%size ;
	sawp(a,left,left+index);
	int temp = a[left];
	index=left;
	int start = left , end = right ;
	while( left < right ){
	
		while( left< right && a[left] <= temp ){
			left++;
		}
		while( right >= left && a[right] >= temp ){
			right--;
		}
		if( left < right){
			sawp(a,left,right);
		}
		else{
			sawp(a,index,right);
		}
	}
	sort1(a,start,right-1);
	sort1(a,left,end);
}
template <class T> void f(T  a){

	cout<<"in f="<<a<<endl;
}

template <class T> void f(T * a){
	T b = *a ;
	cout<<"in *f="<<b<<endl;
}
/*
template <> void f(int  a){
	cout<<"int f="<<a<<endl;
}
*/
/*
template <class T> class TA{
	public:

	TA(T a){
		cout<<"TA"<<endl;
	}
};
template <class T> class TA<T *>{
	public :

	TA(T a){
		cout<<"TA *"<<endl;
	}
};
*/
union obj{
	union obj * next ;
	char data[1];
};
struct bbb{
	int a1;
	int b1;
};
struct aaa{
	int a;
	char c;
	float f ;
	bbb b;
};
int main() {
//	vector<char> v(10);
//	cout<<"size="<<v.size()<<endl;
	int *v = new int[10];
	cout<<"v[5]="<<v[0]<<"|"<<endl;
	aaa a5 ;
	cout<<"a="<<a5.a<<" c="<<a5.c<<" f="<<a5.f<<endl;
	bbb * b1=new bbb();
	cout<<"b1->a1="<<b1->a1<<" b1->b1="<<b1->b1<<endl;
	int tmp=int();
	cout<<"tmp="<<tmp<<endl;
	return 0 ;
	int * pp = new int[10] ;
	*pp = 5 ;
	cout<<"*pp="<<*pp<<endl;
	delete  pp ;
	union obj a1 ;
	cout<<sizeof(a1)<<endl;
	return 0 ;
    int aa=0;
	int * p = &aa;
	double dd = 2.1 ;
	int a[] = {5,7,0,2,2,0,6,7,6,8,7,1,0,0,0,0,1,5};  
    int len = sizeof(a)/sizeof(int);  
	length = len ;
    sort1(a,0,len-1);  
    for (int i = 0; i < len; ++i)  
        cout << a[i] << " ";  
    cout << endl;  
	cout<<"static sa="<<sa<<endl;
	return 0 ;
}