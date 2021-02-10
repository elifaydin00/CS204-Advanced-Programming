#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include "Stack.h"
#include <vector>

using namespace std;

//Elif Aydin

class BinaryTree
{
 public:
  //Constructor
  BinaryTree();
  //Copy Constructor - TODO in .cpp file
  BinaryTree(const BinaryTree&);
  //Destructor
  ~BinaryTree();
  
  // Insertion method
  void insert(int);
  TreeNode* createClone(TreeNode* root) const;   //create clone for copy constructor NEW
  const BinaryTree& operator=(const BinaryTree&);   //assignment operator =
  bool operator==(const BinaryTree&);   //comparison operator which returns true when 2 trees are equal
  bool operator!=(const BinaryTree&);   //comparison operator which returns false when 2 trees are equal
  void operator+=(const int&);   //+= operator for tree+=int which alters tree
  void operator+=(const BinaryTree&); //+= operator for tree+=tree2  which alters tree
  

private:

	
	bool ihelper(int Value, TreeNode*& node); //insertion helper 
	void dhelper(TreeNode* Tree); //destruction helper 

  //The root of the tree
	TreeNode* root;

	friend class Iterator;
};

class Iterator{
public:
  //Constructor
  Iterator();
  
  void Init(const BinaryTree& );
  bool hasNext();
  TreeNode* Next();
  
private:
  TreeNode* myCurrent;
  Stack* stack;

};

//free functions
ostream& operator<<(ostream& output, const BinaryTree&); //<< operator
const BinaryTree operator+(const BinaryTree&, const int&); //+ operator for tree+int which returns another instance
const BinaryTree operator+(const int& ,const BinaryTree&); //+ operator for int+tree which returns another instance
const BinaryTree operator+(const BinaryTree&, const BinaryTree); //+operator for tree+tree which returns another tree
#endif
