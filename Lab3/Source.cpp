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

	V get(const K& key) {
		auto index = normalize(get_hash(key));
		auto* entry = table_[index];

		while (entry != nullptr) {
			if (entry->get_key() == key) {
				return entry->get_value();
			}
			entry = entry->get_next();
		}

		return "Can not find this word";
	}
	void insert(const K& key, const V& value) {
		auto index = normalize(get_hash(key));
		node<K, V>* prev = nullptr;
		auto* entry = table_[index];

		while (entry != nullptr && entry->get_key() != key) {
			prev = entry;
			entry = entry->get_next();
		}

		if (entry == nullptr) {
			entry = new node<K, V>(key, value);
			if (prev == nullptr) {
				table_[index] = entry;
			}
			else {
				prev->set_next(entry);
			}
		}
		else {
			entry->set_value(value);
		}

		++size_;
		if (size() > m_amount_) resize_table();
	}
	
	void remove(const K& key) {
		auto index = normalize(get_hash(key));
		node<K, V>* prev = nullptr;
		auto* entry = table_[index];

		while (entry != nullptr && entry->get_key() != key) {
			prev = entry;
			entry = entry->get_next();
		}

		if (entry == nullptr) return;

		if (prev == nullptr) {
			table_[index] = entry->get_next();
		}
		else {
			prev->set_next(entry->get_next());
		}

		--size_;
		free(entry);
	}

	~hash_table() {
		clear();
		free(table_);
		table_ = nullptr;
	}
	
private:

	int normalize(const uint32_t key_hash) const {
		return key_hash % capacity_;
	}
	
	void resize_table() {
		capacity_ *= increase_;
		m_amount_ = capacity_ * load_factor_;
		auto** new_table = new node<K, V> * [capacity_];

		for (auto i = 0; i < capacity_; ++i) {
			new_table[i] = nullptr;
		}

		for (auto i = 0; i < capacity_ / increase_; ++i) {
			if (table_[i] == nullptr) continue;
			auto* item = table_[i];

			while (item != nullptr) {
				auto index = normalize(get_hash(item->get_key()));
				node<K, V>* prev = nullptr;
				auto* entry = new_table[index];

				while (entry != nullptr) {
					prev = entry;
					entry = entry->get_next();
				}

				entry = new node<K, V>(item->get_key(), item->get_value());
				if (prev == nullptr) {
					new_table[index] = entry;
				}
				else {
					prev->set_next(entry);
				}

				auto* copy = item;
				item = item->get_next();
				table_[i] = item;
				free(copy);
			}
			table_[i] = nullptr;
		}
		free(table_);
		table_ = new_table;
	}
	
	const int d_capacity_ = 6;
	const double d_load_factor_ = 0.8;

	double load_factor_, increase_ = 2.0;
	int capacity_, size_ = 0,
		m_amount_ = d_capacity_ * d_load_factor_;

	node<K, V>** table_;
};

string input() {
	string sentence;
	getline(cin, sentence);
	return sentence;
}

class words_meaning {
public:

	words_meaning() {
		string filename, sentence;
		cout << "Enter path to dictionary ";
		getline(cin, filename);
		load_dictionary(filename);
		cout << "Type sentence: ";
		getline(cin, sentence);
		parse(sentence);
	}

	void output() {
		for (auto& word : words_) {
			transform(begin(word), end(word),
				begin(word), toupper);
			cout << dictionary_.get(word) << "\n\n";
		}
	}
	
private:
	
	void parse(string& sentence) {
		auto position = 0; sentence += " ";
		const string ws = ",.;:!?/\'\"{}[]()";
		while (sentence.find(' ', position) != string::npos) {
			const auto current = sentence.find(' ', position);
			const auto length = current - position;
			auto word = sentence.substr(position, length);
			const auto left = word.find_first_not_of(ws);
			const auto right = word.find_last_not_of(ws);
			if (left == string::npos) {
				position = current + 1; continue;
			}
			word = word.substr(left, right - left + 1);
			position = current + 1;
			words_.push_back(word);
		}
		sentence.pop_back();
	}

	void load_dictionary(const string& filename) {
		ifstream in(filename);
		if (in.is_open()) {
			cout << "Starting loading dictionary" << endl;
			while (!in.eof()) {
				string key, value;
				getline(in >> ws, key, ';');
				getline(in >> ws, value);
				dictionary_.insert(key, value);
			}
			in.close();
			cout << "Dictionary downloaded" << endl;
		}
		else {
			cerr << "Can not open file " + filename;
			exit(1);
		}
	}
	
	hash_table<string, string> dictionary_;
	vector<string> words_;
};

int main()
{
	words_meaning meaning;
	meaning.output();
	return 0;
}