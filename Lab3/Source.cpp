#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

template <typename K, typename V>
class node {
public:
	node() = default;

	node(const K& key, const V& value) :
		key_(move(key)), value_(move(value)), next_(nullptr) {}

	K get_key() const { return key_; }

	V get_value() const { return value_; }

	void set_value(V value) { value_ = value; }

	node* get_next() const { return next_; }

	void set_next(node* next) { next_ = next; }

private:
	K key_;
	V value_;
	node* next_;
};

template<typename T>
long long get_hash(T value) {
	hash<T> hash_fn;
	uint32_t h = hash_fn(value);
	h = (h ^ 61) ^ (h >> 16);
	h += (h << 3);
	h ^= (h >> 4);
	h *= 0x27d4eb2d;
	h ^= (h >> 15);
	return h;
}

template<typename K, typename V>
class hash_table {
public:
	hash_table() :
		load_factor_(d_load_factor_), capacity_(d_capacity_) {
		table_ = new node<K, V> * [capacity_];

		for (auto i = 0; i < capacity_; ++i) {
			table_[i] = nullptr;
		}
	}

	explicit hash_table(const int capacity) :
		load_factor_(d_load_factor_), capacity_(capacity) {

		if (capacity < d_capacity_) capacity_ = d_capacity_;
		table_ = new node<K, V> * [capacity_];
		m_amount_ = capacity_ * d_load_factor_;

		for (auto i = 0; i < capacity_; ++i) {
			table_[i] = nullptr;
		}
	}

	explicit hash_table(const double load_factor, const int capacity) :
		load_factor_(load_factor), capacity_(capacity) {

		if (capacity < d_capacity_) capacity_ = d_capacity_;
		if (load_factor < 0 || load_factor > 1)
			load_factor_ = d_load_factor_;
		table_ = new node<K, V> * [capacity_];
		m_amount_ = capacity_ * load_factor_;

		for (auto i = 0; i < capacity_; ++i) {
			table_[i] = nullptr;
		}
	}

	int size() const { return size_; }

	int empty() const { return !size(); }

	int normalize(const uint32_t key_hash) const {
		return key_hash % capacity_;
	}

	void clear() {
		for (auto i = 0; i < capacity_; ++i) {
			if (table_[i] == nullptr) continue;
			auto* traverse = table_[i];

			while (traverse != nullptr) {
				auto* item = traverse;
				traverse = traverse->get_next();
				free(item);
			}

			table_[i] = nullptr;
		}
		size_ = 0;
	}
	
private:
	const int d_capacity_ = 6;
	const double d_load_factor_ = 0.8;

	double load_factor_, increase_ = 2.0;
	int capacity_, size_ = 0,
		m_amount_ = d_capacity_ * d_load_factor_;

	node<K, V>** table_;
};

int main()
{
	return 0;
}