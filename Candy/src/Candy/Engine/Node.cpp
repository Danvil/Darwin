#include "Node.h"
#include <iostream>

namespace Candy
{

	Node::Node(void)
	{
		transformation_.setIdentity();
	}

	Node::Node(const Mat4f& t)
		: transformation_(t)
	{
	}

	Node::Node(Ptr(IDrawable) x)
		: visual_(x)
	{
		transformation_.setIdentity();
	}

	Node::~Node(void)
	{
	}

	void Node::Add(Ptr(INodeBase) a) {
		children_.push_back(a);
	}

	void Node::Remove(Ptr(INodeBase) a) {
		std::vector<Ptr(INodeBase)>::iterator it=std::find(children_.begin(), children_.end(), a);
		if(it != children_.end()) {
			children_.erase(it);
		}
	}

	void Node::Render()
	{
		IDrawable::sCamera->Push(transformation_);
		if(visual_) {
			visual_->Render();
		}
		for(std::vector<Ptr(INodeBase)>::iterator it=children_.begin(); it!=children_.end(); ++it) {
			(*it)->Render();
		}
		IDrawable::sCamera->Pop();
	}

}
