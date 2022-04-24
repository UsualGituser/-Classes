//
// Created by Ilya on 11.04.2021.
//

#ifndef LIST_H_FASTALLOCATOR_H
#define LIST_H_FASTALLOCATOR_H
template<size_t chunk_sz>
class FixedAllocator {
public:

    std::vector<char*> memory;
    std::vector<char*> dump;
    char* M_chunk = nullptr;
    size_t pos = 0;
    size_t multiplier = 1;


    void* alloc_new() {
        if (!dump.empty()) {
            void* new_t = reinterpret_cast<void*>(dump.back());
            dump.pop_back();
            return new_t;
        }
        if (!pos) {
            char* ptr = new char[multiplier * chunk_sz];
            memory.push_back(ptr);
            M_chunk = ptr;
            pos = multiplier;
            multiplier *= 2;
        }
        void* ret = reinterpret_cast<void*>(M_chunk);
        M_chunk += chunk_sz;
        --pos;
        return ret;
    }

    void free(char* deleted_ptr) {
        dump.push_back(deleted_ptr);
    }

    ~FixedAllocator() {
        for (size_t i = 0; i < memory.size(); ++i) {
            delete[] memory[i];
        }
    }

};

template<typename T>
class FastAllocator {
public:
    FixedAllocator<sizeof(T)> fxalloc;
    using value_type = T;
    using size_type = size_t;
    using difference_type = std::ptrdiff_t;
    using is_always_equal = std::true_type;


    FastAllocator() = default;

    template<typename U>
    FastAllocator(const FastAllocator<U>&) {}

    template<typename U>
    FastAllocator& operator=(const FastAllocator<U>&) {}


    template<typename Y, typename Alloc>
    auto select_on_container_copy_construction(const Alloc& a) {
        return FastAllocator<Y>();
    }

    T* allocate(size_t size) {
        if (sizeof(T) <= 128 && size == 1) {
            return reinterpret_cast<T*>(fxalloc.alloc_new());
        } else {
            T *new_t = reinterpret_cast<T *>(::operator new(size * sizeof(T)));
            return new_t;
        }
    }

    void deallocate(T* ptr, size_t n) {
        if (sizeof(T) <= 128 && n == 1) {
            fxalloc.free(reinterpret_cast<char*>(ptr));
        } else {
            ::operator delete(ptr, n);
        }
    }

    template<typename... Args>
    void construct(T* ptr, const Args&... constr_args) {
        new (ptr) T(constr_args...);
    }

    void destroy(T* ptr) {
        ptr->~T();
    }

    template<class U>
    struct rebind{
        typedef FastAllocator<U> other;
    };

    ~FastAllocator() = default;
};

template<typename V, typename U>
bool operator==(const FastAllocator<V>&, const FastAllocator<U>&) {
    return std::is_same<V, U>::value;
}

template<typename V, typename U>
bool operator!=(const FastAllocator<V>& a, const FastAllocator<U>& b) {
    return !(a == b);
}


template<typename V, typename U>
bool operator==(const FastAllocator<V>&, const FastAllocator<U>&) {
    return std::is_same<V, U>::value;
}

template<typename V, typename U>
bool operator!=(const FastAllocator<V>& a, const FastAllocator<U>& b) {
    return !(a == b);
}
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
            return  common_bidirectional_iterator<true>(current);
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

private:
        void init_iter() {
            _begin = iterator(first->next);
            _cbegin = const_iterator(first->next);
            _end = iterator(last);
            _cend = const_iterator(last);
        }

public:
    iterator begin(){
        return _begin;
    }

    iterator end(){
        return _end;
    }

    const_iterator begin() const{
        return _cbegin;
    }

    const_iterator end() const{
        return _cend;
    }

    const_iterator cbegin() const{
        return _cbegin;
    }

    const_iterator cend() const {
        return _cend;
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
        new_node->val = this->alloc_.allocate(1);
        AllocTraits_::construct(alloc_, new_node->val, val);
        it.current->prev->next = new_node;
        it.current->prev = new_node;
        init_iter();
        ++sz;
    }

    template<typename Iter>
    void erase(Iter it) {
        it.current->prev->next = it.current->next;
        it.current->next->prev= it.current->prev;
        alloc_.destroy(it.current->val);
        alloc_.deallocate(it.current->val, 1);
        AllocTraits::deallocate(alloc, it.current, 1);
        init_iter();
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
        init_iter();
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
            init_iter();
            ++sz;
        }
    }

    List(size_t count, const T& value, const Alloc& _alloc = Alloc()) {
        alloc_ = AllocTraits_::select_on_container_copy_construction(_alloc);
        sz = 0;
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        init_iter();
        while (sz != count) {
            insert(_end, value);
        }
    }

    List(const List& another) {
        if (alloc_ != another.alloc_) {
            alloc_ = AllocTraits_::select_on_container_copy_construction(another.alloc_);
        }
        AllocTraits::construct(alloc, first, Node(first, last));
        AllocTraits::construct(alloc, last, Node(first, last));
        init_iter();
        sz = 0;
        while (sz < another.sz){
            Node* new_node = AllocTraits::allocate(alloc, 1);
            AllocTraits::construct(alloc, new_node, Node(_end.current->prev, _end.current));
            new_node->val =this->alloc_.allocate(1);
            _end.current->prev->next = new_node;
            _end.current->prev = new_node;
            init_iter();
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
            init_iter();
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


#endif //LIST_H_FASTALLOCATOR_H
