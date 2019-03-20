#include <stdio.h>
#include <utility>
#include <functional>
#include <string>
#include <limits>
#include <algorithm>

#include "fibonacci.h"

struct HeapTests {
    using Heap = FibonacciHeap;

    // Good enough infinity for our tests.
    static const int infty = 1000*1000*1000;

    /* Iterator over all children of given node.
     * Also implements begin() and end() so it can
     * be plugged into for-each cycle.
     */
    class NodeIterator {
        Node *root;
        Node *node;
        bool recursive;

        public:
        NodeIterator begin() { return *this; }
        NodeIterator end() { return NodeIterator(); }
        bool operator !=(const NodeIterator& b) { return node != b.node; }

        Node *operator*() { return node; }
        NodeIterator &operator++() {
            if (recursive && node->rank > 0) {
                node = node->first_child;
            } else {
                while (node->next_sibling == node->parent->first_child) {
                    node = node->parent;
                    if (node == root) {
                        node = nullptr;
                        return *this;
                    }
                }
                node = node->next_sibling;
            }
            return *this;
        }

        NodeIterator(Node *root = nullptr, bool recursive = true) : root(root),
            node(root ? root->first_child : nullptr), recursive(recursive) {}
    };

    // Helper of show_heap.
    static void show_node(Node *node, int indent) {
        for (int i = 0; i < indent; i++) printf("  ");
        printf("- %i (payload: %i)\n", node->priority(), node->payload);
        for (Node* child : NodeIterator(node, false))
            show_node(child, indent + 1);
    }

    // Show heap in human-readable form.
    static void show_heap(Heap* H) {
        printf("Heap of size %i\n", (int)H->size);
        for (Node* root : NodeIterator(&H->meta_root, false))
            show_node(root, 0);
        printf("\n");
    }

    // Check heap invariants.
    static void audit_heap(Heap* H) {
        int size = 0;
        for (Node* node : NodeIterator(&H->meta_root)) {
            size++;
            EXPECT(H->is_root(node) || node->parent->priority() <= node->priority(),
                "Parent has bigger priority than its child.");
            std::vector<int> children;
            for (Node *c : NodeIterator(node, false)) {
                children.push_back(c->rank);
                EXPECT(c->parent == node, "Inconsistent parent pointer.");
                EXPECT(c->next_sibling->prev_sibling == c, "Inconsistent sibling pointers.");
                EXPECT(c->prev_sibling->next_sibling == c, "Inconsistent sibling pointers.");
            }
            std::sort(children.begin(), children.end());
            EXPECT(children.size() == node->rank,
                "Rank of node does not match its real number of children.");
            for (int i = 0; i < children.size(); i++)
                EXPECT(children[i] >= i - 1, "Child of the node has too small rank.");
        }
        EXPECT(size == H->size, "Size of the heap does not match real number of its nodes.");
    }

    // Remove given node from heap.
    static void remove(Heap* H, Node *n) {
        H->decrease(n, -infty);
        H->extract_min();
    }

    // Force consolidation of the heap.
    static void consolidate(Heap* H) {
        remove(H, H->insert(0));
    }

    /* Remove the subtree rooted in node from the heap.
     *
     * Note that this method slightly cheats because it
     * learns what nodes are in the subtree by walking over it.
     */
    static void remove_subtree(Heap* H, Node *node) {
        std::vector<Node*> to_remove;

        for (Node* n : NodeIterator(node)) to_remove.push_back(n);
        to_remove.push_back(node);

        for (Node* n : to_remove) remove(H, n);
    }

    /* Build a tree of a given rank with as few nodes as possible.
     *
     * Return pair (root of the tree, its child with the largest rank).
     */
    static std::pair<Node*,Node*> build_sparse_tree(Heap *H, int rank, int prio) {
        if (rank == 0) return { H->insert(prio), nullptr };

        auto A = build_sparse_tree(H, rank - 1, prio);
        auto B = build_sparse_tree(H, rank - 1, prio + 1);

        consolidate(H);
        if (B.second) remove_subtree(H, B.second);

        return { A.first, B.first };
    }

    // Check whether H contains exacly one path and nothing else.
    static void check_path(Heap* H) {
        Node *node = &H->meta_root;
        while (node->rank == 1) node = node->first_child;
        EXPECT(node->rank == 0, "Expected path but found node of rank larger than 1.");
    }

    /* Build a path of a given length.
     *
     * Return lowest node on this path. All nodes on path are marked.
     */
    static Node* build_path(Heap *H, int length) {
        Node *a, *b;
        std::vector<Node*> to_remove;
        H->insert(length + 1);
        Node *end = H->insert(length + 2);
        consolidate(H);
        to_remove.push_back(H->insert(length + 2));
        a = H->insert(length + 3);
        consolidate(H);
        remove(H, a);

        for (int i = length; i > 0; i--) {
            H->insert(i);
            to_remove.push_back(H->insert(i + 1));
            consolidate(H);
            a = H->insert(i + 1);
            b = H->insert(i + 2);
            consolidate(H);
            remove(H, b);
            remove(H, a);
        }

        for (Node *n : to_remove) remove(H, n);

        check_path(H);

        return end;
    }

    /* A simple random test.
     *
     * It does in order:
     * - randomly insert elements,
     * - do random decreases,
     * - extract_min until the heap is empty.
     */
    static void test_random(int size, bool print) {
        Heap H;
        std::vector<std::pair<Node*, int>> node_map;

        for (int i = 0; i < size; i++) {
            int prio = (1009 * i) % 2099;
            node_map.push_back({ H.insert(prio, i), prio });
        }

        consolidate(&H);
        if (print) show_heap(&H);

        for (int i = 0; i < size; i++) {
            if (i % (size / 10) == 0) audit_heap(&H);
            int idx = (i * 839) % node_map.size();
            auto& X = node_map[idx];
            EXPECT(X.first->priority() == X.second,
                "Priority of node changed but its was not decreased.");
            int new_prio = X.second - ((i * 131 + 15) % 239);
            H.decrease(X.first, new_prio);
            EXPECT(X.first->priority() == new_prio,
                "Decrease failed to change priority of the node.");
            if (print) {
                printf("Decrease %i -> %i (payload %i)\n", X.second, new_prio, X.first->payload);
                show_heap(&H);
            }
            X.second = new_prio;
        }

        int last = std::numeric_limits<int>::min();
        audit_heap(&H);
        while (!H.is_empty()) {
            auto x = H.extract_min();
            payload_t payload = x.first;
            priority_t prio = x.second;
            EXPECT(last <= prio, "extract_min is not monotone.");
            EXPECT(node_map[payload].first,
                "Extracted node was already deleted or something changed its payload.");
            EXPECT(prio == node_map[payload].second,
                "Priority of extracted node is wrong.");
            last = prio;
            node_map[payload].first = nullptr;
            if (print) {
                printf("Extract min %i (payload %i)\n", prio, payload);
                show_heap(&H);
            }
        }

        for (auto& X : node_map) EXPECT(X.first == nullptr, "Not all nodes were deleted.");
    }

    // Build a sparse tree of given rank and then empty the heap.
    static void test_sparse_tree(int rank) {
        Heap H;
        build_sparse_tree(&H, rank, 1);
        audit_heap(&H);
        while (!H.is_empty()) H.extract_min();
    }

    // Build a path, decrease it lowest node, and empty the heap.
    static void test_path(int length) {
        Heap H;
        H.decrease(build_path(&H, length), -infty);
        for (Node *n : NodeIterator(&H.meta_root))
            EXPECT(n->rank == 0, "Decrease of lowest node should have broken "
                   "the path into forest of isolated vertices but it has not.");
        audit_heap(&H);
        while (!H.is_empty()) H.extract_min();
    }
};

std::vector<std::pair<std::string, std::function<void()>>> tests = {
    { "small",  [] { HeapTests::test_random(11, true); } },
    { "random", [] { HeapTests::test_random(10000, false); } },
    { "sparse", [] { HeapTests::test_sparse_tree(16); } },
    { "path",   [] { HeapTests::test_path(5000); } },
};
