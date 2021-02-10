#include <iostream>
#include "Stack.h"
using namespace std;

//Constructor
template <class itemType>
Stack<itemType>::Stack()
{
	top=nullptr;
}

template <class itemType>
StackNode<itemType>* Stack<itemType>::GetTopPointer(Stack<itemType> myStack)
{
	return myStack.top;
}

//Push back elements to the stack
template <class itemType>
void Stack<itemType>::push(TreeNode<itemType>* elmt)
{
	StackNode<itemType>*newNode;

	newNode = new StackNode<itemType>;
	newNode->value = elmt;

	if(isEmpty())
	{
		top = newNode;
		newNode->next = nullptr;
	}
	else
	{
		newNode->next = top;
		top = newNode;
	}
}
//Pop up elements from the stack
template <class itemType>
TreeNode<itemType>* Stack<itemType>::pop()
{
	StackNode<itemType>*temp;

	if(!isEmpty())
	{
		TreeNode<itemType>* elmt = top->value;
		temp = top->next;
		delete top;
		top = temp;
		return elmt;
	}
	return nullptr;
}

//If the stack is empty check function
template <class itemType>
bool Stack<itemType>::isEmpty()
{
	return (top==nullptr);
}
