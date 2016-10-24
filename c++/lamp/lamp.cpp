#include <iostream>
#include <vector>
#include <cstdio>
using namespace std ;

int main(){
  int n = 0 , l = 0 , index = 0 , i = 0 ;
 while( cin>>n>>l){
 vector<bool> ret (l+1,0) ;
  for(i = 0 ; i < n ; ++i){
    cin>>index;
    ret[index] = 1 ;
  }
  int last = 0 , max = 0 , flag = 0 ;
  for(i=0;i<=l;++i){
    if( ret[i] == 1 ){
      if( flag == 0 ){
        last = i ;
        flag = 1 ;
      }
      else{
       if( i-last > max ){
         max = i - last ;
       }
        last = i ;
      }
    }
  }
  float result = max*1.0/2 ;
  printf("%.2f\n",result);
 }
  return 0 ;
}