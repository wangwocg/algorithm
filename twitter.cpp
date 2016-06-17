class User{
public:
	User(int userid):userid(userid),flag(0){
	}
	~User(){
	}
	int getuserid(){
		return userid ;
	}
	int addtweet( int userid , int tweetid ){
		if( flag >= 10 ){
			this->tweetuser.front() = userid ;
			this->tweetid.front() = tweetid ;
		}
		else{
			this->tweetuser.push_front(userid);
			this->tweetid.push_front(tweetid);
			++flag ;
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
	int addfollower(int followerid ){
		if( find(this->followerid.begin(),this->followerid.end(),followerid) == this->followerid.end() ){
			this->followerid.push_back(followerid);
			return 0 ;
		}
		return 1 ;
	}
	
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
		list<int>::iterator iter1 , iter2 ,tmp ;
		for( iter1 = tweetuser.begin(), iter2=tweetid.begin(); iter1 != tweetuser.end() && iter2 != tweetid.end() ; ++iter1 , ++iter2 ){
			if( *iter1 == followerid ){
				iter1 = tweetuser.erase(iter1);
				iter2 = tweetid.erase(iter2);
			}
		}
	}
private:
	int userid ;
	int flag ;
	list <int> tweetuser ;
	list <int> tweetid ;
	vector <int> followerid ;//我关注的人
	vector <int> followeeid ;//关注我的人
};
class Twitter {
public:
    /** Initialize your data structure here. */
    Twitter() {
        
    }
    ~Twitter(){
	}
    /** Compose a new tweet. */
    void postTweet(int userId, int tweetId) {
		vector<User>::iterator iter ;
        for( iter = user.begin() ; iter != user.end() ; ++iter ){
			if( iter->getuserid() == userId ){
				iter->addtweet(userId,tweetId);
				vector<int> &followee = iter->getfollowee();
				for( vector<int>::iterator it = followee.begin() ; it != followee.end(); ++it ){
					for( vector<User>::iterator its = user.begin() ; its != user.end(); ++its){
						if( *it == its->getuserid() ){
							its->addtweet(userId,tweetId);
						}
					}
				}
				break ;
			}
		}
		if( iter == user.end() ){
			user.push_back(User(userId));
			user.back().addtweet(userId,tweetId);
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
    void follow(int followerId, int followeeId) {
		int flag = 0 ;
		int flag1 = 0 ;
		int ret = 0 ;
        for( vector<User>::iterator iter = user.begin(); iter != user.end() ;++iter){
			if( iter->getuserid() == followerId ){
				flag1 = 1 ;
				ret = iter->addfollower(followeeId);
				if( ret == 1 ){
					return ;
				}
			}
			if( iter->getuserid() == followeeId ){
				flag = 1 ;
				iter->addfollowee(followerId);
			}
			if( flag==1 && flag1 == 1 ){
				break ;
			}
		}
		if( flag == 0 ){
			user.push_back(User(followeeId));
			user.back().addfollowee(followerId);
		}
		if( flag1 == 0 ){
			user.push_back(User(followerId));
			user.back().addfollower(followeeId);
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
};

/**
 * Your Twitter object will be instantiated and called as such:
 * Twitter obj = new Twitter();
 * obj.postTweet(userId,tweetId);
 * vector<int> param_2 = obj.getNewsFeed(userId);
 * obj.follow(followerId,followeeId);
 * obj.unfollow(followerId,followeeId);
 */