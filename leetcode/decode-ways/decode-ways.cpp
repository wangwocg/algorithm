#include <iostream>
#include <vector>
using namespace std ;

public class Solution {
Basic idea:

Determine number of decodings of all prefixes.
If current digit is anything besides 0 (i.e. it can make a letter on its own), all the solutions for the 1-smaller prefix can be extended.
If current digit and preceding digit make a valid combination, all the solutions for the 2-smaller prefix are also applicable.
    public int numDecodings(String s) {
        int score = 0;
        int prevPrevScore = 0;
        int prevScore = 1;
        int prevNum = 10;

        for (char c : s.toCharArray()) {
            int num = c - '0';
            if (num >= 1) {
                score = prevScore;
            } else {
                score = 0;
            }
            if ((prevNum == 1) || (prevNum == 2 && num <= 6)) score += prevPrevScore;
            prevPrevScore = prevScore;
            prevScore = score;
            prevNum = num;
        }

        return score;
    }
}
int numDecodings(string s) {
    int i, j = 1, k = 0;
    char pre = '3'; // any except '1' and '2'
    for (char cur : s) {
        k = 0;
        if (cur > '0') // decode 1 digit
            k += j;
        if (pre == '1' || (pre == '2' && cur <= '6')) // decode 2 digits
            k += i;
        if (k == 0) // prune
            break;
        i = j;
        j = k;
        pre = cur;
    }
    return k;
}


class Solution {
public:
	int getcount(string & s , int end ){
		if( end < 0 ){
			return 0 ;
		}
		if( end == 0 ){
			if( s[0] != '0' ){
				return 1 ;
			}
			return 0 ;
		}
		if( end == 1 ){
			if( s[0] == '0' ){
				return 0 ;
			}
			else if( s[0] == '1' ){
				if( s[1] != '0' ){
					return 2 ;
				}
				return 1 ;
			}
			else if ( s[0] == '2' ){
				if( s[1] == '0'){
					return 1 ;
				}
				else if ( s[1] >= '1' && s[1] <= '6'){
					return 2 ;
				}
				return 1 ;
			}
			if( s[1] == '0' ){
				return 0 ;
			}
			return 1 ;
		}
		return getcount(s,end-1) + getcount(s,end-2) ;
	}
    int numDecodings(string s) {
		return getcount(s,s.size()-1);
	}
};


class Solution {//4ms
public:

    int numDecodings(string s) {
        int size = s.size();
		int first = 0 , second = 0 , i = 0 , count = 0 , current = 0 ;
		if( size == 0 ){
		    return 0 ;
		}
		if( size >= 3 ){
			if( s[0] == '0' ){
				return 0 ;
			}
			else{
				first = 1 ;
			}
			if( s[0] == '1' ){
				if( s[1] != '0' ){
					second = 2 ;
				}
				else{
					second = 1 ;
				}
			}
			else if( s[0] == '2' ){
				if( s[1] != '0' ){
					if( s[1] >= '1' && s[1] <= '6' ){
						second = 2 ;
					}
					else{
						second = 1 ;
					}
				}
				else{
					second = 1 ;
				}
			}
			else{
			    if(s[1] == '0' ){
			        return 0 ;
			    }
			    second = 1 ;
			}
			i = 2 ;
			while( i < size ){
				if( s[i] == '0' ){
					if( s[i-1] >= '1' && s[i-1] <= '2' ){
						current = first ;
					}
					else{
						return 0 ;
					}
				}
				else{
					if( s[i-1] == '1' ){
						current = first + second ;
					}
					else if ( s[i-1] == '2' ){
						if( s[i] >= '1' && s[i] <= '6' ){
							current = first + second ;
						}
						else{
							current = second ;
						}
					}
					else{
						current = current = second ;
					}
				}
				first = second ;
				second = current ;
				++i ;
			}
			return current ;
		}
		else if ( size == 2 ){
			if( s[0] == '0' ){
				return 0 ;
			}
			else if( s[0] == '1' ){
				if( s[1] != '0' ){
					return 2 ;
				}
				return 1 ;
			}
			else if ( s[0] == '2' ){
				if( s[1] == '0'){
					return 1 ;
				}
				else if ( s[1] >= '1' && s[1] <= '6'){
					return 2 ;
				}
				return 1 ;
			}
			if( s[1] == '0' ){
				return 0 ;
			}
			return 1 ;
		}
		if( s[0] != '0' ){
		    return 1 ;
		}
		return 0 ;
		
    }
};

int main(){
	
}