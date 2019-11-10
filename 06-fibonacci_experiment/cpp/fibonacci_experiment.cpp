#include <algorithm>
#include <functional>
#include <string>
#include <utility>
#include <vector>
#include <iostream>
#include <cmath>

#include "fibonacci.h"
#include "random.h"

void expect_failed(const std::string& message) {
    fprintf(stderr, "Test error: %s\n", message.c_str());
    exit(1);
}


/*
 *  A modified Fibonacci heap for benchmarking.
 *
 *  We inherit the implementation of operations from the FibonacciHeap class
 *  and extend it by keeping statistics on the number of operations
 *  and the total number of structural changes. Also, if naive is turned on,
 *  the decrease does not mark parent which just lost a child.
 */

class HeapTests : public FibonacciHeap {
    int ops = 0;
    int steps = 0;
    bool naive;
    size_t max_size = 0;

  public:
    HeapTests(bool naive = false) : naive(naive) {}

    void stats() {
        printf(" %.3lf\n", (double)(steps * 1.0 / std::max(1, ops)));
    }

    Node *insert(priority_t prio, payload_t payload = payload_t()) {
        ops++;
        return FibonacciHeap::insert(prio, payload);
    }

    std::pair<payload_t, priority_t> extract_min() {
        ops++;
        return FibonacciHeap::extract_min();
    }

    void decrease(Node *node, priority_t new_prio) {
        ops++;
        if (naive) {
            EXPECT(node, "Cannot decrease null pointer.");
            EXPECT(node->prio >= new_prio, "Decrase: new priority larger than old one.");

            node->prio = new_prio;
            if (is_root(node) || new_prio >= node->parent->prio) return;

            add_child(&meta_root, remove(node));
        } else FibonacciHeap::decrease(node, new_prio);
    }

  protected:
    size_t max_rank() override {
        max_size = std::max(max_size, size);
        if (naive) return sqrt(2*max_size) + 2;
        else return FibonacciHeap::max_rank();
    }

    void add_child(Node* parent, Node *node) override {
        steps++;
        FibonacciHeap::add_child(parent, node);
    }

    Node *remove(Node *n) override {
        steps++;
        return FibonacciHeap::remove(n);
    }
};

using Heap = HeapTests;

RandomGen *rng;         // Random generator object


std::vector<int> log_range(int b, int e, int n) {
    std::vector<int> ret;
    for (int i = 0; i < n; i++)
        ret.push_back(b * exp(log(e / b) / (n - 1) * i));
    return ret;
}

// An auxiliary function for generating a random permutation.
template < typename Vector >
void random_permutation(Vector& v)
{
    for (int i = 0; i < v.size(); i++)
        std::swap(v[i], v[i + rng->next_range(v.size() - i)]);
}

// Remove given node from heap.
void remove(Heap* H, Node *n) {
    H->decrease(n, -1000*1000*1000);
    H->extract_min();
}

// Force consolidation of the heap.
void consolidate(Heap* H) {
    remove(H, H->insert(0));
}

// Construct a star with n nodes and root index r.
void star(Heap *H, Node **node_map, int n, int r, bool consolidate_) {
    if (n == 1) {
        EXPECT(!node_map[r], "");
        node_map[r] = H->insert(r);
        if (consolidate_) consolidate(H);
    } else {
        star(H, node_map, n - 1, r, false);
        star(H, node_map, n - 1, r + n - 1, true);

        for (int i = r + n; i < r + 2*n - 2; i++) {
            remove(H, node_map[i]);
            node_map[i] = nullptr;
        }
    }
}

/* Generates a sequence on which non-cascading heaps need lots of time.
 * Source: "Replacing Mark Bits with Randomness in Fibonacci Heaps" Jerry Li and John Peebles, MIT
 * -> modified so that only a quadratic number of elements are needed in the star construction.
 */
void test_star(bool naive) {
    for (int i = 1; i < 17; i++) {
        int start = 3;
        int N = start + i * (i+1) / 2;
        Heap H(naive);
        Node **node_map = new Node*[N];
        for (int i = 0; i < N; i++) node_map[i] = nullptr;

        for (int j = i; j > 0; j--) {
            star(&H, node_map, j, start, false);
            start += j;
        }

        for (int j = 0; j < (1 << i); j++) {
            H.insert(1);
            H.insert(1);
            H.extract_min();
            H.extract_min();
        }

        printf("%i", N);
        H.stats();

        delete[] node_map;
    }
}

/* A random test.
 *
 * The test does 2N insertions first, and then N random operations insert / decrease /
 * extract_min, each with probability proprotional to its weight ins / dec / rem.
 */
void test_random(int ins, int dec, int rem, bool naive) {
    for (int N : log_range(50, 80000, 30)) {
        Heap H(naive);
        std::vector<Node*> node_map;
        int s = ins + dec + rem;

        std::vector<int> ops;
        for (int i = 0; i < (N*ins) / s; i++) ops.push_back(0);
        for (int i = 0; i < (N*dec) / s; i++) ops.push_back(1);
        for (int i = 0; i < (N*rem) / s; i++) ops.push_back(2);
        random_permutation(ops);

#       define INSERT() node_map.push_back(H.insert(rng->next_range(5*N), node_map.size()))

        for (int i = 0; i < 2*N; i++) INSERT();

        for (int op : ops) switch (op) {
          case 0:
            INSERT();
            break;
          case 1: {
            Node *node = node_map[rng->next_range(node_map.size())];
            int p = node->priority() - rng->next_range(N / 5) - 1;
            H.decrease(node, p);
            break;
          }
          default: {
            auto ret = H.extract_min();
            if (ret.first + 1 != node_map.size()) {
                node_map[ret.first] = node_map.back();
                node_map[ret.first]->payload = ret.first;
            }
            node_map.pop_back();
          }
        }

        printf("%i", N);
        H.stats();

#       undef INSERT
    }
}


std::vector<std::pair<std::string, std::function<void(bool)>>> tests = {
    { "star",       test_star },
    { "random",     [](bool n){ test_random(10, 10, 10, n); } },
    { "biased-10",  [](bool n){ test_random(10, 10, 20, n); } },
    { "biased-40",  [](bool n){ test_random(10, 40, 20, n); } },
    { "biased-70",  [](bool n){ test_random(10, 70, 20, n); } },
    { "biased-100", [](bool n){ test_random(10, 100, 20, n); } },
};

int main(int argc, char **argv)
{
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <test> <student-id> (std|naive)\n", argv[0]);
        return 1;
    }

    std::string which_test = argv[1];
    std::string id_str = argv[2];
    std::string mode = argv[3];

    try {
        rng = new RandomGen(stoi(id_str));
    } catch (...) {
        fprintf(stderr, "Invalid student ID\n");
        return 1;
    }

    bool naive;
    if (mode == "std")
      naive = false;
    else if (mode == "naive")
      naive = true;
    else
      {
        fprintf(stderr, "Last argument must be either 'std' or 'naive'\n");
        return 1;
      }

    for (const auto& test : tests) {
        if (test.first == which_test) {
            test.second(naive);
            return 0;
        }
    }

    fprintf(stderr, "Unknown test %s\n", which_test.c_str());
    return 1;
}
