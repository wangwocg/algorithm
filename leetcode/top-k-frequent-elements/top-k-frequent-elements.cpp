#include <iostream>
#include <list>
#include <unordered_map>
#include <vector>
#include <utility>
using namespace std;
140ms
class Solution {
public:

	int insertKFrequent(int currentnum , int freq ){
		int num = klist.size() ;
		if( num == 0 ){
			klist.push_front(pair<int,int>(currentnum,freq));
			++num ;
		}
		else{
			list<pair<int,int> >::iterator iter = klist.begin() ;
			while( iter != klist.end() ){
				if( iter->second > freq ){
					++iter ;
				}
				else{
					break ;
				}
			}
			klist.insert(iter,make_pair(currentnum,freq));
			++num ;
		}
		if( num > knum ){
			--num ;
			klist.pop_back();
		}
		return 0 ;
	}
    vector<int> topKFrequent(vector<int>& nums, int k) {
		unordered_map<int,int> hash ;
		vector<int>::iterator iter , end ;
		int freq = 0 ;
		knum = k ;
		end = nums.end() ;
		for( iter = nums.begin(); iter != end ; ++iter){
			++hash[*iter] ;
		}
		unordered_map<int, int >::iterator iter1 = hash.begin(), iter2 = hash.end() ;
		for( ;iter1 != iter2 ; ++iter1 ){
			insertKFrequent(iter1->first , iter1->second );
		}
		vector<int> ret ;
		for( list<pair<int,int> >::iterator it = klist.begin(); it != klist.end() ; ++it ){
			ret.push_back(it->first);
		}
		return ret ;
    }
private :
	list<pair<int,int> > klist ;
	int knum ;
};

int main(){
	Solution s ;
	vector<int> v ={1};
	int k = 1 ;
	vector<int> ret = s.topKFrequent(v,k);
	for( vector<int>::iterator it = ret.begin();it != ret.end();++it){
		cout<<*it<<" ";
	}
	cout<<endl;
}

23ms
private void swap(Node[] myNode, int x, int y) {
    Node tmp = myNode[x];
    myNode[x] = myNode[y];
    myNode[y] = tmp;
}

private int partition(int pivot, int start, int end, Node[] myNode) {
    int i = start;
    int j = end;
    swap(myNode, start, pivot);
    while (i < j) {
        while (i < j && myNode[j].count < myNode[start].count) {
            j--;
        }

        while (i < j && myNode[i].count >= myNode[start].count) {
            i++;
        }
        swap(myNode, i, j);
    }
    swap(myNode, start, j);
    return j;
}

public List<Integer> topKFrequent(int[] nums, int k) {
    PriorityQueue<Node> pq = new PriorityQueue<>(1, new Comparator<Node>() {
        public int compare(Node left, Node right) {
            return right.count - left.count;
        }
    });

    HashMap<Integer, Node> map = new HashMap<Integer, Node>();

    for (int num : nums) {
        if (!map.containsKey(num)) {
            Node node = new Node(num, 0);
            map.put(num, node);
        }
        map.get(num).count++;
    }

    Node[] myNode = new Node[map.size()];
    int i = 0;
    for (Node now : map.values()) {
        myNode[i++] = now;
    }

    int start = 0;
    int end = myNode.length;
    Random rand = new Random();
    ArrayList<Integer> res = new ArrayList<Integer>();
    while (start < end) {
        int pivot = start + rand.nextInt(end - start);
        int pos = partition(pivot, start, end - 1, myNode);
        if (pos == k - 1) {
            int j = 0;
            while (k > 0) {
                res.add(myNode[j++].val);
                k--;
            }
            break;
        } else if (pos < k - 1) {
            start = pos;
        } else {
            end = pos;
        }
    }

    return res;
}