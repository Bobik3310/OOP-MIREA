#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

// Вариант 30, таблица 2.4: K = (производитель, название).
struct AirplaneKey {
  std::string manufacturer, name;
  bool operator<(const AirplaneKey &o) const {
    return std::tie(manufacturer, name) < std::tie(o.manufacturer, o.name);
  }
  bool operator==(const AirplaneKey &o) const {
    return manufacturer == o.manufacturer && name == o.name;
  }
};
std::ostream &operator<<(std::ostream &os, const AirplaneKey &k) {
  return os << k.manufacturer << " / " << k.name;
}

// Вариант 30, таблица 2.4: C = "Самолет".
struct Airplane {
  bool operator==(const Airplane &o) const {
    return name == o.name && manufacturer == o.manufacturer &&
           capacity == o.capacity && range == o.range &&
           max_speed == o.max_speed;
  }
  std::string name, manufacturer;
  int capacity{};
  double range{};     // km
  double max_speed{}; // km/h
  AirplaneKey key() const { return {manufacturer, name}; }
};
std::ostream &operator<<(std::ostream &os, const Airplane &a) {
  return os << a.manufacturer << ' ' << a.name << ", capacity=" << a.capacity
            << ", range=" << a.range << ", max_speed=" << a.max_speed;
}

// 2.4. Небалансированное бинарное дерево поиска с уникальными ключами.
template <class T, class Key, class KeyOf> class SearchTree {
protected:
  struct Node {
    Key key;
    T value;
    Node *left{}, *right{}, *parent{};
    Node(const Key &k, const T &v, Node *p = nullptr)
        : key(k), value(v), parent(p) {}
  };
  Node *root_{};
  KeyOf key_of_{};

  static Node *minimum(Node *n) {
    while (n && n->left)
      n = n->left;
    return n;
  }
  static Node *maximum(Node *n) {
    while (n && n->right)
      n = n->right;
    return n;
  }
  static Node *successor(Node *n) {
    if (!n)
      return nullptr;
    if (n->right)
      return minimum(n->right);
    Node *p = n->parent;
    while (p && n == p->right) {
      n = p;
      p = p->parent;
    }
    return p;
  }
  static Node *predecessor(Node *n) {
    if (!n)
      return nullptr;
    if (n->left)
      return maximum(n->left);
    Node *p = n->parent;
    while (p && n == p->left) {
      n = p;
      p = p->parent;
    }
    return p;
  }
  using KeyEqual = bool (*)(const Key &, const Key &);
  static bool default_key_equal(const Key &a, const Key &b) { return a == b; }
  Node *find_node(const Key &k, KeyEqual equal = default_key_equal) const {
    Node *n = root_;
    while (n) {
      if (equal(k, n->key))
        return n;
      n = (k < n->key) ? n->left : n->right;
    }
    return nullptr;
  }
  Node *insert_unique_node(const T &value) {
    Key k = key_of_(value);
    if (!root_)
      return root_ = new Node(k, value);
    Node *cur = root_;
    Node *parent = nullptr;
    while (cur) {
      parent = cur;
      if (k == cur->key)
        return cur;
      cur = (k < cur->key) ? cur->left : cur->right;
    }
    Node *n = new Node(k, value, parent);
    if (k < parent->key)
      parent->left = n;
    else
      parent->right = n;
    return n;
  }
  Node *
  insert_right_node(const T &value) { // equal keys go right (2.5, even variant)
    Key k = key_of_(value);
    if (!root_)
      return root_ = new Node(k, value);
    Node *cur = root_;
    Node *parent = nullptr;
    while (cur) {
      parent = cur;
      cur = (k < cur->key) ? cur->left : cur->right;
    }
    Node *n = new Node(k, value, parent);
    if (k < parent->key)
      parent->left = n;
    else
      parent->right = n;
    return n;
  }
  void transplant(Node *u, Node *v) {
    if (!u->parent)
      root_ = v;
    else if (u == u->parent->left)
      u->parent->left = v;
    else
      u->parent->right = v;
    if (v)
      v->parent = u->parent;
  }
  void destroy(Node *n) {
    if (!n)
      return;
    destroy(n->left);
    destroy(n->right);
    delete n;
  }

public:
  class iterator {
    Node *n_{};
    bool reverse_{};
    friend class SearchTree;

  public:
    iterator(Node *n, bool r) : n_(n), reverse_(r) {}
    iterator() = default;
    T &operator*() const { return n_->value; }
    T *operator->() const { return &n_->value; }
    bool operator==(const iterator &o) const { return n_ == o.n_; }
    bool operator!=(const iterator &o) const { return !(*this == o); }
    iterator &operator++() {
      n_ = reverse_ ? predecessor(n_) : successor(n_);
      return *this;
    }
    Node *node() const { return n_; }
  };
  SearchTree() = default;
  ~SearchTree() { destroy(root_); }
  iterator begin() { return iterator(minimum(root_), false); }
  iterator end() { return iterator(nullptr, false); }
  iterator rbegin() { return iterator(maximum(root_), true); }
  iterator rend() { return iterator(nullptr, true); }
  iterator min() { return begin(); }
  iterator max() { return rbegin(); }
  iterator add(const T &v) { return iterator(insert_unique_node(v), false); }
  iterator find(const Key &k, KeyEqual equal = default_key_equal) {
    return iterator(find_node(k, equal), false);
  }
  using Compare = bool (*)(const T &, const T &);
  iterator find_value(const T &wanted, Compare pred) {
    for (auto it = begin(); it != end(); ++it) {
      if (pred && pred(*it, wanted))
        return it;
    }
    return end();
  }
  bool erase(const Key &k) {
    Node *z = find_node(k);
    if (!z)
      return false;
    if (!z->left)
      transplant(z, z->right);
    else if (!z->right)
      transplant(z, z->left);
    else {
      Node *y = minimum(z->right);
      if (y->parent != z) {
        transplant(y, y->right);
        y->right = z->right;
        y->right->parent = y;
      }
      transplant(z, y);
      y->left = z->left;
      y->left->parent = y;
    }
    delete z;
    return true;
  }
  bool erase(iterator pos) {
    return pos.node() ? erase(pos.node()->key) : false;
  }
};

// AVL-наследник дерева 2.4. Вариант 30: равные ключи идут вправо.
template <class T, class Key, class KeyOf>
class AVLTree : public SearchTree<T, Key, KeyOf> {
protected:
  using Base = SearchTree<T, Key, KeyOf>;
  using Node = typename Base::Node;
  std::unordered_map<Node *, int> heights;

  int h(Node *n) const { return n ? heights.at(n) : 0; }
  void update(Node *n) {
    if (n)
      heights[n] = 1 + std::max(h(n->left), h(n->right));
  }
  int balance(Node *n) const { return n ? h(n->left) - h(n->right) : 0; }
  int rebuild_heights(Node *n) {
    if (!n)
      return 0;
    heights[n] =
        1 + std::max(rebuild_heights(n->left), rebuild_heights(n->right));
    return heights[n];
  }
  Node *rotateRight(Node *y) {
    Node *x = y->left;
    Node *t = x->right;
    Node *p = y->parent;
    x->right = y;
    y->left = t;
    y->parent = x;
    if (t)
      t->parent = y;
    x->parent = p;
    if (!p)
      this->root_ = x;
    else if (p->left == y)
      p->left = x;
    else
      p->right = x;
    update(y);
    update(x);
    return x;
  }
  Node *rotateLeft(Node *x) {
    Node *y = x->right;
    Node *t = y->left;
    Node *p = x->parent;
    y->left = x;
    x->right = t;
    x->parent = y;
    if (t)
      t->parent = x;
    y->parent = p;
    if (!p)
      this->root_ = y;
    else if (p->left == x)
      p->left = y;
    else
      p->right = y;
    update(x);
    update(y);
    return y;
  }
  Node *insert(Node *cur, const T &value, Node *parent) {
    Key k = this->key_of_(value);
    if (!cur) {
      Node *n = new Node(k, value, parent);
      heights[n] = 1;
      return n;
    }
    if (k < cur->key)
      cur->left = insert(cur->left, value, cur);
    else
      cur->right = insert(cur->right, value, cur);
    update(cur);
    int b = balance(cur);
    if (b > 1) {
      if (k < cur->left->key)
        return rotateRight(cur);
      cur->left = rotateLeft(cur->left);
      return rotateRight(cur);
    }
    if (b < -1) {
      if (!(k < cur->right->key))
        return rotateLeft(cur);
      cur->right = rotateRight(cur->right);
      return rotateLeft(cur);
    }
    return cur;
  }

public:
  using typename Base::iterator;
  iterator root_iterator() { return iterator(this->root_, false); }
  iterator add(const T &value) {
    this->root_ = insert(this->root_, value, nullptr);
    return iterator(this->find_node(this->key_of_(value)), false);
  }
  std::vector<T> operator[](const Key &key) const {
    std::vector<T> result;
    auto &self = const_cast<AVLTree &>(*this);
    for (auto it = self.begin(); it != self.end(); ++it)
      if (self.key_of_(*it) == key)
        result.push_back(*it);
    return result;
  }
};

// 2.6. Splay-дерево, унаследованное от AVLTree (2.5).
template <class T, class Key, class KeyOf>
class SplayTree : public AVLTree<T, Key, KeyOf> {
  using Base = AVLTree<T, Key, KeyOf>;
  using Node = typename Base::Node;

  void rotate_left(Node *x) {
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
  void rotate_right(Node *x) {
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
      Node *p = x->parent;
      Node *g = p->parent;
      if (!g) {
        if (x == p->left)
          rotate_right(p);
        else
          rotate_left(p);
      } else if (x == p->left && p == g->left) {
        rotate_right(g);
        rotate_right(p);
      } else if (x == p->right && p == g->right) {
        rotate_left(g);
        rotate_left(p);
      } else if (x == p->right) {
        rotate_left(p);
        rotate_right(g);
      } else {
        rotate_right(p);
        rotate_left(g);
      }
    }
  }

public:
  using typename Base::iterator;
  iterator add(const T &value) {
    iterator it = Base::add(value);
    if (it != this->end()) {
      splay(it.node());
      this->rebuild_heights(this->root_);
    }
    return it;
  }
  std::vector<T> operator[](const Key &key) {
    Node *n = this->find_node(key);
    if (n) {
      splay(n);
      this->rebuild_heights(this->root_);
    }
    return Base::operator[](key);
  }
  using EqualPred = bool (*)(const T &, const T &);
  iterator find(const T &wanted, EqualPred equal = nullptr) {
    for (auto it = this->begin(); it != this->end(); ++it) {
      bool same = equal ? equal(*it, wanted) : (*it == wanted);
      if (same) {
        splay(it.node());
        this->rebuild_heights(this->root_);
        return it;
      }
    }
    return this->end();
  }
};

struct AirplaneKeyOf {
  AirplaneKey operator()(const Airplane &a) const { return a.key(); }
};
bool same_airplane(const Airplane &a, const Airplane &b) { return a == b; }
int main() {
  SplayTree<Airplane, AirplaneKey, AirplaneKeyOf> tree;
  tree.add({"A320", "Airbus", 180, 6100, 840});
  tree.add({"737-800", "Boeing", 189, 5765, 842});
  tree.add({"A321", "Airbus", 220, 5950, 840});
  tree.add({"A320", "Airbus", 186, 6100, 840});
  auto values = tree[AirplaneKey{"Airbus", "A320"}];
  std::cout << "Objects with key Airbus/A320: " << values.size() << '\n';
  Airplane wanted{"A321", "Airbus", 220, 5950, 840};
  auto found = tree.find(wanted, same_airplane);
  if (found != tree.end())
    std::cout << "Found and splayed: " << *found << '\n';
  std::cout << "Root: " << *tree.root_iterator() << '\n';
}
