#include "BinaryTree.h"
#include <iostream>

using namespace std;

//Constructor
template <class itemType>
BinaryTree<itemType>::BinaryTree(){
  root = nullptr;
}

//Copy Constructor
template <class itemType>
BinaryTree<itemType>::BinaryTree(const BinaryTree<itemType>& rhs){
  
  root = nullptr;
  
  Iterator<itemType> iter;
  iter.Init(rhs);
    
  while(iter.hasNext()){
    this->insert(iter.InOrderNext()->value);
  }
}

//Destructor
template <class itemType>
BinaryTree<itemType>::~BinaryTree(){
  Iterator<itemType> iter;
  iter.Init(*this);
    
  while(iter.hasNext()){
    delete iter.InOrderNext();
  }
}

template <class itemType>
TreeNode<itemType>* BinaryTree<itemType>::getRoot(){
  return this->root;
}

//First create the new node with the given value
template <class itemType>
void BinaryTree<itemType>::insert(itemType num) {
  TreeNode<itemType>*newNode;	// Pointer to a new node
  TreeNode<itemType>*nodePtr;	// Pointer to traverse the tree
  
  // Create a new node
  newNode = new TreeNode<itemType>;
  newNode->value = num;
  newNode->left = newNode->right = nullptr;
  
  if (!root)	// Is the tree empty?
    root = newNode;
  else 
    {
      //we have a root, need to find the location for the next item
      nodePtr = root; 
      while (nodePtr) {
	if (num < nodePtr->value) {
	  if (nodePtr->left)
	    nodePtr = nodePtr->left;
	  else {
	    nodePtr->left = newNode;
	    break;
	  }
	}
	else if (num > nodePtr->value) {
	  if (nodePtr->right)
	    nodePtr = nodePtr->right;
	  else {
	    nodePtr->right = newNode;
	    break;
	  }
	}
	else { //num is not greater or smaller. So it is equal
	  cout << "Duplicate value found in tree.\n";
	  break;
	}
      }		
    }
}

//Operators

template <class itemType>
const BinaryTree<itemType>& BinaryTree<itemType>::operator= (const BinaryTree<itemType>& rhs){
  if(this!=&rhs)
    {
      this->~BinaryTree(); //deleteList
      this->root = nullptr;
      
      Iterator<itemType> iter;
      iter.Init(rhs);
      while (iter.hasNext()) {
	this->insert(iter.Next()->value);
      }
    }
  return *this;
}

template <class itemType>
bool BinaryTree<itemType>::operator== (const BinaryTree<itemType>& rhs){
  if(this != &rhs){
    Iterator<itemType> iter_lhs, iter_rhs;
    iter_lhs.Init(*this);
    iter_rhs.Init(rhs);
    while (iter_lhs.hasNext() && iter_rhs.hasNext()) {
      if(iter_lhs.Next()->value != iter_rhs.Next()->value){
	return false;
      }
    }
    if(iter_lhs.hasNext() || iter_rhs.hasNext()){ //not equal sizes
      return false;
    }
  }
  return true;
}

template <class itemType>
bool BinaryTree<itemType>::operator!= (const BinaryTree<itemType>& rhs){
  return !operator==(rhs);
}

template <class itemType>
void BinaryTree<itemType>::operator+=(const BinaryTree<itemType>& rhs){
  Iterator<itemType> iter;
  iter.Init(rhs);
  while (iter.hasNext()) {
    this->insert(iter.Next()->value);
  }
}

template <class itemType>
BinaryTree<itemType> BinaryTree<itemType>::operator+(const BinaryTree<itemType>& rhs){
  BinaryTree<itemType> temp(*this);
  temp += rhs;
  return temp;
}

template <class itemType>
void BinaryTree<itemType>::operator+=(itemType number){
  this->insert(number);
}

template <class itemType>
BinaryTree<itemType> BinaryTree<itemType>::operator+(itemType number){
  BinaryTree<itemType> temp(*this);
  temp.insert(number);
  return temp;
}

template <class itemType>
BinaryTree<itemType> operator+(itemType number, const BinaryTree<itemType>& tree){
  BinaryTree<itemType> temp(tree);
  temp.insert(number);
  return temp;
}

template <class itemType>
ostream & operator<< (ostream & os, const BinaryTree<itemType>& rhs)
{
  Iterator<itemType> iter;
  iter.Init(rhs);
  while (iter.hasNext()) {
    os << iter.Next()->value << " ";
  }
  os << endl;
  return os;
}


/*
 *  ITERATOR CLASS
 */

//Constructor
template <class itemType>
Iterator<itemType>::Iterator()
  :myCurrent(nullptr), stack(nullptr), stack2(nullptr)
{}

template <class itemType>
void Iterator<itemType>::Init(const BinaryTree<itemType>&tree)
{
  myCurrent = tree.root;
  stack = new Stack<itemType>();
}

template <class itemType>
bool Iterator<itemType>::hasNext()
{
  return (!stack->isEmpty() || myCurrent != nullptr);
}

template <class itemType>
TreeNode<itemType>* Iterator<itemType>::Next()
{
  while (myCurrent != nullptr) {
    stack->push(myCurrent);
    myCurrent = myCurrent->left;
  }
  
  myCurrent = stack->pop();
  TreeNode<itemType>* node = myCurrent;
  myCurrent = myCurrent->right;
  
  return node;
}

template <class itemType>
TreeNode<itemType>* Iterator<itemType>::InOrderNext()
{
  while (myCurrent != nullptr) {
    stack->push(myCurrent);
    myCurrent = myCurrent->left;
  }
  
  myCurrent = stack->pop();
  TreeNode<itemType>* node = myCurrent;
  myCurrent = myCurrent->right;
  
  return node;
}


//This is just to show. Iterative post-order is much more complex.
template <class itemType>
TreeNode<itemType>* Iterator<itemType>::PreOrderNext()
{
  if(myCurrent->left != nullptr || myCurrent->right != nullptr){
    stack->push(myCurrent);
  }
  
  TreeNode<itemType> * node = myCurrent;

  if(myCurrent->left == nullptr && myCurrent->right == nullptr){
    if(!stack->isEmpty()){
      myCurrent = stack->pop();
    }
    else{
      myCurrent = nullptr;
      return node;
    }
    if(myCurrent->right){
      myCurrent = myCurrent->right;
    }
    else{
      while(!myCurrent->right){
	myCurrent = stack->pop();
	myCurrent = myCurrent->right;
      }
    }   
  }
  else if(myCurrent->left){
    myCurrent = myCurrent -> left;
  }
  else if(myCurrent->right){
    myCurrent = stack->pop();
    myCurrent = myCurrent -> right;
  }

  return node;
}


  
