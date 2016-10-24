#include <vector>
#include <iostream>
using namespace std ;
/*
For example, given array S = [-1, 0, 1, 2, -1, -4],

A solution set is:
[
  [-1, 0, 1],
  [-1, -1, 2]
]
*/
class Solution {
public:
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
    vector<vector<int>> threeSum(vector<int>& nums) {
        int size = nums.size();
		if( size <= 0 ){
			return vector<vector<int> >();
		}
		vector<vector<int> > ret ;
		vector<int> flag(size,-1);
		multimap<int,pair<int,int> > mymap ;
		int i = 0 , j = 0 ;
		for( i = 0 ; i < size ; ++i ){
			for( j = i+1 ; j < size ; ++j ){
				
			}
		}
    }
};

vector<vector<int> > threeSum(vector<int> &num) {//52ms
    
    vector<vector<int> > res;

    std::sort(num.begin(), num.end());

    for (int i = 0; i < num.size(); i++) {
        
        int target = -num[i];
        int front = i + 1;
        int back = num.size() - 1;
		if( target < 0 ){
            break ;
        }
        while (front < back) {

            int sum = num[front] + num[back];
            
            // Finding answer which start from number num[i]
            if (sum < target)
                front++;

            else if (sum > target)
                back--;

            else {
                vector<int> triplet(3, 0);
                triplet[0] = num[i];
                triplet[1] = num[front];
                triplet[2] = num[back];
                res.push_back(triplet);
                
                // Processing duplicates of Number 2
                // Rolling the front pointer to the next different number forwards
                while (front < back && num[front] == triplet[1]) front++;

                // Processing duplicates of Number 3
                // Rolling the back pointer to the next different number backwards
                while (front < back && num[back] == triplet[2]) back--;
            }
            
        }

        // Processing duplicates of Number 1
        while (i + 1 < num.size() && num[i + 1] == num[i]) 
            i++;

    }
    
    return res;
    
}


















