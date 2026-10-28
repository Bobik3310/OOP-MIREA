#include <algorithm>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <list>
#include <optional>
#include <string>
#include <utility>
#include <vector>

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

// Связывает объект C с его ключом K.
template <class T, class Key, class KeyOf>
void print_tree(SearchTree<T, Key, KeyOf> &tree, std::ostream &os,
                bool ascending = true) {
  if (ascending) {
    for (auto it = tree.begin(); it != tree.end(); ++it)
      os << *it << '\n';
  } else {
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
      os << *it << '\n';
  }
}

struct AirplaneKeyOf {
  AirplaneKey operator()(const Airplane &a) const { return a.key(); }
};
bool same_capacity(const Airplane &a, const Airplane &b) {
  return a.capacity == b.capacity;
}
bool same_key(const AirplaneKey &a, const AirplaneKey &b) {
  return a.manufacturer == b.manufacturer && a.name == b.name;
}

int main() {
  // T = C (Самолет), Key = K, KeyOf извлекает K из объекта C.
  using Tree = SearchTree<Airplane, AirplaneKey, AirplaneKeyOf>;
  Tree tree;
  tree.add({"A320", "Airbus", 180, 6100, 840});
  tree.add({"737-800", "Boeing", 189, 5765, 842});
  tree.add({"A321", "Airbus", 220, 5950, 840});
  tree.add({"A350", "Airbus", 325, 15000, 905});

  std::cout << "\nПо возрастанию ключа:\n";
  print_tree(tree, std::cout, true);

  std::cout << "\nПо убыванию ключа:\n";
  print_tree(tree, std::cout, false);

  AirplaneKey key{"Airbus", "A321"};
  auto found = tree.find(key, same_key);
  if (found != tree.end())
    std::cout << "\nНайден по ключу: " << *found << '\n';

  Airplane sample{"", "", 220, 0, 0};
  auto by_value = tree.find_value(sample, same_capacity);
  if (by_value != tree.end())
    std::cout << "Найден по функции сравнения: " << *by_value << '\n';

  tree.erase(key);
  std::cout << "\nПосле удаления Airbus/A321:\n";
  print_tree(tree, std::cout, true);
}
