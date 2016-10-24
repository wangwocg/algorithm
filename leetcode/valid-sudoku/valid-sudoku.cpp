class Solution {
public:
    bool isValidSudoku(vector<vector<char>>& board) {
		int i = 0 , j = 0 , size = 0 ;
		size = board.size();
		if( size <= 0 ){
			return false ;
		}
		int row = board.size(), col = board[0].size() ;
		unsigned short hash = 0 ;
		int val = 0 ;
		unsigned short rows[9] ;
		memset(rows,0,sizeof(unsigned short)*9);
		for( i=0; i < col ; ++i ){
			hash = 0 ;
			for( j=0; j < row ; ++j ){
				if( board[i][j] != '.' ){
					val = board[i][j] - '0' ;
					if( hash & (1<<val) || rows[j] & (1<<val)){
						return false ;
					}
					else{
						hash |= (1<<val) ;
						rows[j] |= (1<<val);
					}
				}
			}
		}
		int k1=0,k2=0,k3=0,k4=0;
		for( k1=0; k1<9 ; k1+=3 ){
			for( k2=0 ; k2<9 ; k2+=3 ){
				hash = 0 ;
				for( k3=0 ; k3<3 ;++k3 ){
					for( k4=0;k4<3;++k4){
						if( board[k1+k3][k2+k4] != '.' ){
							val = board[k1+k3][k2+k4] - '0' ;
							if( hash & (1<<val) ){
								return false ;
							}
							else{
								hash |= (1<<val) ;
							}
						}
					}
				}
			}
		}
        return true ;
    }
};