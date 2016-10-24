#include <iostream>
#include <vector>
#include <cstring>
using namespace std ;
class Solution {
public:
	int set( vector<vector<char> >&board , int i , int j , unsigned short (&hash)[9] , unsigned short (&rows)[9] ,unsigned short (&sub)[9] ,int & ifmatch ){
		if( i >= 9 || j >= 9 ){
			ifmatch = 1 ;
			return 0 ;
		}
		int k = 0 ,ret = 0 ;
		unsigned short val =0;
		if( board[i][j] == '.' ){
			for( k=1;k<=9;++k){
				board[i][j] = '0' + k ;
				val = 1<<k ;
				if( hash[i]&(val) || rows[j]&(val) || sub[(i/3)*3+j/3]&val){
					continue ;
				}
				else{
					hash[i] |= val ;
					rows[j] |= val ;
					sub[(i/3)*3+j/3] |= val ;
				}
				if( j< 8 ){
					set(board,i,j+1,hash,rows,sub,ifmatch);
				}
				else{
					set(board,i+1,0,hash,rows,sub,ifmatch);
				}
				if(ifmatch){
					return 0 ;
				}
				hash[i] &= ~val ;
				rows[j] &= ~val ;
				sub[(i/3)*3+j/3] &= ~val ;
			}
			board[i][j] = '.';
		}
		else{
			
			if( j< 8 ){
				set(board,i,j+1,hash,rows,sub,ifmatch);
			}
			else{
				set(board,i+1,0,hash,rows,sub,ifmatch);
			}
			
		}
		return 0 ;
	}
    void solveSudoku(vector<vector<char> >& board) {
        int i = 0 , j = 0 , val,size = 0 ,ifmatch = 0 ;
		size = board.size();
		if( size <= 0 ){
			return  ;
		}
		unsigned short hash[9] ;
		memset(hash,0,sizeof(unsigned short)*9);
		unsigned short rows[9] ;
		memset(rows,0,sizeof(unsigned short)*9);
		unsigned short sub[9] ;
		memset(sub,0,sizeof(unsigned short)*9);
		for(i=0;i<size;++i){
                        for(j=0;j<board[i].size();++j){
                                if( board[i][j] == '.'){
                                        continue ;
                                }
                                val = 1<<(board[i][j]-'0');
                                hash[i] |= val ;
                                rows[j] |= val ;
                                sub[(i/3)*3+j/3] |= val ;
                        }
        }
		set(board,0,0,hash,rows,sub,ifmatch);
    }
};
int main(){
	Solution s ;
	    char c[9][9]={	{'.','.','9','7','4','8','.','.','.'},	\
						{'7','.','.','.','.','.','.','.','.'},
						{'.','2','.','1','.','9','.','.','.'},	\
						{'.','.','7','.','.','.','2','4','.'},	\
						{'.','6','4','.','1','.','5','9','.'},	\
						{'.','9','8','.','.','.','3','.','.'},	\
						{'.','.','.','8','.','3','.','2','.'},	\
						{'.','.','.','.','.','.','.','.','6'},	\
						{'.','.','.','2','7','5','9','.','.'}};
        vector<char> a0(&c[0][0],&c[0][8]+1);
        vector<char> a1(&c[1][0],&c[1][8]+1);
        vector<char> a2(&c[2][0],&c[2][8]+1);
        vector<char> a3(&c[3][0],&c[3][8]+1);
        vector<char> a4(&c[4][0],&c[4][8]+1);
        vector<char> a5(&c[5][0],&c[5][8]+1);
        vector<char> a6(&c[6][0],&c[6][8]+1);
        vector<char> a7(&c[7][0],&c[7][8]+1);
        vector<char> a8(&c[8][0],&c[8][8]+1);
        vector<vector<char> > board ;
        board.push_back(a0);
        board.push_back(a1);
        board.push_back(a2);
        board.push_back(a3);
        board.push_back(a4);
        board.push_back(a5);
        board.push_back(a6);
        board.push_back(a7);
        board.push_back(a8);
		
		s.solveSudoku(board);
		cout<<"sudoku:"<<endl;
		for( int i=0 , j=0;i<board.size();++i){
			for( j=0;j<board[i].size();++j){
				cout<<board[i][j]<<" ";
			}
			cout<<endl;
		}
}