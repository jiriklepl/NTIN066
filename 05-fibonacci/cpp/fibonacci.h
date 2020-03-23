#include <vector>

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)
void expect_failed(const std::string& message);

/*
 * payload_t: Type used to allow storing user data in each heap node.
 *
 * priority_t: Type used to represent priority of the node. It must
 *             support comparison.
 *
 * In proper C++, the heap should be a template parametrized by these
 * types, but we decided to keep the code as simple as possible.
 */
typedef int payload_t;
typedef int priority_t;

struct FibonacciHeap;

// Node of FibonacciHeap
struct Node {
    payload_t payload;
    priority_t priority() { return prio; }

  private:
    priority_t prio;

    Node *parent = nullptr, *first_child = nullptr;

    // Pointers in a cyclic list of children of the parent.
    Node *prev_sibling, *next_sibling;

    int rank = 0;
    bool marked = false;

    Node(priority_t p, payload_t payload) : payload(payload), prio(p) {
        next_sibling = prev_sibling = this;
    }

    ~Node() {}

    friend class FibonacciHeap;
    friend class HeapTests;
};

/* FibonacciHeap
 *
 * Implemented using a meta root to avoid special handling of the
 * linked list of heap trees.
 */
struct FibonacciHeap {
    // Check whether the heap is empty
    bool is_empty() { return size == 0; }

    /* Insert a new node with given priority and payload and return it.
     *
     * `payload` can be used to store any data in the node.
     */
    Node *insert(priority_t prio, payload_t payload = payload_t()) {
        Node *n = new Node(prio, payload);
        add_child(&meta_root, n);
        size++;
        return n;
    }

    /* Extract node with minimum priority.
     *
     * Must not be called when the heap is empty.
     * Returns pair (payload, priority) of the removed node.
     */
    std::pair<payload_t, priority_t> extract_min() {
        EXPECT(meta_root.rank > 0, "Cannot extract minimum of an empty heap.");

        // Find the tree whose root is minimal.
        Node *min = meta_root.first_child;
        Node *node = min->next_sibling;
        while (node != meta_root.first_child) {
            if (node->prio < min->prio) min = node;
            node = node->next_sibling;
        }

        // Add all its subtrees to the heap.
        while (min->rank > 0)
            add_child(&meta_root, remove(min->first_child));

        // Remove the root.
        std::pair<payload_t, priority_t> ret = { min->payload, min->prio };
        delete remove(min);
        size--;

        // Finally, consolidate the heap.
        consolidate();

        return ret;
    }

    /* Decrease priority of node to new_prio.
     *
     * new_prio must not be higher than node.prio.
     */
    void decrease(Node *node, priority_t new_prio) {
        EXPECT(node, "Cannot decrase null pointer.");
        EXPECT(node->prio >= new_prio, "Decrase: new priority larget than old one.");
        // TODO: Implement
    }

    FibonacciHeap() : size(0), meta_root(priority_t(), payload_t()) {}

    ~FibonacciHeap() {
        if (size == 0) return;
        Node *next;
        Node *n = meta_root.first_child;
        do {
            while (n->first_child) n = n->first_child;
            next = (n->next_sibling != n) ? n->next_sibling : n->parent;
            delete remove(n);
        } while ((n = next) != &meta_root);
    }

  protected:
    // Consolidate heap during extract_min.
    void consolidate() {
        // Calculating max_rank is not cheap and it won't change during consolidate
        size_t current_max_rank = max_rank();

        std::vector<Node*> buckets(current_max_rank, nullptr);
        while (meta_root.first_child) {
            Node *node = remove(meta_root.first_child);
            EXPECT(node->rank < current_max_rank, "Consolidate: node has rank larger than max_rank()");
            while (Node *&b = buckets.at(node->rank)) {
                node = pair_trees(b, node);
                b = nullptr;
            }
            buckets.at(node->rank) = node;
        }

        for (Node *node : buckets)
            if (node) add_child(&meta_root, node);
    }

    // Join two trees of the same rank.
    Node *pair_trees(Node *a, Node *b) {
        if (a->prio > b->prio) std::swap(a, b);
        add_child(a, b);
        return a;
    }

    // Return maximum possible rank of a tree in a heap of the current size.
    virtual size_t max_rank() {
        size_t ret = 1;
        while ((1 << ret) <= size) ret++;
        return ret;
    }

    // Check whether a node is the root.
    bool is_root(Node *n) {
      return n->parent == &meta_root;
    }

    // Link together two elements of linked list -- a and b.
    void join(Node *a, Node *b) {
        a->next_sibling = b;
        b->prev_sibling = a;
    }

    // Add node as a child of a given parent.
    virtual void add_child(Node* parent, Node *node) {
        EXPECT(parent, "add_child: Parent cannot be nullptr.");
        EXPECT(node, "add_child: Node cannot be nullptr.");
        EXPECT(!node->parent, "add_child: Node already has a parent.");
        EXPECT(parent == &meta_root || node->rank == parent->rank,
            "add_child: Ranks of node and parent are different.");
        EXPECT(node->prev_sibling == node && node->next_sibling == node,
            "add_child: Node has a sibling.");
        EXPECT(parent == &meta_root || parent->prio <= node->prio,
            "add_child: Parent has bigger priority than node.");

        if (parent->rank == 0) parent->first_child = node;
        else {
            join(parent->first_child->prev_sibling, node);
            join(node, parent->first_child);
        }

        node->parent = parent;
        parent->rank++;
    }

    // Disconnect a node from its parent.
    virtual Node *remove(Node *n) {
        EXPECT(n->parent, "remove: Cannot disconnect node without parent.");
        EXPECT(n != &meta_root, "remove: Cannot remove meta root.");

        n->parent->rank--;
        if (n->parent->first_child == n) n->parent->first_child = n->next_sibling;
        if (n->parent->rank == 0) n->parent->first_child = nullptr;

        n->parent = nullptr;
        join(n->prev_sibling, n->next_sibling);
        join(n, n);

        return n;
    }

    friend class HeapTests;

    size_t size;
    Node meta_root;
};
