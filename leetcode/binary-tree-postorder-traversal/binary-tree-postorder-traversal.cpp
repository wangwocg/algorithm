/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    vector<int> postorderTraversal(TreeNode* root) {
        if( root == nullptr ){
			return vector<int>();
		}
		vector<int> ret ;
		stack<TreeNode *> s ;
		s.push(root);
		TreeNode * p,*last=nullptr;
		while(!s.empty()){
			p = s.top();
			if( p->left && p->left == last ){
				if( p->right){
					s.push(p->right);
				}
				else{
					ret.push_back(p->val);
					last = p ;
					s.pop();
				}
			}
			else if( p->right && p->right == last ){
				ret.push_back(p->val);
				last = p ;
				s.pop();
			}
			else{
				if( p->left ){
					while(p->left){
						s.push(p->left);
						p = p->left ;
					}
				}
				else if(p->right){
					s.push(p->right);
				}
				else{
					ret.push_back(p->val);
					last = p ;
					s.pop();
				}
			}
		}
		return ret ;
    }
};