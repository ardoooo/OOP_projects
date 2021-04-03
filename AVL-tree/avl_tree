#include <iostream>
#include <algorithm>

using namespace std;

class AVLtree {
public:
	struct node {
		int key;
		node* left;
		node* right;
		int height;
		node(int key) {
			this->key = key;
			this->left = nullptr;
			this->right = nullptr;
			this->height = 0;
		}
	};
	node* root = nullptr;

	int Height(node* v) {
		if (v == 0) return 0;
		return v->height;
	}
	int Bfactor(node* v) {
		return Height(v->right) - Height(v->left);
	}
	void correct(node* v) {
		v->height = 1 + max(Height(v->left), Height(v->right));
	}

	void Add(int key) {
		this->root = AddNode(this->root, key);
	}
	void Delete(int key) {
		this->root = DeleteNode(this->root, key);
	}

	node* getMin(node* v) {
		if (v->left == 0) return v;
		return getMin(v->left);
	}
	node* DeleteMin(node* v) {
		if (v->left == 0) return v->right;
		v->left = DeleteMin(v->left);
		correct(v);
		return v;
	}

	node* rotateRight(node* v) {
		node* temp = v->left;
		v->left = temp->right;
		temp->right = v;
		correct(v);
		correct(temp);
		return temp;
	}
	node* rotateLeft(node* v) {
		node* temp = v->right;
		v->right = temp->left;
		temp->left = v;
		correct(v);
		correct(temp);
		return temp;
	}
	node* balance(node* v) {
		correct(v);
		if (Bfactor(v) == 2) {
			if (Bfactor(v->right) < 0) {
				v->right = rotateRight(v->right);
			}
			return rotateLeft(v);
		}
		if (Bfactor(v) == -2) {
			if (Bfactor(v->left) > 0) {
				v->left = rotateLeft(v->left);
			}
			return rotateRight(v);
		}
		return v;
	}
	node* AddNode(node* v, int key) {
		if (v == 0) return new node(key);
		if (v->key > key) {
			v->left = AddNode(v->left, key);
		}
		if (v->key < key) {
			v->right = AddNode(v->right, key);
		}
		correct(v);

		return balance(v);
	}

	node* DeleteNode(node* v, int key) {
		if (v == 0) return 0;
		if (v->key > key) {
			v->left = DeleteNode(v->left, key);
		}
		if (v->key < key) {
			v->right = DeleteNode(v->right, key);
		}
		if (v->key == key) {
			if (v->right == 0) return v->left;
			if (v->left == 0) return v->right;
			node* t = v;
			v = getMin(t->right);
			v->right = DeleteMin(t->right);
			v->left = t->left;
		}

		return balance(v);
	}

	void Exists(node* v, int key) {
		if (v == 0) {
			cout << "false" << '\n';
			return;
		}
		if (v->key > key) {
			Exists(v->left, key);
		}
		if (v->key < key) {
			Exists(v->right, key);
		}
		if (v->key == key) {
			cout << "true"<< '\n';
			return;
		}
	}


	void Next(node* v, int key, long long a) {
		if (v == 0) {
			if (a != 10e9) cout << a << "\n";
			else cout << "none" << '\n';
			return;
		}
		if (v->key <= key) {
			if (v->key < a && v->key > key) Next(v->right, key, v->key);
			else Next(v->right, key, a);
		}
		if (v->key > key) {
			if (v->key < a && v->key > key) Next(v->left, key, v->key);
			else Next(v->left, key, a);
		}
	}

	void Prev(node* v, int key, long long a) {
		if (v == 0) {
			if (a != -10e9) cout << a << "\n";
			else cout << "none" << '\n';
			return;
		}
		if (v->key < key) {
			if (v->key > a && v->key < key) Prev(v->right, key, v->key);
			else Prev(v->right, key, a);
		}
		if (v->key >= key) {
			if (v->key > a && v->key < key) Prev(v->left, key, v->key);
			else Prev(v->left, key, a);
		}
	}
};
