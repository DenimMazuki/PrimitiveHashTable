To pre-compile test.h, execute "g++ -O2 -Wall -x c++-header test.h -o test.h.gch"
To compile test file, execute "g++ -O2 -Wall -include test.h primitivehashmap.h  main.cpp"
To run test file, execute "./test"

The table is built using C++ primitive. Optimization is done by choosing a bucketsize
that is approximately 2 times the size of the capacity. The bucketsize is also
set to be a power of 2. The constructor built provides flexibility to add custom
hashfunction. If a custom hashfunction is not provided, the C++ STL hashfunction
will be provided.

Collision is resolved through linear-probing due to the fixed-size constraints.
The operations run in amortized of O(1).
The space complexity is O(n).

The hash table can be further optimized by implementing an additional "linked-list"
to reference the elements in the table to allow iteration. However, because the
key is a string and given the time constraints, I thought the trade-off of extra
memory to store the string keys will not be worth the improve in run-time of
having an additional data structure in the table.

Public API:

bool set(const std::string& key, T value, HashFunction hash = HashFunction())
- Returns true if successful, false otherwise
- If key exists, old value will be overwritten

T* get(const std::string& key, HashFunction hash = HashFunction())
- Returns the pointer if exists, NULL otherwise

T* erase(const std::string& key, HashFunction hash = HashFunction());
- Removes the item and returns the pointer if exists, NULL otherwise

float load()
- Returns the load factor of the table. With this implementation, should not
  exceed 0.5

int size()
- Returns the number of item currently in the table
