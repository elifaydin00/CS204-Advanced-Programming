#ifndef _BINARYTREE_H
#define _BINARYTREE_H

#include <iostream>
#include "Stack.h"

using namespace std;

template <class itemType>
class BinaryTree
{
 public:
  //Constructor
  BinaryTree();
  //Copy Constructor
  BinaryTree(const BinaryTree<itemType>&);
  //Destructor
  ~BinaryTree();
  
  // Insertion method
  void insert(itemType a);
  TreeNode<itemType>* getRoot();
  
  //operators
  const BinaryTree<itemType>& operator= (const BinaryTree<itemType>& rhs);
  bool operator== (const BinaryTree<itemType>& rhs);
  bool operator!= (const BinaryTree<itemType>& rhs);
  void operator+= (const BinaryTree<itemType>& rhs);
  BinaryTree<itemType> operator+ (const BinaryTree<itemType>& rhs);
  void operator+= (itemType number);
  BinaryTree<itemType> operator+(itemType number);
  
  template <class T>
  friend BinaryTree<T> operator+(T number, const BinaryTree<T>&);
  template <class T>
  friend ostream& operator<<(ostream&, const BinaryTree<T>&);

 private:
  //The root of the tree
  TreeNode<itemType>* root;

  template <class itemType>friend class Iterator;
};

template <class itemType>
class Iterator{
 public:
  //Constructor
  Iterator();

  void Init(const BinaryTree<itemType>& );
  bool hasNext();
  TreeNode<itemType>* Next(); //Actually the same with InOrderNext()
  TreeNode<itemType>* InOrderNext();
  TreeNode<itemType>* PreOrderNext();
  
 private:
  TreeNode<itemType>* myCurrent;
  Stack<itemType>* stack;
  Stack<itemType>* stack2;
};

#include "BinaryTree.cpp"
///////////////////////////


#endif
