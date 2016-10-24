#include <vector>
#include <iostream>
using namespace std ;
/*
For example, given array S = [1, 0, -1, 0, -2, 2], and target = 0.

A solution set is:
[
  [-1,  0, 0, 1],
  [-2, -1, 1, 2],
  [-2,  0, 0, 2]
]
*/
class Solution {
public:
	int threeSum(vector<int>::iterator start, vector<int>::iterator end ,int mytarget ,int current,vector<vector<int> > & res ) {//52ms
    
  //  vector<vector<int> > res;
	int limit = mytarget*2/3,target = 0 ,sum=0;
 //   std::sort(num->begin(), num->end());
	vector<int>::iterator i , front, back ;
    for (i = start; i < end ; i++) {
        target = mytarget-num[i];
        front = i + 1;
        back = end - 1;
		if( target < limit || target < *i ){
            break ;
        }
        while (front < back) {

            sum = *front + *back;
            
            // Finding answer which start from number num[i]
            if (sum < target)
                front++;

            else if (sum > target)
                back--;

            else {
                vector<int> triplet(4, 0);
                triplet[1] =*i;
                triplet[2] = *front;
                triplet[3] = *back;
				triplet[0] = current ;
                res.push_back(triplet);
                
                // Processing duplicates of Number 2
                // Rolling the front pointer to the next different number forwards
                while (front < back && *front == triplet[1]) front++;

                // Processing duplicates of Number 3
                // Rolling the back pointer to the next different number backwards
                while (front < back && *back == triplet[2]) back--;
            }
            
        }

        // Processing duplicates of Number 1
        while (i + 1 < end && *(i+1) == *i ) 
            i++;

    }
    
    return res.size();
    
}
    vector<vector<int> > fourSum(vector<int>& nums, int target) {
		int size = nums.size();
		int limit = target*3/4 ;
		vector<vector<int> > retv ;
		if( size <= 3 ){
			return vector<vector<int> >() ;
		}
		std:sort(nums.begin(),nums.end());
		int i, j , sum,ret,mytarget,last=size-3;
		vector<int>::iterator iter1=nums.begin(),iter2 =nums.end();
    //    multimap<int,pair<int,int> > mymap ;
		for( i=0;i<last;++i,++iter1){
			mytarget = target - nums[i] ;
			if( mytarget < limit ){
				break ;
			}
			vector<vector<int> > res ;
			ret = threeSum(iter1+1,iter2,mytarget ,nums[i] ,res );
			if( ret > 0 ){
				retv.insert(retv.end(),res.begin(),res.end());
			}
		}
		return retv ;
    }
};


class Solution {
private:
    // Valid for K >= 2
    void KSum(int k, vector<int>& nums, int l, int r, int target, vector<vector<int>>& retVal, vector<int>& cur, int ci ) 
    {
        int i, f, mn, mx;
        int km1 = k - 1;

        if ( r-l+1 < k ) return;
        
        f = l; // The first may not be 0 after recursion
        while ( l < r )
        {
            mn = nums[l];
            mx = nums[r];
            
            // If K minus 1 largest + min < target, move to larger
            if ( ( mn + km1*mx ) < target ) l++;
            // If K minus 1 smaller + max > target, move to smaller
            else if ( ( km1*mn + mx ) > target ) r--;
            // If K * min > target, stop looking
            else if ( k*mn > target ) break;
            // If K * min == target, reached the threshold, check then stop looking
            else if ( k*mn == target )
            {
                if ( ( l + km1 <= r ) && ( mn == ( nums[l+km1] ) ) )
                {
                    for ( i = 0; i < k; i++ ) cur[ci+i] = mn;
                    retVal.push_back( cur );
                }
                break;
            }
            // If K * max < target, stop looking
            else if ( k*mx < target ) break;
            // If K * max == target, reached the threshold, check then stop looking
            else if ( k*mx == target )
            {
                if ( ( l <= r - km1 ) && ( mx == ( nums[r-km1] ) ) )
                {
                    for ( i = 0; i < k; i++ ) cur[ci+i] = mx;
                    retVal.push_back( cur );
                }
                break;                
            }
            // If K == 2, we found a match!
            else if ( k == 2 )
            {
                cur[ci] = mn;
                cur[ci+1] = mx;
                retVal.push_back( cur );
                l++;
                while ( ( l < r ) && ( nums[l] == mn ) ) l++;
                r--;
                while ( ( l < r ) && ( nums[r] == mx ) ) r--;
            }
            // Otherwise, convert the problem to a K-1 problem
            else
            {
                cur[ci] = mn;
                KSum( km1, nums, ++l, r, target - mn, retVal, cur, ci+1 );
                while ( ( l < r ) && ( nums[l] == nums[l-1] ) ) l++;
            }
        }
    }

public:
    vector<vector<int>> fourSum(vector<int>& nums, int target) 
    {
        vector<vector<int>> lRetVal;
        vector<int> lQuad( 4, 0 ); // Pre-allocate the size of the result

        // Sort to provide a mechanism for avoiding duplicates
        sort( nums.begin(), nums.end() );
        
        KSum( 4, nums, 0, nums.size()-1, target, lRetVal, lQuad, 0 );

        return( lRetVal );        
    }
};

7、一个n个点m条边的无向图（每条边都连接两个不同的点），要求你给每条边指定一个正整数边权，使得这个图中的所有环上的边权异或(xor)起来是0
8、一个n个点m条边的无向图（每条边都连接两个不同的点），判断图中是否每个环上的边权异或(xor)起来都是0