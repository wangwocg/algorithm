#include<iostream>
#include <vector>
using namespace std ;
class Solution {
public:
    vector<int> grayCode1(int n) {
        if( n < 0 ){
			return vector<int>();
		}
		else if( n==0){
		    return vector<int>(1,0);
		}
		vector<int> ret ;
		ret.push_back(0);
		ret.push_back(1);
		int i= 0 , j = 0 ;
		for( i=1 ; i< n ; ++i ){
			for( j=ret.size()-1; j>= 0 ; --j ){
				ret.push_back( ret[j]|(1<<i) );
			}
		}
		return ret ;
    }
	void set( int & val ,int index, vector<int>&ret ){
		if( index <= -1 ){
			ret.push_back(val);
			return ;
		}
		set(val,index-1,ret);
		val ^=(1<<index);
		set(val,index-1,ret);
		return ;
	}
    vector<int> grayCode(int n) {
		if( n < 0 ){
			return vector<int>();
		}
		else if( n==0){
		    return vector<int>(1,0);
		}
		vector<int> ret ;
		int val = 0 ;
		set(val,n,ret);
		return ret ;
	}
};

int main(){
	Solution s ;
	vector<int> ret = s.GrayCode(1);
	for(int i = 0 ;i<ret.size();++i){
		cout<<ret[i]<<" ";
	}
	cout<<endl;
}