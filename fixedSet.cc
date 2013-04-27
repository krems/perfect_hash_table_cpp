// Author: Valeriy Ovchinnikov

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <vector>

using std::vector;
using std::cin;
using std::cout;
using std::endl;

// I believe that'll be enough big value to consider it not user value
const unsigned long long NOT_USER_VALUE = 3000000000;
// max value of coeffs in hash functions
const size_t MAX_HASH_COEFF = std::numeric_limits<int>::max();

// hash code functor
class HashCode {
 public:
  void InitializeRandomly(size_t power) {
    this->power = power;
    a_coeff = 1 + rand() % (MAX_HASH_COEFF - 1);
    b_coeff = 1 + rand() % (MAX_HASH_COEFF - 1);
  }

  size_t operator () (int number) const {
    return ((a_coeff * number + b_coeff) % PRIME) % power;
  }
 private:
  // That number is prime and it's more than int range
  static const unsigned long long PRIME = 4294967311;
  // hash function coeffs
  unsigned long long a_coeff;
  unsigned long long b_coeff;
  // power of reflection set
  size_t power;
};

class SquareMemoryHashSet {
 public:
  void Initialize(const vector<int>& numbers) {
    value_table.resize(numbers.size() * numbers.size(), NOT_USER_VALUE);
    MakeTable(numbers);
  }

  bool Contains(int number) const {
    if (value_table.empty()) {
      return false;
    }
    size_t hash = hash_code(number);
    return (value_table[hash] == number);
  }

 private:
  // numbers holder
  vector<unsigned long long> value_table;
  // hash function
  HashCode hash_code;

  void MakeTable(const vector<int>& numbers) {
    if (value_table.empty()) {
      return;
    }
    bool filled = false;
    while (!filled) {
      std::fill(value_table.begin(), value_table.end(), NOT_USER_VALUE);
      hash_code.InitializeRandomly(value_table.size());
      filled = true;
      for (vector<int>::const_iterator it = numbers.begin();
           it != numbers.end(); ++it) {
        size_t hash = hash_code(*it);
        if (value_table[hash] != NOT_USER_VALUE) {
          filled = false;
          break;
        } else {
          value_table[hash] = *it;
        }
      }
    }
  }
};

// hash set that may contain ints and can not be changed
// build in E(t)=O(n)
// Contains in O(1)
// needs O(n) memory
class FixedSet {
 public:
  void Initialize(const vector<int>& numbers) {
    // some kind of memset
    table.resize(numbers.size());

    MakeTable(numbers);
  }

  bool Contains(int number) const {
    if (table.empty()) {
      return false;
    }
    size_t hash = hash_code(number);
    return table[hash].Contains(number);
  }
  
 private:
  // ratio of sum of squares of second layer tables' powers to number of numbers
  static const double MEMORY_COEFF = 3;
  // second-layer hash tables holder
  vector<SquareMemoryHashSet> table;
  // hash function
  HashCode hash_code;

  void MakeTable(const vector<int>& numbers) {
    if (table.empty()) {
      return;
    }
    bool created = false;
    vector<vector<int> > cell_content(numbers.size());
    while (!created) {
      std::fill(cell_content.begin(), cell_content.end(), vector<int>());
      hash_code.InitializeRandomly(table.size());
      for (vector<int>::const_iterator it = numbers.begin();
           it != numbers.end(); ++it) {
        size_t hash = hash_code(*it);
        cell_content[hash].push_back(*it);
      }
      unsigned long long second_layer_power = 0;
      for (vector<vector<int> >::const_iterator it = cell_content.begin();
           it != cell_content.end(); ++it) {
        second_layer_power += (static_cast<unsigned long long>(it->size())
                               * it->size());
      }
      // we need linear memory so...
      if (second_layer_power < MEMORY_COEFF * table.size()) {
        created = true;
      }
    }
    for (vector<int>::const_iterator it = numbers.begin();
         it != numbers.end(); ++it) {
      size_t hash = hash_code(*it);
      table[hash].Initialize(cell_content[hash]);
    }
  }
};

void read_set(vector<int>& numbers) {
  size_t set_power;
  cin >> set_power;
  for (size_t i = 0; i < set_power; ++i) {
    int number;
    cin >> number;
    numbers.push_back(number);
  }
}

void read_requests(vector<int>& requests) {
  size_t requests_number;
  cin >> requests_number;
  for (size_t i = 0; i < requests_number; ++i) {
    int number;
    cin >> number;
    requests.push_back(number);
  }
}

void test_numbers(const vector<int>& numbers,
                  const vector<int>& requests,
                  vector<bool>& results) {
  FixedSet set;
  set.Initialize(numbers);
  results.reserve(requests.size());
  for (vector<int>::const_iterator it = requests.begin();
       it != requests.end(); ++it) {
    results.push_back(set.Contains(*it));
  }
}

void write_answer(const vector<bool>& answer) {
  for (vector<bool>::const_iterator it = answer.begin();
       it != answer.end(); ++it) {
    if (*it) {
      cout << "Yes" << endl;
    } else {
      cout << "No" << endl;
    }
  }
}

int main() {
  srand(5);
  vector<int> numbers;
  read_set(numbers);
  vector<int> requests;
  read_requests(requests);
  vector<bool> results;
  test_numbers(numbers, requests, results);
  write_answer(results);
  return 0;
}
