#pragma once

#include "../Candy_Dll.h"
#include "IDrawable.h"
#include <Candy/Tools/Ptr.h>
#include <Candy/Tools/LinAlg.h>
#include <vector>

namespace Candy
{
	class INodeBase
	{
	public:
		virtual ~INodeBase() {}

		virtual void Render() = 0;

	};

	class CANDY_API Node
		: public INodeBase
	{
	public:
		Node();

		Node(const Mat4f& t);

		Node(Ptr(IDrawable) x);

		~Node();

		void SetTransformation(const Mat4f& t) {
			transformation_ = t;
		}

		void SetTranslation(const Vec3f& t) {
			transformation_(0,3) = t(0);
			transformation_(1,3) = t(1);
			transformation_(2,3) = t(2);
		}

		void SetPosition(const Vec3f& p) {
			transformation_(0,3) = p(0);
			transformation_(1,3) = p(1);
			transformation_(2,3) = p(2);
		}

		const Mat4f& GetTransformation() const {
			return transformation_;
		}

		void SetVisual(IDrawable* x) {
			visual_.reset(x);
		}

		void SetVisual(Ptr(IDrawable) x) {
			visual_ = x;
		}

		Ptr(IDrawable) GetVisual() const {
			return visual_;
		}

		void Add(Ptr(INodeBase) a);

		void Remove(Ptr(INodeBase) a);

		Ptr(Node) Add(IDrawable* x) {
			return Add(Ptr(IDrawable)(x));
		}

		Ptr(Node) Add(Ptr(IDrawable) x) {
			Ptr(Node) node(new Node(x));
			Add(node);
			return node;
		}

		virtual void Render();

	protected:
		Mat4f transformation_;
		Ptr(IDrawable) visual_;
		std::vector<Ptr(INodeBase)> children_;

	public:
		EIGEN_MAKE_ALIGNED_OPERATOR_NEW
	};

}
