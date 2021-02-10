#include "BinaryTree.h"
#include <iostream>

using namespace std;

//Constructor
BinaryTree::BinaryTree(){
  root = nullptr;
}

//create clone nodes and return root
TreeNode* BinaryTree::createClone(TreeNode* root) const {
	if (root == nullptr) {
		return nullptr;
	}
	TreeNode *newNode = new TreeNode;
	newNode->value = root->value;
	newNode->left = createClone(root->left);
	newNode->right = createClone(root->right);
	return newNode;
}

//Copy Constructor
BinaryTree::BinaryTree(const BinaryTree& rhs){
  //TODO: Implement deep copy constructor
  //You may make use of Iterator class
	TreeNode* start = rhs.root;
	root = createClone(start);	
}

//helper for destructor
void BinaryTree::dhelper(TreeNode* node) {
	if (node!= nullptr) {
		dhelper(node->left);
		dhelper(node->right);
		delete node;
	}
}

//Destructor
BinaryTree::~BinaryTree(){
  //TODO: Implement destructor
	dhelper(root);
}

bool BinaryTree::ihelper(int val, TreeNode*& node) { //insert helper
	if (node == nullptr) {
		node = new TreeNode;
		node->value = val;
		node->left = nullptr;
		node->right = nullptr;

		return true;
	}
	else if (node->value > val) {
		return (ihelper(val, node->left));
	}
	else  if (node->value < val) {
		return (ihelper(val, node->right));
	}
	else  if (node->value == val) {
		return false; //duplicate item
	}
}

void BinaryTree::insert(int num) {
  //TODO: Implement insert function for BinaryTree
	if (!(ihelper(num, root))) {
		cout << num << "Duplicate value found in tree" << endl;
	}
}

//Operators
//Give the implementations of operators in here either as free function or member function

//free ostream operator<< (lhs is ostream)
ostream& operator<<(ostream& output, const BinaryTree& tree) {

	Iterator i;
	i.Init(tree);
	while (i.hasNext())
		output << i.Next()->value << " ";

	return output;
}

//member function assignment opeartor (lhs is BinaryTree)
const BinaryTree& BinaryTree::operator=(const BinaryTree& rhs) {
	//if they are the same objects, their address would be same: We don't need to make assignment then!
	if (this!=&rhs) {
		dhelper(this->root); //delete old data
		this->root = createClone(rhs.root);	//now make a deep copy	
	}
	return *this;
}

//member function ==operator(lhs is BinaryTree), returns true if contains same elements
bool BinaryTree::operator==(const BinaryTree& rhs) {
	vector<int> lhselmt, rhselmt;
	Iterator i1;
	i1.Init(rhs);
	while (i1.hasNext()) {
		rhselmt.push_back(i1.Next()->value);
	}

	Iterator i2;
	i2.Init(*this);
	while (i2.hasNext()) {
		lhselmt.push_back(i2.Next()->value);
	}

	if (rhselmt.size() != lhselmt.size()) { //their sizes are different, no need to check further
		return false;
	}
	else { //their sizes are same, compare the elements
		bool finded = false;
		for (int i = 0; i < rhselmt.size(); i++) {
			for (int j = 0; j < lhselmt.size(); j++) {
				if (rhselmt[i] == lhselmt[j]) {
					finded = true;
				}
			}
			if (!finded) {
				return false;
			}
		}
	}
	return true;
}

//negation of == operator (tree1!=tree2) returns false if (tree1==tree2)
bool BinaryTree::operator!=(const BinaryTree& rhs) {
	if (*this == rhs) {
		return false;
	}
	else {
		return true;
	}
}



//tree+=int, (lhs is a BinaryTree, its a member function)
//adds a node containing that integer to the tree. Alter the nodes of current tree.
void BinaryTree::operator+=(const int& num) {
	if (!(ihelper(num, root))) {
		cout << num << "Duplicate value found in tree" << endl;
	}
}

//tree+=tree2, (lhs is a BinaryTree, its a member function)
// takes tree2 as parameter and adds its nodes to the current tree instance. Alter the nodes of current tree.
void BinaryTree::operator+=(const BinaryTree& rhs) {
	vector<int> rhselmt;
	Iterator i1;
	i1.Init(rhs);
	while (i1.hasNext()) {
		rhselmt.push_back(i1.Next()->value);
	}

	for (int i = 0; i < rhselmt.size(); i++) {
		if (!(ihelper(rhselmt[i], root))) {
			cout << "Duplicate value found in tree" << endl;
		}
	}
}

//+ operator for tree+int which returns another instance
const BinaryTree operator+(const BinaryTree& tree,const int& num) {
	BinaryTree newTree(tree);
	newTree += num;
	return newTree; 
}

//+ operator for int+tree which returns another instance
const BinaryTree operator+(const int& num, const BinaryTree& tree) {
	BinaryTree newTree(tree);
	newTree += num;
	return newTree;
}

//+operator for tree+tree which returns another tree
const BinaryTree operator+(const BinaryTree& rhs, const BinaryTree lhs) {
	
	BinaryTree newTree(lhs);
	newTree += rhs;
	return newTree;
}

/*
 *  ITERATOR CLASS
 */

//Constructor
Iterator::Iterator()
  :myCurrent(nullptr), stack(nullptr)
{}

void Iterator::Init(const BinaryTree &tree)
{
  myCurrent = tree.root;
  stack = new Stack();
}

//Returns true if tree has more nodes to explore
bool Iterator::hasNext()
{
  //TODO: Implement a function to see if there is more nodes to iterate over.
	return (!stack->isEmpty() || myCurrent);
}

//Next node to explore
TreeNode* Iterator::Next()
{
  //TODO: Implement next function of the iterator
  //Note that it returns the next element

	TreeNode* node;
	while (myCurrent) {
		stack->push(myCurrent);
		myCurrent = myCurrent->left;
	}
	myCurrent=stack->pop();
	node = myCurrent;
	myCurrent=myCurrent->right;
	return node;
}


