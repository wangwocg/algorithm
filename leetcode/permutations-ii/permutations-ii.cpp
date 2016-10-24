class Solution {
public:
	void run( vector<int>&nums , vector<int>::iterator start , vector<int>::iterator end , vector<vector<int> > & ret ){
		if( start >= end ){
			ret.push_back(nums);
			return ;
		}
		int tmp = 0 ;
		vector<int> flag ;
		for( vector<int>::iterator cur = start ; cur!=end ; ++cur ){
			if( find(flag.begin(),flag.end(),*cur) != flag.end() ){
				continue ;
			}
			flag.push_back(*cur);
			swap(*start,*cur);
			run(nums,start+1,end,ret);
			swap(*start,*cur);
		}
		return ;
	}
    vector<vector<int> > permuteUnique(vector<int>& nums) {
        int i = 0 , j = 0 , size = 0 ;
		size = nums.size();
		if( size <= 0 ){
			return vector<vector<int> >();
		}
		vector<vector<int> > ret ;
		run(nums,nums.begin(),nums.end(),ret);
		return ret ;
    }
};