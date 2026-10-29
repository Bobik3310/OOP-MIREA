#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

template <class T, class Compare> class AVLTree;

// 2.4. Универсальное несбалансированное дерево поиска.
template <class T, class Compare>
class SearchTree {
protected:
  struct Node {
    T value;
    Node *left{}, *right{}, *parent{};
    Node(const T &v, Node *p = nullptr) : value(v), parent(p) {}
  };
  Node *root_{};
  Compare compare_{};
  Node *insert_right_node(const T &v) {
    if (!root_)
      return root_ = new Node(v);
    Node *cur = root_;
    Node *parent = nullptr;
    while (cur) {
      parent = cur;
      cur = compare_(v, cur->value) ? cur->left : cur->right;
    }
    Node *n = new Node(v, parent);
    if (compare_(v, parent->value))
      parent->left = n;
    else
      parent->right = n;
    return n;
  }
  static int height(Node *n) {
    return n ? 1 + std::max(height(n->left), height(n->right)) : 0;
  }
  static Node *next(Node *n) {
    if (!n)
      return nullptr;
    if (n->right) {
      n = n->right;
      while (n->left)
        n = n->left;
      return n;
    }
    Node *p = n->parent;
    while (p && n == p->right) {
      n = p;
      p = p->parent;
    }
    return p;
  }
  void destroy(Node *n) {
    if (!n)
      return;
    destroy(n->left);
    destroy(n->right);
    delete n;
  }
  void print_inorder(Node *n, std::ostream &os) const {
    if (!n)
      return;
    print_inorder(n->left, os);
    os << n->value << '\n';
    print_inorder(n->right, os);
  }

public:
  class iterator {
    Node *n_{};
    friend class SearchTree;
    friend class AVLTree<T, Compare>;

  public:
    iterator(Node *n = nullptr) : n_(n) {}
    T &operator*() const { return n_->value; }
    Node *node() const { return n_; }
    iterator &operator++() {
      n_ = next(n_);
      return *this;
    }
    bool operator==(const iterator &o) const { return n_ == o.n_; }
    bool operator!=(const iterator &o) const { return !(*this == o); }
  };
  ~SearchTree() { destroy(root_); }
  iterator begin() {
    Node *n = root_;
    while (n && n->left)
      n = n->left;
    return iterator(n);
  }
  iterator end() { return iterator(nullptr); }
  void print(std::ostream &os) const { print_inorder(root_, os); }
};

// 2.5. AVL-дерево наследуется от несбалансированного дерева 2.4.
template <class T, class Compare>
class AVLTree : public SearchTree<T, Compare> {
protected:
  using Base = SearchTree<T, Compare>;
  using Node = typename Base::Node;
  std::unordered_map<Node *, int> h_;
  int h(Node *n) const { return n ? h_.at(n) : 0; }
  void upd(Node *n) {
    if (n)
      h_[n] = 1 + std::max(h(n->left), h(n->right));
  }
  void left(Node *x) {
    Node *y = x->right, *p = x->parent;
    x->right = y->left;
    if (y->left)
      y->left->parent = x;
    y->left = x;
    x->parent = y;
    y->parent = p;
    if (!p)
      this->root_ = y;
    else if (p->left == x)
      p->left = y;
    else
      p->right = y;
    upd(x);
    upd(y);
  }
  void right(Node *y) {
    Node *x = y->left, *p = y->parent;
    y->left = x->right;
    if (x->right)
      x->right->parent = y;
    x->right = y;
    y->parent = x;
    x->parent = p;
    if (!p)
      this->root_ = x;
    else if (p->left == y)
      p->left = x;
    else
      p->right = x;
    upd(y);
    upd(x);
  }
  void rebalance(Node *n) {
    while (n) {
      upd(n);
      int b = h(n->left) - h(n->right);
      if (b > 1) {
        if (h(n->left->left) < h(n->left->right))
          left(n->left);
        right(n);
      } else if (b < -1) {
        if (h(n->right->right) < h(n->right->left))
          right(n->right);
        left(n);
      }
      n = n->parent;
    }
  }

public:
  using typename Base::iterator;
  iterator add(const T &v) {
    Node *n = this->insert_right_node(v);
    h_[n] = 1;
    rebalance(n->parent);
    return iterator(n);
  }
  std::vector<T> operator[](const T &key) const {
    std::vector<T> r;
    collect(this->root_, key, r);
    return r;
  }

private:
  void collect(Node *n, const T &k, std::vector<T> &r) const {
    if (!n)
      return;
    collect(n->left, k, r);
    if (!this->compare_(n->value, k) && !this->compare_(k, n->value))
      r.push_back(n->value);
    collect(n->right, k, r);
  }
};

template <class T, class Compare> 
class SplayTree : public AVLTree<T, Compare> {
  using Base = AVLTree<T, Compare>;
  using Node = typename Base::Node;
  void left(Node *x) {
    Node *y = x->right;
    if (!y)
      return;
    x->right = y->left;
    if (y->left)
      y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent)
      this->root_ = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
    y->left = x;
    x->parent = y;
  }
  void right(Node *x) {
    Node *y = x->left;
    if (!y)
      return;
    x->left = y->right;
    if (y->right)
      y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent)
      this->root_ = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;
    y->right = x;
    x->parent = y;
  }
  void splay(Node *x) {
    while (x && x->parent) {
      Node *p = x->parent, *g = p->parent;
      if (!g) {
        if (x == p->left)
          right(p);
        else
          left(p);
      } else if (x == p->left && p == g->left) {
        right(g);
        right(p);
      } else if (x == p->right && p == g->right) {
        left(g);
        left(p);
      } else if (x == p->right) {
        left(p);
        right(g);
      } else {
        right(p);
        left(g);
      }
    }
  }

public:
  using typename Base::iterator;
  iterator add(const T &v) {
    iterator it = Base::add(v);
    if (it != this->end())
      splay(it.node());
    return it;
  }
  using EqualFunction = bool (*)(const T &, const T &);
  iterator find(const T &wanted, EqualFunction equal = nullptr) {
    for (auto it = this->begin(); it != this->end(); ++it) {
      bool matches = equal ? equal(*it, wanted) : (*it == wanted);
      if (matches) {
        splay(it.node());
        return it;
      }
    }
    return this->end();
  }
  std::vector<T> operator[](const T &key) {
    std::vector<T> r;
    for (auto it = this->begin(); it != this->end(); ++it)
      if (!this->compare_(*it, key) && !this->compare_(key, *it)) {
        splay(it.node());
        r.push_back(*it);
        break;
      }
    for (auto it = this->begin(); it != this->end(); ++it)
      if (!this->compare_(*it, key) && !this->compare_(key, *it))
        r.push_back(*it);
    return r;
  }
};

struct Airplane {
  std::string name, manufacturer;
  int capacity{};
  double range{}, max_speed{};
};
std::ostream &operator<<(std::ostream &os, const Airplane &a) {
  return os << a.manufacturer << ' ' << a.name << " capacity=" << a.capacity;
}
struct AirplaneCompare {
  bool operator()(const Airplane &a, const Airplane &b) const {
    return a.manufacturer != b.manufacturer ? a.manufacturer < b.manufacturer
                                            : a.name < b.name;
  }
};
int main() {
  SplayTree<Airplane, AirplaneCompare> t;
  t.add({"A320", "Airbus", 180, 6100, 840});
  t.add({"A320", "Airbus", 186, 6100, 840});
  auto v = t[Airplane{"A320", "Airbus"}];
  std::cout << v.size() << '\n';
}
