class User{
public:
	User(int userid):userid(userid),flag(0),mytweetnum(0){
	/*
		tweetuser.resize(20) ;
        tweetid.resize(20) ;
        tweetdate.resize(20) ;
        mytweetid.resize(20) ;
        mytweetdate.resize(20) ;
	*/
	}
	~User(){
	}
	int getuserid(){
		return userid ;
	}
	int addtweet( int userid , int tweetid ,int tweetdate ){
		if( flag >= 10 ){
			this->tweetuser.push_front(userid);
			this->tweetid.push_front(tweetid);
			this->tweetdate.push_front(tweetdate);
			
			this->tweetuser.pop_back();
			this->tweetid.pop_back();
			this->tweetdate.pop_back();
		}
		else{
			this->tweetuser.push_front(userid);
			this->tweetid.push_front(tweetid);
			this->tweetdate.push_front(tweetdate);
			++flag ;
		}
		return 0 ;
	}
	int addmytweet( int tweetid ,int tweetdate){
		if( mytweetnum >= 10 ){
			this->mytweetid.front() = tweetid ;
			this->mytweetdate.front() = tweetdate ;
		}
		else{
			this->mytweetid.push_front(tweetid);
			this->mytweetdate.push_front(tweetdate);
			++mytweetnum ;
		}
		return 0 ;
	}
	vector <int> getnewsfeed(){
		return vector<int>(tweetid.begin(),tweetid.end());
	}
	vector<int> & getfollowee(){
		return followeeid ;
	}
	int addfollowee(int followeeid ){
		if( find(this->followeeid.begin(),this->followeeid.end(),followeeid) == this->followeeid.end() ){
			this->followeeid.push_back(followeeid);
			return 0 ;
		}
		return 1 ;
	}
	//返回1则表示已经存在了，不用再关注
	int addfollower(int followerid ){
		if( followerid == userid ){
			return 1 ;
		}
		if( find(this->followerid.begin(),this->followerid.end(),followerid) == this->followerid.end() ){
			this->followerid.push_back(followerid);
			return 0 ;
		}
		return 1 ;
	}
	//返回1表示这个关注者存在
	int decfollowee(int followeeid ){
		vector<int>::iterator iter = find(this->followeeid.begin(),this->followeeid.end(),followeeid) ;
		if( iter == this->followeeid.end() ){
			return 0 ;
		}
		this->followeeid.erase(iter);
		return 1 ;
	}
	int decfollower(int followerid ){
		vector<int>::iterator iter = find(this->followerid.begin(),this->followerid.end(),followerid) ;
		if( iter == this->followerid.end() ){
			return 0 ;
		}
		this->followerid.erase(iter);
		deltweet(followerid);
		return 1 ;
	}
	void deltweet(int followerid){
		list<int>::iterator iter1 , iter2 ,iter3,tmp ;
		for( iter1 = tweetuser.begin(), iter2=tweetid.begin() , iter3 = tweetdate.begin() ; iter1 != tweetuser.end() && iter2 != tweetid.end()&& iter3 != tweetdate.end() ; ++iter1 , ++iter2,++iter3 ){
			if( *iter1 == followerid ){
				iter1 = tweetuser.erase(iter1);
				iter2 = tweetid.erase(iter2);
				iter3 = tweetdate.erase(iter3);
			}
		}
	}
	list <int> & gettweetuser(){
		return tweetuser ;
	}
	list <int> & gettweetid(){
		return tweetid ;
	}
	list <int> & gettweetdate(){
		return tweetdate ;
	}
	list <int> & getmytweetid(){
		return mytweetid ;
	}
	list <int> & getmytweetdate(){
		return mytweetdate ;
	}
	int & getflag(){
		return flag ;
	}
private:
	int userid ;
	int flag ;
	int mytweetnum ;
	list <int> tweetuser ;
	list <int> tweetid ;
	list <int> tweetdate ;
	list <int> mytweetid ;
	list <int> mytweetdate ;
	vector <int> followerid ;//我关注的人
	vector <int> followeeid ;//关注我的人
};
class Twitter {
public:
    /** Initialize your data structure here. */
    Twitter() {
        tweetnum = 0 ;
    }
    ~Twitter(){
	}
    /** Compose a new tweet. */
    void postTweet(int userId, int tweetId) {
		vector<User>::iterator iter ;
		++tweetnum;
        for( iter = user.begin() ; iter != user.end() ; ++iter ){
			if( iter->getuserid() == userId ){
				iter->addmytweet(tweetId,tweetnum);
				iter->addtweet(userId,tweetId,tweetnum);
				vector<int> &followee = iter->getfollowee();
				for( vector<int>::iterator it = followee.begin() ; it != followee.end(); ++it ){
					for( vector<User>::iterator its = user.begin() ; its != user.end(); ++its){
						if( *it == its->getuserid() ){
							its->addtweet(userId,tweetId,tweetnum);
						}
					}
				}
				break ;
			}
		}
		if( iter == user.end() ){
			user.push_back(User(userId));
			user.back().addtweet(userId,tweetId,tweetnum);
			user.back().addmytweet(tweetId,tweetnum);
		}
    }
    
    /** Retrieve the 10 most recent tweet ids in the user's news feed. Each item in the news feed must be posted by users who the user followed or by the user herself. Tweets must be ordered from most recent to least recent. */
    vector<int> getNewsFeed(int userId) {
        for(vector<User>::iterator iter = user.begin() ; iter != user.end() ; ++iter){
			if(iter->getuserid() == userId ){
				return iter->getnewsfeed();
			}
		}
		return vector<int>();
    }
    
    /** Follower follows a followee. If the operation is invalid, it should be a no-op. */
	void updatetweetid( User *user , int userid2 ,list <int> * tweetid2 , list <int> * tweetdate2 ){
		list <int> * tweetuser = &user->gettweetuser() ;
		list <int> * tweetid = &user->gettweetid() ;
		list <int> * tweetdate = &user->gettweetdate() ;
		int  *flag = &user->getflag() ;
		list <int>::iterator iter1 , iter2 , iteruser, iterid ,iterid2 ;
		for(iteruser = tweetuser->begin() ,iterid = tweetid->begin(),iterid2 = tweetid2->begin() ,iter1 = tweetdate->begin() , iter2 = tweetdate2->begin(); iter1 != tweetdate->end() && iter2 != tweetdate2->end() ; ){
			if( *iter2 > *iter1 ){
				
				tweetdate->insert(iter1,*iter2);
				tweetuser->insert(iteruser , userid2);
				tweetid->insert( iterid, *iterid2 );
				++iter2 ;
				++iterid2 ;
				if( *flag >= 10 ){
					if( ++iter1 != tweetdate->end() ){
						--iter1 ;
						tweetdate->pop_back();
						tweetuser->pop_back( );
						tweetid->pop_back(   );
					}
					else{
						tweetdate->pop_back();
						tweetuser->pop_back( );
						tweetid->pop_back(   );
						iter1 = tweetdate->end() ;
						iteruser = tweetuser->end();
						iterid = tweetid->end();
					}
				}
				
				else{
					++*flag ;
				}
			}
			else{
				++iter1;
				++iterid;
				++iteruser;
			}
		}
		
		if( iter2 != tweetdate2->end() && *flag < 10 ){
			while( iter2 != tweetdate2->end()&& *flag < 10 ){
				tweetdate->insert(iter1,*iter2++);
				tweetid->insert(iterid,*iterid2++);
				tweetuser->insert(iteruser,userid2);
				++*flag ;
			}
		}
		
	}
    void follow(int followerId, int followeeId) {
		int flag = 0 ;
		int flag1 = 0 ;
		int ret = 0 , i = 0 ,followeri=-1, followeei = -1 ;
		User *user1 =NULL;
		list <int> * tweetid2 =NULL, * tweetdate2 =NULL;
        for( vector<User>::iterator iter = user.begin(); iter != user.end() ;++iter,++i){
			if( iter->getuserid() == followerId ){
				flag1 = 1 ;
				ret = iter->addfollower(followeeId);
				user1 = &*iter ;
				followeri = i ;
				if( ret == 1 ){
					return ;
				}
			
				
			}
			if( iter->getuserid() == followeeId ){
				flag = 1 ;
				followeei = i ;
			
			}
			if( flag==1 && flag1 == 1 ){
				break ;
			}
		}
		
		if( flag1 == 0 ){
			user.push_back(User(followerId));
			user.back().addfollower(followeeId);
			user1 = &user.back();
			
			
		}
		else{
			user1 = &user[followeri];
		}
		if( flag != 0){
			user[followeei].addfollowee(followerId);
				tweetid2 = &user[followeei].getmytweetid() ;
				tweetdate2 = &user[followeei].getmytweetdate() ;
			updatetweetid( user1 , followeeId, tweetid2 ,  tweetdate2 );
		}
		else{
			user.push_back(User(followeeId));
			user.back().addfollowee(followerId);
		}
    }
    
    /** Follower unfollows a followee. If the operation is invalid, it should be a no-op. */
    void unfollow(int followerId, int followeeId) {
        int flag = 0 ;
		int flag1 = 0 ;
		int ret = 0 ;
        for( vector<User>::iterator iter = user.begin(); iter != user.end() ;++iter){
			if( iter->getuserid() == followerId ){
				flag1 = 1 ;
				ret = iter->decfollower(followeeId);
				if( ret == 0 ){
					return ;
				}
			}
			if( iter->getuserid() == followeeId ){
				flag = 1 ;
				iter->decfollowee(followerId);
			}
			if( flag==1 && flag1 == 1 ){
				break ;
			}
		}
		/*
		if( flag == 0 ){
			user.push_back(User(followeeId));
			user.back().addfollowee(followerId);
		}
		if( flag1 == 0 ){
			user.push_back(User(followerId));
			user.back().addfollower(followeeId);
		}
		*/
    }
private:
	vector <User> user ;
	int tweetnum;
};



class Twitter {
public:
    /** Initialize your data structure here. */
    Twitter() {
        timestamp=0;
    }
    /** Compose a new tweet. */
    void postTweet(int userId, int tweetId) {
        timestamp++;
        push(posts[userId],{timestamp,tweetId});
        for(auto i:followeeMap[userId]){
            push(news[i],{timestamp,tweetId});
        }
        push(news[userId],{timestamp,tweetId});
    }

    /** Retrieve the 10 most recent tweet ids in the user's news feed. Each item in the news feed must be posted by users who the user followed or by the user herself. Tweets must be ordered from most recent to least recent. */
    vector<int> getNewsFeed(int userId) {
        vector<int> tmp;
        for(int i=0;i<news[userId].size();i++){
            tmp.push_back(news[userId][i].second);
        }
        return tmp;
    }

    /** Follower follows a followee. If the operation is invalid, it should be a no-op. */
    void follow(int followerId, int followeeId) {
         if(followerMap[followerId].count(followeeId)||followerId==followeeId)return;
        followeeMap[followeeId].insert(followerId);
        followerMap[followerId].insert(followeeId);
        push(news[followerId],posts[followeeId]);
    }
    /** Follower unfollows a followee. If the operation is invalid, it should be a no-op. */
    void unfollow(int followerId, int followeeId) {
         if(!followerMap[followerId].count(followeeId))return;
        followerMap[followerId].erase(followeeId);
        followeeMap[followeeId].erase(followerId);
        vector<pair<int,int>> tmp=posts[followerId];
        for(auto i:followerMap[followerId]){
            push(tmp,posts[i]);
        }
        news[followerId]=tmp;
    }
private:
    int timestamp;
    unordered_map<int,vector<pair<int,int>>> news;
    unordered_map<int,vector<pair<int,int>>> posts;
    unordered_map<int,unordered_set<int>> followeeMap;
    unordered_map<int,unordered_set<int>> followerMap;
    void push(vector<pair<int,int>> &num,pair<int,int> i){
        if(num.size()==10)num.pop_back();
        num.insert(num.begin(),i);
    }
    void push(vector<pair<int,int>> &num,vector<pair<int,int>> &num1){
        num.insert(num.end(),num1.begin(),num1.end());
        sort(num.begin(),num.end(),compare);
        if(num.size()>10)num.erase(num.begin()+10,num.end());
    }
    static bool compare(pair<int,int>  a,pair<int,int> b) {
        return a.first>b.first;
    }
};






/**
 * Your Twitter object will be instantiated and called as such:
 * Twitter obj = new Twitter();
 * obj.postTweet(userId,tweetId);
 * vector<int> param_2 = obj.getNewsFeed(userId);
 * obj.follow(followerId,followeeId);
 * obj.unfollow(followerId,followeeId);
 */