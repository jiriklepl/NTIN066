#include <limits>
#include <stack>
#include <vector>
#include <iostream>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)

void expect_failed(const string& message);

/*** One node ***/

class ab_node {
  public:
    // Keys stored in this node and the corresponding children
    // The vectors are large enough to accomodate one extra entry
    // in overflowing nodes.
    vector<ab_node *> children;
    vector<int> keys;

    // If this node contains the given key, return true and set i to key's position.
    // Otherwise return false and set i to the first key greater than the given one.
    bool find_branch(int key, std::size_t &i)
    {
        i = 0;
        while (i < keys.size() && keys[i] <= key) {
            if (keys[i] == key)
                return true;
            i++;
        }
        return false;
    }

    // Insert a new key at posision i and add a new child between keys i and i+1.
    void insert_branch(std::size_t i, int key, ab_node *child)
    {
        keys.insert(keys.begin() + i, key);
        children.insert(children.begin() + i + 1, child);
    }

    // An auxiliary function for displaying a sub-tree under this node.
    void show(int indent);
};

/*** Tree ***/

class ab_tree {
  public:
    int a;          // Minimum allowed number of children
    int b;          // Maximum allowed number of children
    ab_node *root;  // Root node (even a tree with no keys has a root)
    int num_nodes;  // We keep track of how many nodes the tree has

    // Create a new node and return a pointer to it.
    ab_node *new_node()
    {
        ab_node *n = new ab_node;
        n->keys.reserve(b);
        n->children.reserve(b+1);
        num_nodes++;
        return n;
    }

    // Delete a given node, assuming that its children have been already unlinked.
    void delete_node(ab_node *n)
    {
        num_nodes--;
        delete n;
    }

    // Constructor: initialize an empty tree with just the root.
    ab_tree(int a, int b)
    {
        EXPECT(a >= 2 && b >= 2*a - 1, "Invalid values of a,b");
        this->a = a;
        this->b = b;
        num_nodes = 0;
        // The root has no keys and one null child pointer.
        root = new_node();
        root->children.push_back(nullptr);
    }

    // An auxiliary function for deleting a subtree recursively.
    void delete_tree(ab_node *n)
    {
        for (int i=0; i < n->children.size(); i++)
            if (n->children[i])
                delete_tree(n->children[i]);
        delete_node(n);
    }

    // Destructor: delete all nodes.
    ~ab_tree()
    {
        delete_tree(root);
        EXPECT(num_nodes == 0, "Memory leak detected: some nodes were not deleted");
    }

    // Find a key: returns true if it is present in the tree.
    bool find(int key) const
    {
        ab_node *n = root;
        while (n) {
            std::size_t i;
            if (n->find_branch(key, i))
                return true;
            n = n->children[i];
        }
        return false;
    }

    // Display the tree on standard output in human-readable form.
    void show();

    // Check that the data structure satisfies all invariants.
    void audit();

    // Insert: add key to the tree (unless it was already present).
    void insert(int key)
    {
        ab_node *n = root;
        std::size_t i;

        // holds the path to the inserted key
        std::stack<std::pair<ab_node * const, std::size_t>> parents;

        do {
            if (n->find_branch(key, i))
                return;

            parents.emplace(n, i);
        } while (n->children[i] != nullptr && (n = n->children[i]));

        n->insert_branch(i, key, nullptr);

        while (n->keys.size() >= b) {
            // splitting n into n and m

            ab_node * const m = new_node();
            i = n->keys.size() / 2 + 1;
            key = n->keys[i - 1];

            std::move(n->keys.begin() + i, n->keys.end(),
                std::back_inserter(m->keys));
            std::move(n->children.begin() + i, n->children.end(),
                std::back_inserter(m->children));

            n->keys.erase(n->keys.begin() + (i - 1), n->keys.end());
            n->children.erase(n->children.begin() + i, n->children.end());

            if (n == root) {
                // we need a new layer

                root = new_node();
                root->children.emplace_back(n);
                root->children.emplace_back(m);
                root->keys.emplace_back(key);

                return;
            } else {
                // we add m as n's right sibling and propagate upwards

                parents.pop();
                const auto [parent, i] = parents.top();

                parent->insert_branch(i, key, m);
                n = parent;
            }
        }
    }
};
