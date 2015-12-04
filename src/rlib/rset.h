#pragma once

#include "ralgo.h"

template<typename T>
struct rset_i
{
	T key;
	rset_i<T>* father;
	rset_i<T>* left;
	rset_i<T>* right;
	int col;
};

template<typename T>
struct rset
{
	rset_i<T>* proot;
	rset_i<T>* pnil;//todo:pnil可用null代替?
	int num;

	enum
	{
		c_black,
		c_red,
	};

	~rset<T>()
	{
		clear_x(proot);
		r_delete<rset_i<T> >(pnil);
	}

	rset<T>()
	{
		pnil=r_new<rset_i<T> >(1);
		init();
	}

	rset<T>(const rset<T>& a)
	{
		pnil=r_new<rset_i<T> >(1);
		init();
		//用递归拷贝更快
		copy_x(a.pnil,a.proot);
	}

	void operator=(const rset<T>& a)
	{
		clear();
		copy_x(a.pnil,a.proot);
	}

	void copy_x(const rset_i<T>* xnil,const rset_i<T>* x)
	{
		if(x!=xnil)
		{
			copy_x(xnil,x->left);
			copy_x(xnil,x->right);
			insert(x->key);
		}
	}
	
	void copy_all(const rset<T>& a)
	{
		T* p;
		for_set(p,a)
		{
			insert(*p);
		}
	}

	void init()
	{
		pnil->father=pnil;
		pnil->left=pnil;
		pnil->right=pnil;
		pnil->col=c_black;

		proot=pnil;
		num=0;
	}

	void clear()
	{
		clear_x(proot);
		init();
	}

	void clear_x(rset_i<T>* x)
	{
		if(x!=pnil)
		{
			clear_x(x->left);
			clear_x(x->right);
			r_delete<rset_i<T> >(x);
		}
	}

	rbool empty() const
	{
		return num==0;
	}

	int count() const
	{
		return num;
	}

	void rotate_left(rset_i<T>* x)
	{
		rset_i<T>* y;
		if(x->right==pnil)
		{
			return;
		}
		y=x->right;
		x->right=y->left;
		if(y->left!=pnil)
		{
			y->left->father=x;
		}
		y->father=x->father;
		if(x->father==pnil)
		{
			proot=y;
		}
		elif(x->father->left==x)
		{
			x->father->left=y;
		}
		else
		{
			x->father->right=y;
		}
		y->left=x;
		x->father=y;
	}

	void rotate_right(rset_i<T>* x)
	{
		rset_i<T>* y;
		if(x->left==pnil)
		{
			return;
		}
		y=x->left;
		x->left=y->right;
		if(y->right!=pnil)
		{
			y->right->father=x;
		}
		y->father=x->father;
		if(x->father==pnil)
		{
			proot=y;
		}
		elif(x->father->left==x)
		{
			x->father->left=y;
		}
		else
		{
			x->father->right=y;
		}
		y->right=x;
		x->father=y;
	}

	//重复代码需简化
	void fixup_insert(rset_i<T>* z)
	{
		rset_i<T>* y;
		while(z->father->col==c_red) 
		{ 
			if(z->father==z->father->father->left) 
			{ 
				y=z->father->father->right; 
				if(y->col==c_red) 
				{ 
					z->father->col=c_black; 
					y->col=c_black; 
					z->father->father->col=c_red; 
					z=z->father->father; 
				} 
				else 
				{
					if(z==z->father->right) 
					{ 
						z=z->father; 
						rotate_left(z); 
					} 
					z->father->col=c_black; 
					z->father->father->col=c_red; 
					rotate_right(z->father->father); 
				}
			} 
			else 
			{ 
				y=z->father->father->left; 
				if(y->col==c_red) 
				{ 
					z->father->col=c_black; 
					y->col=c_black; 
					z->father->father->col=c_red; 
					z=z->father->father; 
				} 
				else 
				{
					if(z==z->father->left) 
					{ 
						z=z->father; 
						rotate_right(z); 
					} 
					z->father->col=c_black; 
					z->father->father->col=c_red; 
					rotate_left(z->father->father);
				}
			} 
		} 
		proot->col=c_black; 
	}

	void insert_p(rset_i<T>* z)
	{
		rset_i<T>* y=pnil;
		rset_i<T>* x=proot;
		while(x!=pnil)
		{
			y=x;
			if(z->key<x->key)
			{
				x=x->left;
			}
			else
			{
				x=x->right;
			}
		}
		z->father=y;
		if(y==pnil)
		{
			proot=z;
		}
		elif(z->key<y->key)
		{
			y->left=z;
		}
		else
		{
			y->right=z;
		}
		z->left=pnil;
		z->right=pnil;
		z->col=c_red;
		fixup_insert(z);
	}

	void insert(const T& a)
	{
		rset_i<T>* p=r_new<rset_i<T> >();
		p->key=a;
		insert_p(p);
		num++;
	}

	void insert_c(const T& a)
	{
		if(exist(a))
		{
			return;
		}
		insert(a);
	}

	void fixup_erase(rset_i<T>* x)
	{
		rset_i<T>* w;
		while(x!=proot&&x->col==c_black)
		{
			if(x==x->father->left)
			{
				w=x->father->right;
				if(w->col==c_red)
				{
					w->col=c_black;
					x->father->col=c_red;
					rotate_left(x->father);
					w=x->father->right;
				}
				if(w->left->col==c_black&&w->right->col==c_black)
				{
					w->col=c_red;
					x=x->father;
				}
				else 
				{
					if(w->right->col==c_black)
					{
						w->left->col=c_black;
						w->col=c_red;
						rotate_right(w);
						w=x->father->right;
					}
					w->col=x->father->col;
					x->father->col=c_black;
					w->right->col=c_black;
					rotate_left(x->father);
					x=proot;
				}
			}
			else
			{
				w=x->father->left;
				if(w->col==c_red)
				{
					w->col=c_black;
					x->father->col=c_red;
					rotate_right(x->father);
					w=x->father->left;
				}
				if(w->right->col==c_black&&w->left->col==c_black)
				{
					w->col=c_red;
					x=x->father;
				}
				else
				{
					if(w->left->col==c_black)
					{
						w->right->col=c_black;
						w->col=c_red;
						rotate_left(w);
						w=x->father->left;
					}
					w->col=x->father->col;
					x->father->col=c_black;
					w->left->col=c_black;
					rotate_right(x->father);
					x=proot;
				}
			}
		}
		x->col=c_black;
	}

	void transplant(rset_i<T>* u,rset_i<T>* v)
	{
		if(u->father==pnil)
		{
			proot=v;
		}
		elif(u==u->father->left)
		{
			u->father->left=v;
		}
		else
		{
			u->father->right=v;
		}
		v->father=u->father;
	}

	void erase_p(rset_i<T>* z)
	{
		rset_i<T>* y=z;
		rset_i<T>* x;
		int y_original_col=y->col;
		if(z->left==pnil)
		{
			x=z->right;
			transplant(z,z->right);
		}
		elif(z->right==pnil)
		{
			x=z->left;
			transplant(z,z->left);
		}
		else
		{
			y=minimum(z->right);
			y_original_col=y->col;
			x=y->right;
			if(y->father==z)
			{
				x->father=y;
			}
			else
			{
				transplant(y,y->right);
				y->right=z->right;
				y->right->father=y;
			}
			transplant(z,y);
			y->left=z->left;
			y->left->father=y;
			y->col=z->col;
		}
		if(y_original_col==c_black)
		{
			fixup_erase(x);
		}
	}

	void erase(const T& a)
	{
		rset_i<T>* x=search(proot,a);
		if(x!=pnil)
		{
			erase_p(x);
			r_delete<rset_i<T> >(x);
			num--;
		}
	}

	bool exist(const T& a) const
	{
		return search(proot,a)!=pnil;
	}

	rset_i<T>* successor(const rset_i<T>* x) const
	{
		rset_i<T>* y;
		if(x->right!=pnil)
		{
			return minimum(x->right);
		}
		y=x->father;
		while(y!=pnil&&x==y->right)
		{
			x=y;
			y=y->father;
		}
		return y;
	}

	rset_i<T>* predecessor(const rset_i<T>* x) const
	{
		rset_i<T>* y;
		if(x->left!=pnil)
		{
			return maximum(x->left);
		}
		y=x->father;
		while(y!=pnil&&x==y->left)
		{
			x=y;
			y=y->father;
		}
		return y;
	}

	rset_i<T>* minimum(const rset_i<T>* x) const
	{
		while(x->left!=pnil)
		{
			x=x->left;
		}
		return (rset_i<T>*)x;
	}

	rset_i<T>* maximum(const rset_i<T>* x) const
	{
		while(x->right!=pnil)
		{
			x=x->right;
		}
		return x;
	}

	rset_i<T>* search(const rset_i<T>* x,const T& a) const
	{
		while(x!=pnil&&r_not_equal<T>(a,x->key))
		{
			if(a<x->key)
			{
				x=x->left;
			}
			else
			{
				x=x->right;
			}
		}
		return (rset_i<T>*)x;
	}

	T* find(const T& a) const
	{
		rset_i<T>* x=search(proot,a);
		if(x==pnil)
		{
			return null;
		}
		return (T*)x;
	}

	T* next(const void* p) const
	{
		rset_i<T>* x=successor((rset_i<T>*)p);
		if(x==pnil)
		{
			return null;
		}
		return (T*)x;
	}
	
	T* prev(const void* p) const
	{
		rset_i<T>* x=predecessor((rset_i<T>*)p);
		if(x==pnil)
		{
			return null;
		}
		return (T*)x;
	}

	T* begin() const
	{
		rset_i<T>* x=minimum(proot);
		if(x==pnil)
		{
			return null;
		}
		return (T*)x;
	}

	static T* end()
	{
		return null;
	}

	T* index(int n) const
	{
		T* p=begin();
		for(;n;n--)
		{
			if(p==null)
			{
				return null;
			}
			p=next(p);
		}
		return p;
	}

	T& operator[](int n) const
	{
		return *index(n);
	}
};
