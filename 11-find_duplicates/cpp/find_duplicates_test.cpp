#include <cmath>
#include <functional>
#include <iterator>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

// If the condition is not true, report an error and halt.
#define EXPECT(condition, message) do { if (!(condition)) expect_failed(message); } while (0)

void expect_failed(const string& message);

class DataGenerator {
 public:
   class Iterator : iterator<input_iterator_tag, string> {
     public:
       Iterator(int counter, DataGenerator* generator) { this->counter = counter; this->generator = generator; }
       Iterator(const Iterator& other) { this->counter = other.counter; this->generator = other.generator; }
       Iterator& operator++() { if (counter < generator->total_size) counter++; return *this; }
       Iterator operator++(int) { Iterator tmp(*this); operator++(); return tmp; }
       bool operator==(const Iterator& other) const { return counter == other.counter; }
       bool operator!=(const Iterator& other) const { return counter != other.counter; }
       const string& operator*() {
           data.clear();

           int segment = counter / generator->segment_size, index = counter % generator->segment_size;
           if (index + 1 == generator->segment_size) {
               data.push_back('0' + generator->base - 1 - segment);
               data.append(generator->suffix);
           } else {
               data.push_back('0' + segment);
               for (int length = generator->length - 1; length; length--, index /= generator->base)
                   data.push_back('0' + (index % generator->base));
           }
           return data;
       }

     private:
       DataGenerator* generator;
       string data;
       int counter;
   };

   inline Iterator begin() { return Iterator(0, this); }
   inline Iterator end() { return Iterator(total_size, this); }

   DataGenerator(int base, int length, string suffix) {
       this->base = base;
       this->length = length;
       this->suffix = suffix;
       segment_size = powl(base, length - 1) + 1;
       total_size = base * segment_size;
   };

 private:
   int base, length;
   int segment_size, total_size;
   string suffix;
};

#include "find_duplicates.h"

#ifdef __linux__
#include <sys/time.h>
#include <sys/resource.h>
#endif

void test_duplicates(int base, int length, string suffix) {
#ifdef __linux__
    rlimit data_limit;
    data_limit.rlim_cur = data_limit.rlim_max = 64 << 20;
    setrlimit(RLIMIT_DATA, &data_limit);
#endif

    DataGenerator generator(base, length, suffix);
    auto results = find_duplicates(generator);

    vector<string> correct;
    for (int i = 0; i < base / 2; i++) {
        correct.push_back(string(1, '0' + i) + suffix);
        correct.push_back(string(1, '0' + base - 1 - i) + suffix);
    }

    EXPECT(results.size() == correct.size(),
           "Wrong number of generated duplicates, got " + to_string(results.size()) +
           " and expected " + to_string(correct.size()));
    for (int i = 0; i < int(results.size()); i++)
        EXPECT(results[i] == correct[i],
               "Wrong generated duplicate, got " + results[i] + " and expected " + correct[i]);
}

vector<pair<string, function<void()>>> tests = {
    {"10k", [] { test_duplicates(10, 4, "101"); }},
    {"100k", [] { test_duplicates(10, 5, "1984"); }},
    {"1M", [] { test_duplicates(10, 6, "22222"); }},
    {"10M", [] { test_duplicates(10, 7, "314159"); }},
    {"16M", [] { test_duplicates(8, 8, "7654321"); }},
};
