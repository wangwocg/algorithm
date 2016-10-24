#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <cstdlib>
using namespace std;
/*
class Solution {//604ms
public:
	int insertKFrequent(int currentnum ){
		int num = 0 ;
		if( num == 0 ){
			klist.push_front(currentnum);
			++num ;
		}
		else{
			list<int>::iterator iter = klist.begin() ;
			while( iter != klist.end() ){
				if( *iter > currentnum ){
					++iter ;
				}
				else{
					break ;
				}
			}
			klist.insert(iter,currentnum);
			++num ;
		}
		if( num > knum ){
			--num ;
			klist.pop_back();
		}
		return 0 ;
	}
    int findKthLargest(vector<int>& nums, int k) {
        knum = k ;
		for( vector<int>::iterator iter = nums.begin();iter != nums.end();iter++){
			insertKFrequent(*iter);
		}
		return klist.back();
    }
private:
	int knum ;
	list<int> klist ;
};
*/

class Solution {//12ms
public:

	int partition( int nindex , int start , int end , vector<int>& nums ){
		swap(nums[nindex],nums[start]);
		int i = start ;
		int j = end ;
		int n = nums[start];
		while( i < j ){
		
			while( i < j && nums[j] < n ){
				--j ;
			}
			while( i < j && nums[i] >= n ){
				++i ;
			}
			swap(nums[i],nums[j]);
		}
		swap(nums[i],nums[start]);
		return i ;
	}
	int klargest(vector<int>&nums,int start,int end ,int k ){
		int sub = end - start ,nindex = 0 ;
		if( sub == 0 ){
			nindex = start ;
		}
		else{
			nindex = random( )%sub + start ;//有这个随机性耗费12ms，没有这个随机性耗费68ms
		}
		int ret = partition(nindex,start,end,nums);
		if( ret == k-1 ){
			return nums[ret];
		}
		else if(ret < k-1 ) {
			return klargest(nums,ret+1,end,k);
		}
		
			return klargest(nums,start,ret-1,k);
		
	}
    int findKthLargest(vector<int>& nums, int k) {
		
        int ret = klargest(nums,0,nums.size()-1,k);
		return ret ;
    }
};

int main(){
	Solution s ;
	int a [] = {2,1};
	vector<int> v;
	v.push_back(2);
	v.push_back(1);
	v.push_back(6);
	int k =2 ;
	int ret = s.findKthLargest(v,k);
	cout<<"ret="<<ret<<endl;
}