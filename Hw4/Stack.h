#ifndef _STACK_H
#define _STACK_H

#include <iostream>

template <class itemType>
struct TreeNode
{
  itemType value;
  TreeNode<itemType>*left;
  TreeNode<itemType>*right;

  TreeNode(){
    left = nullptr;
    right = nullptr;
  }
};

template <class itemType>
struct StackNode
{
  TreeNode<itemType>* value;
  StackNode<itemType>*next;
};

template <class itemType>
class Stack
{
private:
  StackNode<itemType> *top;
  StackNode<itemType>* GetTopPointer(Stack<itemType> myStack);
  
public:
  Stack(void);
  void push(TreeNode<itemType>* );
  TreeNode<itemType>* pop();
  bool isEmpty();
};

#include "Stack.cpp"
////////////////////////////////////////////////


#endif
