#include <algorithm>
#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

struct Airplane {
  std::string name, manufacturer;
  int capacity{};
  double range{};
  double max_speed{};
};

std::ostream &operator<<(std::ostream &os, const Airplane &a) {
  return os << a.manufacturer << ' ' << a.name
            << ", capacity=" << a.capacity
            << ", range=" << a.range
            << ", max_speed=" << a.max_speed;
}

// Компаратор сравнивает сами объекты T.
struct AirplaneCompare {
  bool operator()(const Airplane &a, const Airplane &b) const {
    return std::tie(a.manufacturer, a.name) <
           std::tie(b.manufacturer, b.name);
  }
};

template <class T, class Compare = std::less<T>>
class SearchTree {
protected:
  struct Node {
    T value;
    Node *left{}, *right{}, *parent{};
    Node(const T &v, Node *p = nullptr) : value(v), parent(p) {}
  };

  Node *root_{};
  Compare compare_{};

  bool equivalent(const T &a, const T &b) const {
    return !compare_(a, b) && !compare_(b, a);
  }

  static Node *minimum(Node *n) {
    while (n && n->left) n = n->left;
    return n;
  }
  static Node *maximum(Node *n) {
    while (n && n->right) n = n->right;
    return n;
  }
  static Node *successor(Node *n) {
    if (!n) return nullptr;
    if (n->right) return minimum(n->right);
    Node *p = n->parent;
    while (p && n == p->right) n = p, p = p->parent;
    return p;
  }
  static Node *predecessor(Node *n) {
    if (!n) return nullptr;
    if (n->left) return maximum(n->left);
    Node *p = n->parent;
    while (p && n == p->left) n = p, p = p->parent;
    return p;
  }
  void transplant(Node *u, Node *v) {
    if (!u->parent) root_ = v;
    else if (u == u->parent->left) u->parent->left = v;
    else u->parent->right = v;
    if (v) v->parent = u->parent;
  }
  void destroy(Node *n) {
    if (!n) return;
    destroy(n->left);
    destroy(n->right);
    delete n;
  }

public:
  class iterator {
    Node *node_{};
    bool reverse_{};
    friend class SearchTree;
    iterator(Node *n, bool reverse) : node_(n), reverse_(reverse) {}
  public:
    iterator() = default;
    T &operator*() const { return node_->value; }
    T *operator->() const { return &node_->value; }
    bool operator==(const iterator &o) const { return node_ == o.node_; }
    bool operator!=(const iterator &o) const { return !(*this == o); }
    iterator &operator++() {
      node_ = reverse_ ? predecessor(node_) : successor(node_);
      return *this;
    }
  };

  explicit SearchTree(Compare compare = Compare{}) : compare_(std::move(compare)) {}
  ~SearchTree() { destroy(root_); }
  SearchTree(const SearchTree &) = delete;
  SearchTree &operator=(const SearchTree &) = delete;

  iterator begin() { return {minimum(root_), false}; }
  iterator end() { return {nullptr, false}; }
  iterator rbegin() { return {maximum(root_), true}; }
  iterator rend() { return {nullptr, true}; }

  // Уникальная вставка.
  iterator add(const T &value) {
    if (!root_) return {root_ = new Node(value), false};
    Node *cur = root_, *parent = nullptr;
    while (cur) {
      parent = cur;
      if (equivalent(value, cur->value)) return {cur, false};
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    Node *n = new Node(value, parent);
    if (compare_(value, parent->value)) parent->left = n;
    else parent->right = n;
    return {n, false};
  }

  // Вставка с дубликатами: эквивалентные элементы идут вправо.
  iterator add_right(const T &value) {
    if (!root_) return {root_ = new Node(value), false};
    Node *cur = root_, *parent = nullptr;
    while (cur) {
      parent = cur;
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    Node *n = new Node(value, parent);
    if (compare_(value, parent->value)) parent->left = n;
    else parent->right = n;
    return {n, false};
  }

  iterator find(const T &value) {
    Node *cur = root_;
    while (cur) {
      if (equivalent(value, cur->value)) return {cur, false};
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    return end();
  }

  template <class Predicate>
  iterator find_value(const T &wanted, Predicate predicate) {
    for (auto it = begin(); it != end(); ++it)
      if (predicate(*it, wanted)) return it;
    return end();
  }

  template <class Predicate>
  std::vector<T> find_all(Predicate predicate) {
    std::vector<T> result;
    for (auto it = begin(); it != end(); ++it)
      if (predicate(*it)) result.push_back(*it);
    return result;
  }

  bool erase(iterator position) {
    Node *z = position.node_;
    if (!z) return false;
    if (!z->left) transplant(z, z->right);
    else if (!z->right) transplant(z, z->left);
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

  bool erase(const T &value) {
    auto it = find(value);
    return it != end() && erase(it);
  }
};

template <class T, class Compare = std::less<T>>
class AVLTree {
  struct Node {
    T value;
    Node *left{}, *right{}, *parent{};
    Node(const T &v, Node *p = nullptr) : value(v), parent(p) {}
  };

  Node *root_{};
  Compare compare_{};
  std::unordered_map<Node *, int> heights_;

  int height(Node *n) const { return n ? heights_.at(n) : 0; }
  void update(Node *n) {
    if (n) heights_[n] = 1 + std::max(height(n->left), height(n->right));
  }
  int balance(Node *n) const { return n ? height(n->left) - height(n->right) : 0; }
  static Node *minimum(Node *n) {
    while (n && n->left) n = n->left;
    return n;
  }
  static Node *maximum(Node *n) {
    while (n && n->right) n = n->right;
    return n;
  }
  static Node *successor(Node *n) {
    if (!n) return nullptr;
    if (n->right) return minimum(n->right);
    Node *p = n->parent;
    while (p && n == p->right) n = p, p = p->parent;
    return p;
  }
  static Node *predecessor(Node *n) {
    if (!n) return nullptr;
    if (n->left) return maximum(n->left);
    Node *p = n->parent;
    while (p && n == p->left) n = p, p = p->parent;
    return p;
  }
  Node *rotate_right(Node *y) {
    Node *x = y->left, *t = x->right, *p = y->parent;
    x->right = y; y->left = t; y->parent = x;
    if (t) t->parent = y;
    x->parent = p;
    if (!p) root_ = x;
    else if (p->left == y) p->left = x;
    else p->right = x;
    update(y); update(x);
    return x;
  }
  Node *rotate_left(Node *x) {
    Node *y = x->right, *t = y->left, *p = x->parent;
    y->left = x; x->right = t; x->parent = y;
    if (t) t->parent = x;
    y->parent = p;
    if (!p) root_ = y;
    else if (p->left == x) p->left = y;
    else p->right = y;
    update(x); update(y);
    return y;
  }
  Node *insert(Node *cur, const T &value, Node *parent) {
    if (!cur) {
      Node *n = new Node(value, parent);
      heights_[n] = 1;
      return n;
    }
    if (compare_(value, cur->value)) cur->left = insert(cur->left, value, cur);
    else cur->right = insert(cur->right, value, cur);
    update(cur);
    int b = balance(cur);
    if (b > 1) {
      if (compare_(value, cur->left->value)) return rotate_right(cur);
      cur->left = rotate_left(cur->left);
      return rotate_right(cur);
    }
    if (b < -1) {
      if (!compare_(value, cur->right->value)) return rotate_left(cur);
      cur->right = rotate_right(cur->right);
      return rotate_left(cur);
    }
    return cur;
  }
  void destroy(Node *n) {
    if (!n) return;
    destroy(n->left); destroy(n->right);
    heights_.erase(n); delete n;
  }

public:
  class iterator {
    Node *node_{}; bool reverse_{};
    friend class AVLTree;
    iterator(Node *n, bool r) : node_(n), reverse_(r) {}
  public:
    iterator() = default;
    T &operator*() const { return node_->value; }
    T *operator->() const { return &node_->value; }
    bool operator==(const iterator &o) const { return node_ == o.node_; }
    bool operator!=(const iterator &o) const { return !(*this == o); }
    iterator &operator++() {
      node_ = reverse_ ? predecessor(node_) : successor(node_);
      return *this;
    }
  };

  explicit AVLTree(Compare compare = Compare{}) : compare_(std::move(compare)) {}
  ~AVLTree() { destroy(root_); }
  iterator begin() { return {minimum(root_), false}; }
  iterator end() { return {nullptr, false}; }
  iterator rbegin() { return {maximum(root_), true}; }
  iterator rend() { return {nullptr, true}; }

  // AVL-вариант допускает одинаковые элементы и помещает их вправо.
  iterator add(const T &value) {
    root_ = insert(root_, value, nullptr);
    Node *cur = root_;
    while (cur) {
      if (!compare_(value, cur->value) && !compare_(cur->value, value))
        return {cur, false};
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    return end();
  }

  iterator find(const T &value) {
    Node *cur = root_;
    while (cur) {
      if (!compare_(value, cur->value) && !compare_(cur->value, value))
        return {cur, false};
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    return end();
  }

  template <class Predicate>
  std::vector<T> find_all(Predicate predicate) {
    std::vector<T> result;
    for (auto it = begin(); it != end(); ++it)
      if (predicate(*it)) result.push_back(*it);
    return result;
  }
};


template <class T, class Compare = std::less<T>>
class SplayTree {
  struct Node {
    T value;
    Node *left{}, *right{}, *parent{};
    Node(const T &v, Node *p = nullptr) : value(v), parent(p) {}
  };

  Node *root_{};
  Compare compare_{};

  bool equivalent(const T &a, const T &b) const {
    return !compare_(a, b) && !compare_(b, a);
  }
  static Node *minimum(Node *n) {
    while (n && n->left) n = n->left;
    return n;
  }
  static Node *maximum(Node *n) {
    while (n && n->right) n = n->right;
    return n;
  }
  static Node *successor(Node *n) {
    if (!n) return nullptr;
    if (n->right) return minimum(n->right);
    Node *p = n->parent;
    while (p && n == p->right) n = p, p = p->parent;
    return p;
  }
  static Node *predecessor(Node *n) {
    if (!n) return nullptr;
    if (n->left) return maximum(n->left);
    Node *p = n->parent;
    while (p && n == p->left) n = p, p = p->parent;
    return p;
  }
  void rotate_left(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left) y->left->parent = x;
    y->parent = x->parent;
    if (!x->parent) root_ = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->left = x;
    x->parent = y;
  }
  void rotate_right(Node *x) {
    Node *y = x->left;
    x->left = y->right;
    if (y->right) y->right->parent = x;
    y->parent = x->parent;
    if (!x->parent) root_ = y;
    else if (x == x->parent->left) x->parent->left = y;
    else x->parent->right = y;
    y->right = x;
    x->parent = y;
  }
  void splay(Node *x) {
    while (x->parent) {
      Node *p = x->parent;
      Node *g = p->parent;
      if (!g) {
        if (x == p->left) rotate_right(p);
        else rotate_left(p);
      } else if (x == p->left && p == g->left) {
        rotate_right(g); rotate_right(p);
      } else if (x == p->right && p == g->right) {
        rotate_left(g); rotate_left(p);
      } else if (x == p->right && p == g->left) {
        rotate_left(p); rotate_right(g);
      } else {
        rotate_right(p); rotate_left(g);
      }
    }
  }
  static void destroy(Node *n) {
    if (!n) return;
    destroy(n->left); destroy(n->right); delete n;
  }

public:
  class iterator {
    Node *node_{};
    bool reverse_{};
    friend class SplayTree;
    iterator(Node *n, bool r) : node_(n), reverse_(r) {}
  public:
    iterator() = default;
    T &operator*() const { return node_->value; }
    T *operator->() const { return &node_->value; }
    bool operator==(const iterator &o) const { return node_ == o.node_; }
    bool operator!=(const iterator &o) const { return !(*this == o); }
    iterator &operator++() {
      node_ = reverse_ ? predecessor(node_) : successor(node_);
      return *this;
    }
  };

  explicit SplayTree(Compare compare = Compare{}) : compare_(std::move(compare)) {}
  ~SplayTree() { destroy(root_); }

  iterator begin() { return {minimum(root_), false}; }
  iterator end() { return {nullptr, false}; }
  iterator rbegin() { return {maximum(root_), true}; }
  iterator rend() { return {nullptr, true}; }

  iterator add(const T &value) {
    if (!root_) return {root_ = new Node(value), false};
    Node *cur = root_, *parent = nullptr;
    while (cur) {
      parent = cur;
      if (equivalent(value, cur->value)) {
        splay(cur);
        return {root_, false};
      }
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    Node *n = new Node(value, parent);
    if (compare_(value, parent->value)) parent->left = n;
    else parent->right = n;
    splay(n);
    return {root_, false};
  }

  iterator find(const T &value) {
    Node *cur = root_;
    Node *last = nullptr;
    while (cur) {
      last = cur;
      if (equivalent(value, cur->value)) {
        splay(cur);
        return {root_, false};
      }
      cur = compare_(value, cur->value) ? cur->left : cur->right;
    }
    if (last) splay(last);
    return end();
  }

  template <class Predicate>
  std::vector<T> find_all(Predicate predicate) {
    std::vector<T> result;
    for (auto it = begin(); it != end(); ++it)
      if (predicate(*it)) result.push_back(*it);
    return result;
  }

  bool erase(const T &value) {
    auto it = find(value);
    if (it == end()) return false;
    Node *z = root_;
    Node *left = z->left;
    Node *right = z->right;
    if (left) left->parent = nullptr;
    if (right) right->parent = nullptr;
    delete z;
    if (!left) { root_ = right; return true; }
    root_ = left;
    Node *m = maximum(root_);
    splay(m);
    root_->right = right;
    if (right) right->parent = root_;
    return true;
  }
};

template <class Tree>
void print_tree(Tree &tree, bool ascending = true) {
  if (ascending)
    for (auto it = tree.begin(); it != tree.end(); ++it) std::cout << *it << '\n';
  else
    for (auto it = tree.rbegin(); it != tree.rend(); ++it) std::cout << *it << '\n';
}

int main() {
  using Tree = SearchTree<Airplane, AirplaneCompare>;
  using AVL = AVLTree<Airplane, AirplaneCompare>;

  Tree tree;
  tree.add({"A320", "Airbus", 180, 6100, 840});
  tree.add({"A321", "Airbus", 220, 5950, 840});
  tree.add({"737-800", "Boeing", 189, 5765, 842});

  std::cout << "BST:\n";
  print_tree(tree);

  auto found = tree.find_all([](const Airplane &a) {
    return a.manufacturer == "Airbus" && a.name == "A320";
  });
  std::cout << "Airbus A320: " << found.size() << "\n\n";

  AVL avl;
  avl.add({"A320", "Airbus", 180, 6100, 840});
  avl.add({"A320", "Airbus", 186, 6100, 840});
  avl.add({"A321", "Airbus", 220, 5950, 840});
  avl.add({"737-800", "Boeing", 189, 5765, 842});

  std::cout << "AVL:\n";
  print_tree(avl);

  auto same_key = avl.find_all([](const Airplane &a) {
    return a.manufacturer == "Airbus" && a.name == "A320";
  });
  std::cout << "Same key: " << same_key.size() << '\n';

  SplayTree<Airplane, AirplaneCompare> splay;
  splay.add({"A320", "Airbus", 180, 6100, 840});
  splay.add({"A321", "Airbus", 220, 5950, 840});
  splay.add({"737-800", "Boeing", 189, 5765, 842});
  std::cout << "Splay:\n";
  print_tree(splay);
}
