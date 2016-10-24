#include <vector>
#include <iostream>
using namespace std ;
/*
[
     [2],
    [3,4],
   [6,5,7],
  [4,1,8,3]
]
*/
class Solution {
public:
    int minimumTotal(vector<vector<int>>& triangle) {
		int size = triangle.size() ;
		int tmp1 = 0 , tmp2 = 0 , min = 0 , lastsumi_1 = 0 ;
        if( size == 0 ){
			return 0 ;
		}
		vector<int> sum (triangle[size-1].size(),0);
		int end = 0 , level = 1 , i = 0 , count = 0 ;
		sum[0] = triangle[0][0] ;
		while( level < size ){
			count = triangle[level].size();
			lastsumi_1 = sum[0] ;
			sum[0] += triangle[level][0] ;
			for( i = 1 ; i < count-1 ; ++i ){
				tmp1 = lastsumi_1 ;
				tmp2 = sum[i] ;
				min = tmp1 < tmp2 ? tmp1 : tmp2 ;
				lastsumi_1 = sum[i] ;
				sum[i] = triangle[level][i] + min ;
			}
			sum[i] = triangle[level][i] + lastsumi_1 ;
			++level ;
		}
		tmp2 = sum.size() ;
		tmp1 = sum[0] ;
		for( i = 1 ; i < tmp2 ; ++i ){
			if(tmp1 > sum[i] ){
				tmp1 = sum[i] ;
			}
		}
		return tmp1 ;
    }
};

int main(){
	return 0 ;
}