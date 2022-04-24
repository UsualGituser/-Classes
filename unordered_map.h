//
// Created by ilya_ on 23.04.2021.
//

#ifndef UNORDERED_MAP_UNORDERED_MAP_H
#define UNORDERED_MAP_UNORDERED_MAP_H
#include <vector>
#include <stdexcept>
#include <iostream>

template<typename T, typename Alloc = std::allocator<T>>
class List {
private:
    Alloc alloc_;
    size_t sz;

    struct Node {
        T* val;
        mutable Node* prev;
        mutable Node* next;
        Node(Node* prev = nullptr, Node* next = nullptr): prev(prev), next(next) {}

        Node(const Node& another): val(another.val), prev(another.prev), next(another.next){}
        ~Node() = default;
    };

    typedef typename Alloc::template rebind<Node>::other Allocator;
    Allocator alloc;
    using AllocTraits = std::allocator_traits<Allocator>;
    using AllocTraits_ = std::allocator_traits<Alloc>;

    Node* first = alloc.allocate(1);
    Node* last = alloc.allocate(1);

    template<bool IsConst>
    class common_bidirectional_iterator {
    private:
        Allocator alloc;
    public:
        typedef std::ptrdiff_t difference_type;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;
        Node* current;
        common_bidirectional_iterator(): current(nullptr){}
        common_bidirectional_iterator(Node* _current): current(_current) {}

        common_bidirectional_iterator(const common_bidirectional_iterator& another): current(another.current) {}


        common_bidirectional_iterator& operator=(const common_bidirectional_iterator& other) {
            current = other.current;
            return *this;
        }

        operator common_bidirectional_iterator<true>() {
            common_bidirectional_iterator<true> temp(current);
            return temp;
        }

        common_bidirectional_iterator& operator ++() {
            current = current->next;
            return *this;
        }

        common_bidirectional_iterator operator ++(int) {
            common_bidirectional_iterator<IsConst> ret = *this;
            ++*this;
            return ret;
        }

        common_bidirectional_iterator& operator --() {
            current = current->prev;
            return *this;
        }

        common_bidirectional_iterator&operator --(int) {
            common_bidirectional_iterator<IsConst> ret = *this;
            --*this;
            return ret;
        }

        friend bool operator==(const common_bidirectional_iterator& a, const common_bidirectional_iterator& b) {
            return (a.current == b.current);
        }
        friend bool operator!=(const common_bidirectional_iterator& a, const common_bidirectional_iterator& b) {
            return !(a == b);
        }

        std::conditional_t<IsConst, const T&, T&> operator *() {
            return *current->val;
        }

        std::conditional_t<IsConst, const T*, T*> operator->(){
            return current->val;
        }

        ~common_bidirectional_iterator() = default;

    };

public:
    using iterator = common_bidirectional_iterator<false>;
    using const_iterator = common_bidirectional_iterator<true>;
private:
    template<typename Iterator>
    class common_reverse_iterator{
    public:
        typedef std::ptrdiff_t difference_type;
        using value_type = T;
        using pointer = T*;
        using reference = T&;
        using iterator_category = std::bidirectional_iterator_tag;

        Iterator iter;

        common_reverse_iterator(Node* val) {
            iter = Iterator(val);
        }
        common_reverse_iterator(Iterator iter_): iter(iter_){}

        common_reverse_iterator(const common_reverse_iterator& another): iter(another.iter){}

        operator common_reverse_iterator<const_iterator>(){
            common_reverse_iterator<const_iterator> temp (iter);
            return temp;
        }

        common_reverse_iterator& operator=(const common_reverse_iterator& another) {
            iter = another.iter;
            return *this;
        }

        common_reverse_iterator& operator++(){
            --iter;
            return *this;
        }

        common_reverse_iterator& operator ++(int) {
            common_reverse_iterator<Iterator> ret = *this;
            ++*this;
            return ret;
        }

        common_reverse_iterator& operator--() {
            ++iter;
            return *this;
        }

        common_reverse_iterator& operator --(int) {
            common_reverse_iterator<Iterator> ret = *this;
            --*this;
            return ret;
        }

        friend bool operator==(const common_reverse_iterator& a, const common_reverse_iterator& b) {
            return (a.iter == b.iter);
        }

        friend bool operator!=(const common_reverse_iterator& a, const common_reverse_iterator& b) {
            return (a.iter != b.iter);
        }

        auto& operator*(){
            return *iter;
        }

        Iterator base() {
            Iterator res = iter;
            ++res;
            return res;
        }
    };

public:

    using reverse_iterator = common_reverse_iterator<iterator>;
    using const_reverse_iterator = common_reverse_iterator<const_iterator>;

    iterator _begin = iterator(nullptr);
    iterator _end = iterator(nullptr);
    const_iterator _cbegin = const_iterator(nullptr);
    const_iterator _cend = const_iterator(nullptr);

    iterator begin(){
        return iterator(first->next);
    }

    iterator end(){
        return iterator(last);
    }

    const_iterator begin() const{
        return const_iterator(first->next);
    }

    const_iterator end() const{
        return const_iterator(last);
    }

    const_iterator cbegin() const{
        return const_iterator(first->next);
    }

    const_iterator cend() const {
        return const_iterator(last);
    }

    reverse_iterator rbegin() {
        return reverse_iterator(--end());
    }

    reverse_iterator rend() {
        return reverse_iterator(--begin());
    }

    const_reverse_iterator rbegin() const {
        return const_reverse_iterator(--end());
    }

    const_reverse_iterator rend() const{
        return const_reverse_iterator(--begin());
    }

    const_reverse_iterator crbegin() const {
        return const_reverse_iterator(--end());
    }

    const_reverse_iterator crend() const {
        return const_reverse_iterator(--begin());
    }

    template<typename Iter>
    void insert(Iter it, const T& val) {
        Node* new_node = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, new_node, Node(it.current->prev, it.current));
        new_node->val = AllocTraits_::allocate(this->alloc_, 1);
        AllocTraits_::construct(alloc_, new_node->val, val);
        /*new_node->next = it.current;
        new_node->prev = it.current->prev;*/
        it.current->prev->next = new_node;
        it.current->prev = new_node;
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        ++sz;
    }

    template<typename Iter, typename T_>
    void insert(Iter it, T_&& val) {
        Node* new_node = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, new_node, Node(it.current->prev, it.current));
        new_node->val = AllocTraits_::allocate(this->alloc_, 1);
        AllocTraits_::construct(alloc_, new_node->val, std::forward<T_>(val));
        it.current->prev->next = new_node;
        it.current->prev = new_node;
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        ++sz;
    }

    template<typename Iter>
    Node* get_n(Iter it) {
        auto g = it;
        --sz;
        return g.current;
    }
    template<typename Iter>
    void placeNode(Iter it, Node* node) {
        node->prev = it.current->prev;
        node->next = it.current;
        it.current->prev->next = node;
        it.current->prev = node;
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        ++sz;
    }

    template<typename Iter>
    void insertReference(Iter it, T* value) {
        Node* new_node = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, new_node, Node(it.current->prev, it.current));
        new_node->val = value;
        it.current->prev->next = new_node;
        it.current->prev = new_node;
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        ++sz;
    }

    template<typename Iter,typename... Args>
    void emplace(Iter it, Args&&... args) {
        Node* new_node = AllocTraits::allocate(alloc, 1);
        AllocTraits::construct(alloc, new_node, Node(it.current->prev, it.current));
        new_node->val = AllocTraits_::allocate(this->alloc_, 1);
        AllocTraits_::construct(alloc_, new_node->val, std::forward<Args>(args)...);
        it.current->prev->next = new_node;
        it.current->prev = new_node;
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        ++sz;
    }

    template<typename Iter>
    void erase(Iter it) {
        it.current->prev->next = it.current->next;
        it.current->next->prev= it.current->prev;
        alloc_.destroy(it.current->val);
        alloc_.deallocate(it.current->val, 1);
        AllocTraits::deallocate(alloc, it.current, 1);
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        --sz;
    }

    auto& get_allocator() {
        return alloc_;
    }

    explicit List(const Alloc& _alloc = Alloc()) {
        if (alloc_ != _alloc) {
            alloc_ = AllocTraits_::select_on_container_copy_construction(_alloc);
        }
        sz = 0;
        alloc.construct(first, Node(first, last));
        alloc.construct(last, Node(first, last));
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
    }

    List(size_t count, const Alloc& _alloc = Alloc()) {
        alloc_ = AllocTraits_::select_on_container_copy_construction(_alloc);
        sz = 0;
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        _end = iterator(last);
        _cend = const_iterator(last);
        while (sz != count) {
            Node* new_node = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, new_node, Node(_end.current->prev, _end.current));
            new_node->val =this->alloc_.allocate(1);
            new (new_node->val) T();
            _end.current->prev->next = new_node;
            _end.current->prev = new_node;
            _begin = iterator(first->next);
            _cbegin = const_iterator(first->next);
            _end = iterator(last);
            _cend = const_iterator(last);
            ++sz;
        }
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
    }

    List(size_t count, const T& value, const Alloc& _alloc = Alloc()) {
        alloc_ = AllocTraits_::select_on_container_copy_construction(_alloc);
        sz = 0;
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        _end = iterator(last);
        _cend = const_iterator(last);
        while (sz != count) {
            insert(_end, value);
        }
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
    }

    List(const List& another) {
        if (alloc_ != another.alloc_) {
            alloc_ = AllocTraits_::select_on_container_copy_construction(another.alloc_);
        }
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        sz = 0;
        while (sz < another.sz){
            Node* new_node = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, new_node, Node(_end.current->prev, _end.current));
            new_node->val =this->alloc_.allocate(1);
            _end.current->prev->next = new_node;
            _end.current->prev = new_node;
            _begin = iterator(first->next);
            _cbegin = const_iterator(first->next);
            _end = iterator(last);
            _cend = const_iterator(last);
            ++sz;
        }
        Node* next = first->next;
        Node* other_next = another.first->next;
        size_t ctr = 0;
        while (ctr < sz) {
            AllocTraits_::construct(alloc_, next->val, *other_next->val);
            next = next->next;
            other_next = other_next->next;
            ++ctr;
        }
    }

    List(List&& other) {
        alloc_ = std::move(other.alloc);
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        first->next = other.first->next;
        last->prev = other.last->prev;
        other.first->next = other.last;
        other.last->prev = other.first;
        sz = other.sz;
        other.sz = 0;
    }

    List& operator=(const List& another){
        while (sz != 0) {
            erase(--_end);
            _end = iterator(last);
            _cend = const_iterator(last);
        }
        if (AllocTraits_ ::propagate_on_container_copy_assignment::value && alloc_ != another.alloc_) {
            alloc_ = another.alloc_;
        }
        while (sz != another.sz) {
            Node* new_node = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, new_node, Node(_end.current->prev, _end.current));
            new_node->val =  this->alloc_.allocate(1);
            _end.current->prev->next = new_node;
            _end.current->prev = new_node;
            _begin = iterator(first->next);
            _cbegin = const_iterator(first->next);
            _end = iterator(last);
            _cend = const_iterator(last);
            ++sz;
        }
        Node* next = first->next;
        Node* other_next = another.first->next;
        size_t ctr = 0;
        while (ctr < sz) {
            AllocTraits_::construct(alloc_, next->val, *other_next->val);
            next = next->next;
            other_next = other_next->next;
            ++ctr;
        }
        return *this;
    }

    List& operator=(List&& other){
        if (&other == this) {
            return *this;
        }
        while (sz != 0) {
            erase(--_end);
            _end = iterator(last);
            _cend = const_iterator(last);
        }
        if (AllocTraits_ ::propagate_on_container_copy_assignment::value && alloc_ != other.alloc_) {
            alloc_ = other.alloc_;
        }
        alloc_ = std::move(other.alloc);
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        _begin = iterator(first->next);
        _cbegin = const_iterator(first->next);
        _end = iterator(last);
        _cend = const_iterator(last);
        first->next = other.first->next;
        other.first->next->prev = first;
        last->prev = other.last->prev;
        other.last->prev->next = last;
        other.first->next = other.last;
        other.last->prev = other.first;
        sz = other.sz;
        other.sz = 0;
        return *this;
    }

    size_t size() const{
        return sz;
    }

    void push_back(const T& val) {
        insert(_end, val);
    }

    void pop_back() {
        erase(--end());
    }

    void push_front(const T& val) {
        insert(_begin, val);
    }

    void pop_front() {
        erase(begin());
    }

    ~List() {
        while (sz != 0) {
            pop_back();
        }
        alloc.deallocate(first, 1);
        alloc.deallocate(last, 1);
    }
};

template<typename Key, typename Value, typename Hash = std::hash<Key>, \
        typename EqualTo = std::equal_to<Key>, typename Allocator = std::allocator<std::pair<const Key, Value>>>
class UnorderedMap {
private:
    using NodeType = std::pair<const Key, Value>;
    using iterator = typename List<NodeType, Allocator>::iterator;
    using const_iterator = typename List<NodeType, Allocator>::const_iterator;

    std::vector<iterator> hash_table;
    std::vector<size_t> bucket_count;
    List<NodeType, Allocator> elements;
    List<NodeType, Allocator> buffer;
    size_t sz;
    size_t max_load = 1;
    size_t _max_size = std::numeric_limits<size_t>::max() / sizeof(NodeType);
    Allocator alloc;

    void rehash(bool type = true) {
        if (hash_table.empty()) {
            hash_table.resize(1);
            hash_table[0] = elements.end();
            bucket_count.resize(1);
            bucket_count[0] = 0;
            return;
        }
        std::vector<iterator> rehashed_table;
        std::vector<size_t> rehashed_bucket;
        if (type) {
            rehashed_table.resize(std::min(2 * hash_table.size(), _max_size), buffer.end());
            rehashed_bucket.resize(std::min(2 * hash_table.size(), _max_size), 0);
        } else {
            rehashed_table.resize(std::min(hash_table.size(), _max_size), buffer.end());
            rehashed_bucket.resize(std::min(hash_table.size(), _max_size), 0);
        }
        iterator _begin = begin();
        while (_begin != end()) {
            size_t new_pos = Hash{}(_begin->first) % rehashed_table.size();
            buffer.placeNode(rehashed_table[new_pos], elements.get_n(_begin++));
            --rehashed_table[new_pos];
            ++rehashed_bucket[new_pos];
        }
        hash_table = rehashed_table;
        bucket_count = std::move(rehashed_bucket);
        elements = std::move(buffer);
    }

public:

    iterator begin() {
        return elements.begin();
    }

    iterator end() {
        return elements.end();
    }

    const_iterator begin() const{
        return elements.begin();
    }

    const_iterator end() const {
        return elements.end();
    }

    const_iterator cbegin() const{
        return elements.cbegin();
    }

    const_iterator cend() const {
        return elements.cend();
    }

    template<typename Key_>
    iterator find(Key_&& pos) {
        if (hash_table.size() > 0) {
            size_t new_pos = Hash{}(pos) % hash_table.size();
            size_t visited = 0;
            if (hash_table[new_pos] == buffer.end()) {
                hash_table[new_pos] = elements.end();
            }
            iterator it = hash_table[new_pos];
            while (visited != bucket_count[new_pos]) {
                if (EqualTo()(it->first, pos)) {
                    return it;
                }
                ++it;
                if (it == end()) {
                    break;
                }
                ++visited;
            }
        }
        return end();
    }

    template<typename Key_>
    const_iterator find(Key_&& pos) const {
        size_t new_pos = Hash{}(pos) % hash_table.size();
        size_t visited = 0;
        if (hash_table[pos] == buffer.end()) {
            hash_table[pos] = elements.end();
        }
        const_iterator it = hash_table[new_pos];
        while (visited != sz) {
            if (EqualTo()(it->first, pos)) {
                return it;
            }
            ++it;
            if (it == cend()) {
                it = cbegin();
            }
            ++visited;
        }
        return end();
    }

    std::pair<iterator, bool> insert(const NodeType& new_item) {
        if (sz == _max_size) {
            throw std::out_of_range("No space");
        }
        auto it = find(new_item.first);
        if (it != end()) {
            return std::make_pair(it, false);
        }
        if (load_factor() >= max_load) {
            rehash();
        }
        size_t pos = Hash{}(new_item.first) % hash_table.size();
        if (hash_table[pos] == buffer.end()) {
            hash_table[pos] = elements.end();
        }
        elements.insert(hash_table[pos], new_item);
        --hash_table[pos];
        ++bucket_count[pos];
        ++sz;
        return std::make_pair(hash_table[pos], true);
    }

    template<typename NodeType_>
    std::pair<iterator, bool> insert(NodeType_&& new_item) {
        if (sz == _max_size) {
            throw std::out_of_range("No space");
        }
        auto it = find(new_item.first);
        if (it != end()) {
            return std::make_pair(it, false);
        }
        if (load_factor() >= max_load) {
            rehash();
        }
        size_t pos = Hash{}(new_item.first) % hash_table.size();
        if (hash_table[pos] == buffer.end()) {
            hash_table[pos] = elements.end();
        }
        elements.insert(hash_table[pos], std::forward<NodeType_>(new_item));
        --hash_table[pos];
        ++bucket_count[pos];
        ++sz;
        return std::make_pair(hash_table[pos], true);
    }

    template<class InputIterator>
    void insert(InputIterator it1, InputIterator it2) {
        InputIterator it = it1;
        while (it != it2) {
            insert(*(it++));
        }
    }

    template<typename... Args>
    std::pair<iterator, bool> emplace(Args&&... args){
        NodeType* new_node = reinterpret_cast<NodeType *>(std::allocator_traits<Allocator>::allocate(alloc, 1));
        std::allocator_traits<Allocator>::construct(alloc, new_node, std::forward<Args>(args)...);
        if (sz == _max_size) {
            throw std::out_of_range("No space");
        }
        auto it = find(new_node->first);
        if (it != end()) {
            return std::make_pair(it, false);
        }
        if (load_factor() >= max_load) {
            rehash();
        }
        size_t pos = Hash{}(new_node->first) % hash_table.size();
        if (hash_table[pos] == buffer.end()) {
            hash_table[pos] = elements.end();
        }
        elements.insertReference(hash_table[pos], new_node);
        --hash_table[pos];
        ++bucket_count[pos];
        ++sz;
        return std::make_pair(hash_table[pos], true);
    }

    void erase(const_iterator it) {
        size_t pos = Hash{}(it->first)%hash_table.size();
        if (hash_table[pos] == it) {
            if (bucket_count[pos] > 1) {
                it = hash_table[pos]++;
            } else {
                hash_table[pos] = elements.end();
            }
        }
        --bucket_count[pos];
        elements.erase(it);
        --sz;
    }

    void erase(const_iterator it1, const_iterator it2) {
        const_iterator it = it1;
        while (it != it2) {
            auto er = it++;
            size_t pos = Hash{}(er->first)%hash_table.size();
            if (hash_table[pos] == er) {
                if (bucket_count[pos] > 1) {
                    er = hash_table[pos]++;
                } else {
                    hash_table[pos] = elements.end();
                }
            }
            --bucket_count[pos];
            elements.erase(er);
            --sz;
        }
    }

    size_t size() const { return sz;}
    explicit UnorderedMap(const Allocator& alloc_ = Allocator()): sz(0) {
        alloc = std::allocator_traits<Allocator>::select_on_container_copy_construction(alloc_);
    }

    UnorderedMap(const UnorderedMap& other): hash_table(other.hash_table), elements(other.elements), sz(other.sz), max_load(other.max_load), _max_size(other._max_size) {
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            alloc = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc);
        }
    }

    UnorderedMap& operator=(const UnorderedMap& other) {
        if (std::allocator_traits<Allocator>::propagate_on_container_copy_assignment::value) {
            alloc = std::allocator_traits<Allocator>::select_on_container_copy_construction(other.alloc);
        }
        hash_table = other.hash_table;
        elements = other.elements;
        sz = other.sz;
        max_load = other.max_load;
        _max_size = other._max_size;
    }

    UnorderedMap(UnorderedMap&& other) {
        hash_table = std::move(other.hash_table);
        sz = other.sz;
        max_load = other.max_load;
        _max_size = other._max_size;
        other.sz = 0;
    }

    UnorderedMap& operator=(UnorderedMap&& other) {
        if (&other == this) {
            return *this;
        }
        hash_table = std::move(other.hash_table);
        sz = other.sz;
        max_load = other.max_load;
        _max_size = other._max_size;
        other.sz = 0;
        return *this;
    }

    Value& operator[](const Key& pos) {
        NodeType* n = std::allocator_traits<Allocator>::allocate(alloc, 1);
        std::allocator_traits<Allocator>::construct(alloc, n, pos, Value());
        return insert(*n).first->second;
    }

    Value& operator[](Key&& pos) {
        NodeType* n = std::allocator_traits<Allocator>::allocate(alloc, 1);
        std::allocator_traits<Allocator>::construct(alloc, n, std::move(pos), Value());
        return insert(*n).first->second;
    }

    Value& at(const Key& pos) {
        iterator result = find(pos);
        if (result == end()) {
            throw std::out_of_range ("invalid key");
        }
        return result->second;
    }

    Value at(const Key& pos) const{
        iterator result = find(pos);
        if (result == end()) {
            throw std::out_of_range ("invalid key");
        }
        return result->second;
    }

    void reserve(size_t val) {
        hash_table.resize(hash_table.size() + val);
        rehash(false);
    }

    size_t max_size() const{
        return _max_size;
    }

    size_t& max_size() {
        return _max_size;
    }

    size_t max_load_factor() const{
        return max_load;
    }

    size_t& max_load_factor() {
        return max_load;
    }

    float load_factor() const{
        if (hash_table.size() == 0) return 1;
        return (sz / (hash_table.size()));
    }

    ~UnorderedMap() = default;
};






#endif //UNORDERED_MAP_UNORDERED_MAP_H
