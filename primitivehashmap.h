#ifndef PHASHMAP_H
#define PHASHMAP_H

#include <string>
#include <functional>

template<typename T, typename HashFunction = std::hash<std::string> >
class PrimitiveHashMap {
	public:
		PrimitiveHashMap(int size, HashFunction hash = HashFunction());
		~PrimitiveHashMap();
		bool set(const std::string& key, T value, HashFunction hash = HashFunction());
		T* get(const std::string& key, HashFunction hash = HashFunction());
		T* erase(const std::string& key, HashFunction hash = HashFunction());
		float load();
		int size();
	private:
		int capacity_;
		int size_;
		int bucketsize_;
		int ComputeBucketSize(int capacity);
		T* GetHelper(const std::string& key, int& index);
		void InsertInNextEmptyBucket(const std::string& key, const T& value, int& index);
		T* data_;
		short* state_;
		std::string* keys_;
		enum State {
			EMPTY = (short) 0,
			FILLED = (short) 1,
			DELETED = (short) 2
		};
};

// Constructor, taking in the size parameter for capacity, and an optional HashFunction.
// HashFunction defaults to STL HashFunction
template<typename T, typename HashFunction>
PrimitiveHashMap<T, HashFunction>::PrimitiveHashMap(int size, HashFunction hash)
: capacity_(size), size_(0), bucketsize_(ComputeBucketSize(size))
{
	data_ = new T[bucketsize_];
	// initialize state_ array and declare the value to false (empty)
	state_ = new short[bucketsize_];
	for (int i = 0; i < bucketsize_; ++i) {
		state_[i] = EMPTY;
	}
	keys_ = new std::string[bucketsize_];
}

// Destructor
template<typename T, typename HashFunction>
PrimitiveHashMap<T, HashFunction>::~PrimitiveHashMap() {
	delete[] data_;
	delete[] state_;
	delete[] keys_;
}

// Insert function. Returns bool depending on success
template<typename T, typename HashFunction>
bool PrimitiveHashMap<T, HashFunction>::set(const std::string& key, T value, HashFunction hash) {
	if (size_ >= capacity_) {
		return false;
	}
	int index = hash(key) % bucketsize_;
	T* location = GetHelper(key, index);
	// Key does not exist. Insert into table
	if (location == NULL) {
		if (state_[index] == EMPTY) {
			data_[index] = value;
			state_[index] = FILLED;
			keys_[index] = key;
		} else {
			InsertInNextEmptyBucket(key,value,index);
		}
		++size_;
	} else {
	// Key exist. Update value
		*location = value;
	}
	return true;
}

// Get function. Returns the value associated with the key or NULL if key doesn't exist
template<typename T, typename HashFunction>
T* PrimitiveHashMap<T, HashFunction>::get(const std::string& key, HashFunction hash) {
	int index = hash(key) % bucketsize_;
	return GetHelper(key, index);
}

// Delete function. Returns the value associated with the key or NULL if key doesn't exist
template<typename T, typename HashFunction>
T* PrimitiveHashMap<T, HashFunction>::erase(const std::string& key, HashFunction hash) {
	int index = hash(key) % bucketsize_;
	T* location = GetHelper(key, index);
	if (location == NULL) {
		return NULL;
	}
	--size_;
	state_[index] = DELETED;
	return &data_[index];
}

// Load function. Returns the load factor. Will not exceed one due to open-addressing
template<typename T, typename HashFunction>
float PrimitiveHashMap<T, HashFunction>::load() {
	return static_cast<float>(size_)/static_cast<float>(bucketsize_);
}

// Size function. Returns the number of item in the table
template<typename T, typename HashFunction>
int PrimitiveHashMap<T, HashFunction>::size() {
	return size_;
}

// Helper function to compute a power of 2 for the bucketsize. Modified to double the size before
// finding the closest power of 2. This is to improve the performance of the table.
// Credit goes to: http://graphics.stanford.edu/~seander/bithacks.html#RoundUpPowerOf2
template<typename T, typename HashFunction>
int PrimitiveHashMap<T, HashFunction>::ComputeBucketSize(int capacity) {
	if (capacity == 0) {
		return 0;
	}
	capacity*=2;
	--capacity;
	capacity |= capacity >> 1;
	capacity |= capacity >> 2;
	capacity |= capacity >> 4;
	capacity |= capacity >> 8;
	capacity |= capacity >> 16;
	++capacity;
	return capacity;
}

// Helper function to find the bucket where key resides. Returns NULL if it doesn't exist
template<typename T, typename HashFunction>
T* PrimitiveHashMap<T, HashFunction>::GetHelper(const std::string& key, int& index) {
	if (state_[index] == EMPTY) {
		return NULL;
	}
	for (int i = 0; i < bucketsize_; ++i) {
		if (index == bucketsize_) {
			index = 0;
		}
		if (state_[index] == EMPTY) {
			return NULL;
		}
		if (state_[index] == FILLED && keys_[index] == key) {
			return &data_[index];
		}
		index += 1;
	}
	return NULL;
}

// Helper function to insert into empty slot
template<typename T, typename HashFunction>
void PrimitiveHashMap<T, HashFunction>::InsertInNextEmptyBucket(const std::string& key,const T& value,int& index) {
	for (int i = 0; i < bucketsize_; ++i) {
		if (index == bucketsize_) {
			index = 0;
		}
		if (state_[index] == EMPTY || state_[index] == DELETED) {
			data_[index] = value;
			state_[index] = FILLED;
			keys_[index] = key;
			return;
		}
	}
}
#endif
