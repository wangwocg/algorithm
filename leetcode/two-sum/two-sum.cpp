#include <vector>
#include <iostream>
using namespace std ;
class Solution {
public:
	/*
    vector<int> twoSum(vector<int>& nums, int target) {//568ms
        int i = 0 , j = 0 ;
		int size = nums.size();
		vector<int> v(2,-1);
		for( i = 0 ; i < size ; ++i ){
			for( j = i + 1 ; j < size ; ++j ){
				if( nums[i] + nums[j] == target ){
					v[0] = i ;
					v[1] = j ;
					return v ;
				}
			}
		}
		return v ;
    }
	*/
	vector<int> twoSum(vector<int>& nums, int target) {//30ms
        int i = 0 , j = 0 ;
		int size = nums.size();
		vector<int> v(2,-1);
		map<int,int> m ;
		map<int,int>::iterator end , it ;
		for( i = 0 ; i < size ; ++i ){
			m[nums[i]] = i ;
		}
		end = m.end();
		for( i = 0 ; i < size ; ++i ){
			if( ( it = m.find(target - nums[i]) ) != end ){
				if( it->second != i ){
					v[0] = i ;
					v[1] = it->second ;
					return v ;
				}
			}
		}
		return v ;
    }
};
//python
class Solution(object)://56ms
    def twoSum(self, nums, target):
        h = {}
        for i, num in enumerate(nums):
            if (target - num) in h:
                return [h[target - num] , i ]
            h[num] = i