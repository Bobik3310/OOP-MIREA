#include <functional>
#include <iostream>
#include <string>
#include <tuple>
#include <utility>

struct Airplane {
  std::string name;
  std::string manufacturer;
  int capacity{};
  double range{};
  double max_speed{};

  bool operator==(const Airplane &other) const {
    return name == other.name &&
           manufacturer == other.manufacturer &&
           capacity == other.capacity &&
           range == other.range &&
           max_speed == other.max_speed;
  }
};

std::ostream &operator<<(std::ostream &os, const Airplane &a) {
  return os << a.manufacturer << ' ' << a.name
            << ", capacity=" << a.capacity
            << ", range=" << a.range
            << ", max_speed=" << a.max_speed;
}

// Порядок элементов дерева задаётся снаружи.
// Само дерево ничего не знает о полях Airplane.
struct AirplaneCompare {
  bool operator()(const Airplane &a, const Airplane &b) const {
    return std::tie(a.manufacturer, a.name) < std::tie(b.manufacturer, b.name);
  }
};

bool same_capacity(const Airplane &a, const Airplane &b) {
  return a.capacity == b.capacity;
}

template <class T, class Compare = std::less<T>>
class SearchTree {
  struct Node {
    T value;
    Node *left{};
    Node *right{};
    Node *parent{};

    explicit Node(const T &v, Node *p = nullptr)
        : value(v), parent(p) {}
  };

  Node *root_{};
  Compare compare_{};

  bool equivalent(const T &a, const T &b) const {
    return !compare_(a, b) && !compare_(b, a);
  }

  static Node *minimum(Node *node) {
    while (node && node->left)
      node = node->left;
    return node;
  }

  static Node *maximum(Node *node) {
    while (node && node->right)
      node = node->right;
    return node;
  }

  static Node *successor(Node *node) {
    if (!node)
      return nullptr;

    if (node->right)
      return minimum(node->right);

    Node *parent = node->parent;
    while (parent && node == parent->right) {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  static Node *predecessor(Node *node) {
    if (!node)
      return nullptr;

    if (node->left)
      return maximum(node->left);

    Node *parent = node->parent;
    while (parent && node == parent->left) {
      node = parent;
      parent = parent->parent;
    }
    return parent;
  }

  void transplant(Node *from, Node *to) {
    if (!from->parent) {
      root_ = to;
    } else if (from == from->parent->left) {
      from->parent->left = to;
    } else {
      from->parent->right = to;
    }

    if (to)
      to->parent = from->parent;
  }

  void erase_node(Node *node) {
    if (!node->left) {
      transplant(node, node->right);
    } else if (!node->right) {
      transplant(node, node->left);
    } else {
      Node *replacement = minimum(node->right);

      if (replacement->parent != node) {
        transplant(replacement, replacement->right);
        replacement->right = node->right;
        replacement->right->parent = replacement;
      }

      transplant(node, replacement);
      replacement->left = node->left;
      replacement->left->parent = replacement;
    }

    delete node;
  }

  static void destroy(Node *node) {
    if (!node)
      return;

    destroy(node->left);
    destroy(node->right);
    delete node;
  }

public:
  class iterator {
    Node *node_{};
    bool reverse_{};

    friend class SearchTree;

    iterator(Node *node, bool reverse)
        : node_(node), reverse_(reverse) {}

  public:
    iterator() = default;

    T &operator*() const {
      return node_->value;
    }

    T *operator->() const {
      return &node_->value;
    }

    bool operator==(const iterator &other) const {
      return node_ == other.node_;
    }

    bool operator!=(const iterator &other) const {
      return !(*this == other);
    }

    iterator &operator++() {
      node_ = reverse_
                  ? predecessor(node_)
                  : successor(node_);
      return *this;
    }
  };

  explicit SearchTree(Compare compare = Compare{})
      : compare_(std::move(compare)) {}

  ~SearchTree() {
    destroy(root_);
  }

  SearchTree(const SearchTree &) = delete;
  SearchTree &operator=(const SearchTree &) = delete;

  iterator begin() {
    return {minimum(root_), false};
  }

  iterator end() {
    return {nullptr, false};
  }

  iterator rbegin() {
    return {maximum(root_), true};
  }

  iterator rend() {
    return {nullptr, true};
  }

  // Уникальная вставка.
  iterator add(const T &value) {
    if (!root_)
      return {root_ = new Node(value), false};

    Node *current = root_;
    Node *parent = nullptr;

    while (current) {
      parent = current;

      if (equivalent(value, current->value))
        return {current, false};

      current = compare_(value, current->value)
                    ? current->left
                    : current->right;
    }

    Node *node = new Node(value, parent);

    if (compare_(value, parent->value))
      parent->left = node;
    else
      parent->right = node;

    return {node, false};
  }

  // Вставка с дубликатами: эквивалентные элементы идут вправо.
  iterator add_right(const T &value) {
    if (!root_)
      return {root_ = new Node(value), false};

    Node *current = root_;
    Node *parent = nullptr;

    while (current) {
      parent = current;
      current = compare_(value, current->value)
                    ? current->left
                    : current->right;
    }

    Node *node = new Node(value, parent);

    if (compare_(value, parent->value))
      parent->left = node;
    else
      parent->right = node;

    return {node, false};
  }

  iterator find(const T &value) {
    Node *current = root_;

    while (current) {
      if (equivalent(value, current->value))
        return {current, false};

      current = compare_(value, current->value)
                    ? current->left
                    : current->right;
    }

    return end();
  }

  template <class Predicate>
  iterator find_value(const T &wanted, Predicate predicate) {
    for (auto it = begin(); it != end(); ++it) {
      if (predicate(*it, wanted))
        return it;
    }

    return end();
  }

  bool erase(const T &value) {
    auto position = find(value);

    if (position == end())
      return false;

    erase_node(position.node_);
    return true;
  }

  bool erase(iterator position) {
    if (!position.node_)
      return false;

    erase_node(position.node_);
    return true;
  }
};

template <class Tree>
void print_tree(Tree &tree, std::ostream &os,
                bool ascending = true) {
  if (ascending) {
    for (auto it = tree.begin(); it != tree.end(); ++it)
      os << *it << '\n';
  } else {
    for (auto it = tree.rbegin(); it != tree.rend(); ++it)
      os << *it << '\n';
  }
}

int main() {
  using Tree = SearchTree<Airplane, AirplaneCompare>;
  Tree tree;

  tree.add({"A320", "Airbus", 180, 6100, 840});
  tree.add({"737-800", "Boeing", 189, 5765, 842});
  tree.add({"A321", "Airbus", 220, 5950, 840});
  tree.add({"A350", "Airbus", 325, 15000, 905});

  std::cout << "\nПо возрастанию:\n";
  print_tree(tree, std::cout, true);

  std::cout << "\nПо убыванию:\n";
  print_tree(tree, std::cout, false);

  // Для поиска по manufacturer/name создаётся обычный T.
  Airplane sample{"A321", "Airbus", 0, 0, 0};
  auto found = tree.find(sample);

  if (found != tree.end())
    std::cout << "\nНайден: " << *found << '\n';

  Airplane capacity_sample{"", "", 220, 0, 0};
  auto by_value = tree.find_value(capacity_sample, same_capacity);

  if (by_value != tree.end())
    std::cout << "Найден по функции сравнения: "
              << *by_value << '\n';

  tree.erase(sample);

  std::cout << "\nПосле удаления Airbus/A321:\n";
  print_tree(tree, std::cout, true);
}
