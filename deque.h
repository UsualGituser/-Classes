//
// Created by Ilya on 23.03.2021.
//

#ifndef DEQUE_DEQUE_H
#define DEQUE_DEQUE_H

struct deque_pos {
    size_t outer_arr_pos;
    size_t inner_arr_pos;
    deque_pos() = default;
    deque_pos(size_t x, size_t y): outer_arr_pos(x), inner_arr_pos(y) {}
    friend bool operator==(const deque_pos& a, const deque_pos& b) {
        return (a.outer_arr_pos == b.outer_arr_pos && a.inner_arr_pos == b.inner_arr_pos);
    }
    friend bool operator!=(const deque_pos& a, const deque_pos& b) {
        return !(a == b);
    }
};

deque_pos operator+(deque_pos a, int b){
    if (b < 0) {
        if ((int)a.inner_arr_pos >= -b) {
            a.inner_arr_pos += b;
        } else {
            b += a.inner_arr_pos;
            a.inner_arr_pos = 0;
            a.outer_arr_pos += b / 32;
            b = -b;
            b %= 32;
            if (b != 0) {
                --a.outer_arr_pos;
                a.inner_arr_pos = 31 - b + 1;
            }
        }
    } else {
        a.inner_arr_pos += b;
        a.outer_arr_pos += a.inner_arr_pos / 32;
        a.inner_arr_pos %= 32;
    }
    return a;
}

#include <vector>
template <typename T, typename Alloc = std::allocator<T>>
class Deque{
private:
    deque_pos last;
    deque_pos first;
    const size_t inner_sz = 32;
    std::vector<T*> deque;
    size_t sz;
    Alloc alloc;
    using AllocTraits = std::allocator_traits<Alloc>;

    template<bool IsConst>
    struct inner_iterator{
    public:
        inner_iterator() {}
        std::conditional_t<IsConst, const T*, T*> ptr;

        inner_iterator(T* _ptr){
            ptr = _ptr;
        }

        inner_iterator& operator++() {
            ptr += 1;
            return *this;
        }

        inner_iterator& operator--() {
            ptr -= 1;
            return *this;
        }

        friend bool operator<(const inner_iterator& a, const inner_iterator& b) {
            return a.ptr < b.ptr;
        }
        friend bool operator>(const inner_iterator& a, const inner_iterator& b) {
            return !(a < b || a == b);
        }
        friend bool operator<=(const inner_iterator& a, const inner_iterator& b) {
            return (a < b || a == b);
        }
        friend bool operator>=(const inner_iterator& a, const inner_iterator& b) {
            return (a > b || a == b);
        }
        friend bool operator==(const inner_iterator& a, const inner_iterator& b) {
            return a.ptr == b.ptr;
        }
        friend bool operator!=(const inner_iterator& a, const inner_iterator& b) {
            return !(a == b);
        }

        inner_iterator& operator+=(const int& a){
            ptr += a;
            return *this;
        }

        friend inner_iterator operator+(const inner_iterator& a, int b){
            inner_iterator new_it(a);
            new_it += b;
            return new_it;
        }

        inner_iterator& operator-=(const int& a){
            ptr -= a;
            return *this;
        }

        friend inner_iterator operator-(const inner_iterator& a, int b){
            inner_iterator new_it(a);
            new_it -= b;
            return new_it;
        }

        friend int operator-(const inner_iterator& a, const inner_iterator& b) {
            return a.ptr - b.ptr;
        }

        std::conditional_t<IsConst, const T&, T&> operator *() {
            return *ptr;
        }

        std::conditional_t<IsConst, const T*, T*> operator->(){
            return ptr;
        }
    };

public:


    template<bool IsConst>
    struct common_iterator {
    private:
        inner_iterator<IsConst> _it;
        std::vector<T*>& _deque;
    public:
        common_iterator(){}
        deque_pos pos;
        common_iterator(deque_pos new_pos, T* ptr, std::vector<T*>& dek): _it(ptr), _deque(dek), pos(new_pos){}

        common_iterator& operator=(const common_iterator& another) {
            _it = another._it;
            _deque = another._deque;
            pos = another.pos;
            return *this;
        }

        common_iterator& operator++() {
            pos = pos + 1;
            if (pos.inner_arr_pos == 0) {
                _it = inner_iterator<IsConst> (_deque[pos.outer_arr_pos]);
            } else {
                ++_it;
            }
            return *this;
        }

        common_iterator& operator--() {
            pos = pos + -1;
            if (pos.inner_arr_pos == 31) {
                _it = _deque[pos.outer_arr_pos];
                _it += pos.inner_arr_pos;
            } else {
                --_it;
            }
            return *this;
        }

        friend bool operator<(const common_iterator& a, const common_iterator& b) {
            return ((a.pos.outer_arr_pos < b.pos.outer_arr_pos) || (a.pos.outer_arr_pos == b.pos.outer_arr_pos && a._it < b._it));
        }
        friend bool operator==(const common_iterator& a, const common_iterator& b) {
            return a._it == b._it;
        }
        friend bool operator!=(const common_iterator& a, const common_iterator& b) {
            return !(a == b);
        }
        friend bool operator>(const common_iterator& a, const common_iterator& b) {
            return !(a < b || a == b);
        }
        friend bool operator<=(const common_iterator& a, const common_iterator& b) {
            return !(a > b);
        }
        friend bool operator>=(const common_iterator& a, const common_iterator& b) {
            return !(a < b);
        }

        common_iterator& operator+=(const int& a){
            size_t old = pos.outer_arr_pos;
            pos = pos + a;
            if (pos.outer_arr_pos > old) {
                _it = _deque[pos.outer_arr_pos];
                _it += pos.inner_arr_pos;
            } else {
                _it += a;
            }
            return *this;
        }

        friend common_iterator operator+(const common_iterator& a, int b) {
            common_iterator new_it = a;
            new_it += b;
            return new_it;
        }

        common_iterator& operator-=(const int& a){
            size_t old = pos.outer_arr_pos;
            pos = pos + -a;
            if (pos.outer_arr_pos < old) {
                _it = _deque[pos.outer_arr_pos];
                _it += pos.inner_arr_pos;
            } else {
                _it -= a;
            }
            return *this;
        }

        friend common_iterator operator-(const common_iterator& a, int b) {
            common_iterator new_it = a;
            new_it -= b;
            return new_it;
        }

        friend int operator-(const common_iterator& a, const common_iterator& b) {
            if (a.pos.outer_arr_pos == b.pos.outer_arr_pos) {
                return a._it - b._it;
            } else if (a.pos.outer_arr_pos > b.pos.outer_arr_pos){
                return (a.pos.inner_arr_pos + 32 - b.pos.inner_arr_pos + (a.pos.outer_arr_pos - b.pos.outer_arr_pos - 1) * 32);
            } else {
                return b - a;
            }
        }

        std::conditional_t<IsConst, const T&, T&> operator*() {
            return *_it;
        }

        std::conditional_t<IsConst, const T*, T*> operator->(){
            return _it.ptr;
        }
    };

    using iterator = common_iterator<false>;
    using const_iterator = common_iterator<true>;


    const_iterator const_begin = const_iterator (deque_pos(0,0), nullptr, deque);
    const_iterator const_end = const_iterator (deque_pos(0,0), nullptr, deque);
    iterator _begin = iterator (deque_pos(0,0), nullptr, deque);
    iterator _end = iterator (deque_pos(0,0), nullptr, deque);

private:
    void iterator_refresh() {
        const_begin = const_iterator (first, deque[(first).outer_arr_pos] + (first).inner_arr_pos, deque);
        const_end = const_iterator (last + 1, deque[(last + 1).outer_arr_pos] + (last + 1).inner_arr_pos, deque);
        _begin = iterator (first, deque[(first).outer_arr_pos] + (first).inner_arr_pos, deque);
        _end = iterator (last + 1, deque[(last + 1).outer_arr_pos] + (last + 1).inner_arr_pos, deque);
    }

public:

    iterator begin(){
        return _begin;
    }

    iterator end(){
        return _end;
    }

    const_iterator begin() const{
        return const_begin;
    }

    const_iterator end() const{
        return const_end;
    }

    const_iterator cbegin() const {
        return const_begin ;
    }

    const_iterator cend() const {
        return const_end;
    }

    template<typename Iterator>
    struct reverseiterator {
    private:
        Iterator iter;
    public:
        reverseiterator(Iterator it): iter(it){}

        reverseiterator& operator++() {
            --iter;
            return *this;
        }

        reverseiterator& operator--() {
            ++iter;
            return *this;
        }

        friend bool operator<(const reverseiterator& a, const reverseiterator& b) {
            return (a.iter > b.iter);
        }

        friend bool operator==(const reverseiterator& a, const reverseiterator& b) {
            return (a.iter == b.iter);
        }

        friend bool operator!=(const reverseiterator& a, const reverseiterator& b) {
            return !(a == b);
        }

        friend bool operator>(const reverseiterator& a, const reverseiterator& b) {
            return !(a == b || a < b);
        }

        friend bool operator<=(const reverseiterator& a, const reverseiterator& b) {
            return (a < b && a == b);
        }

        friend bool operator>=(const reverseiterator& a, const reverseiterator& b) {
            return !(a < b);
        }

        reverseiterator& operator+=(const int& a) {
            for (int i = 0; i < a; ++i) {
                --iter;
            }
            return *this;
        }

        friend reverseiterator operator+(const reverseiterator& a, const int& b) {
            reverseiterator new_it;
            new_it.iter = a.iter;
            return new_it += b;
        }

        reverseiterator& operator-=(const int& a) {
            for (int i = 0; i < a; ++i) {
                ++iter;
            }
            return *this;
        }

        friend reverseiterator operator-(const reverseiterator& a, const int& b) {
            reverseiterator new_it;
            new_it.iter = a.iter;
            return new_it -= b;
        }

        friend int operator-(const reverseiterator& a, const reverseiterator& b) {
            return (b.iter - a.iter);
        }
    };

    using reverse_iterator = reverseiterator<iterator>;
    using const_reverse_iterator = reverseiterator<const_iterator>;

    reverse_iterator rbegin() {
        return reverse_iterator(end() - 1);
    }

    reverse_iterator rend() {
        return reverse_iterator(begin() - 1);
    }

    const_reverse_iterator crbegin() {
        return const_reverse_iterator(end() - 1);
    }

    const_reverse_iterator crend() {
        return const_reverse_iterator(begin() - 1);
    }


    void insert(iterator it, const T& val){
        deque_pos ps = last;
        deque_pos pos = it.pos;
        if (pos == last + 1) {
            push_back(val);
        } else {
            if (last.outer_arr_pos == deque.size() - 1 && last.inner_arr_pos == inner_sz - 2) {
                size_t old_sz = deque.size();
                deque.resize(deque.size() * 2);
                for (size_t i = 0; i < old_sz; ++i) {
                    deque[i + old_sz] = alloc.allocate(inner_sz);
                }
            }
            while (ps != pos) {
                std::swap(deque[ps.outer_arr_pos][ps.inner_arr_pos], deque[(ps + 1).outer_arr_pos][(ps + 1).inner_arr_pos]);
                ps = ps + -1;
            }
            std::swap(deque[ps.outer_arr_pos][ps.inner_arr_pos], deque[(ps + 1).outer_arr_pos][(ps + 1).inner_arr_pos]);
            ps = ps + -1;
            deque[(ps + 1).outer_arr_pos][(ps + 1).inner_arr_pos] = val;
            last = last + 1;
            ++_end;
            ++const_end;
            ++sz;
        }
    }
    void erase(iterator it) {
        deque_pos ps = it.pos;
        if (!((ps.outer_arr_pos == last.outer_arr_pos && ps.inner_arr_pos > last.inner_arr_pos) || (last.outer_arr_pos > ps.outer_arr_pos))) {
            while (ps != last) {
                std::swap(deque[ps.outer_arr_pos][ps.inner_arr_pos], deque[(ps + 1).outer_arr_pos][(ps + 1).inner_arr_pos]);
                ps = ps + 1;
            }
            last = last + -1;
            --_end;
            --const_end;
            --sz;
        }
        std::cerr <<'\n';
    }


    Deque() {
        try {
            sz = 0;
            deque.resize(2);
            deque[0] = alloc.allocate(inner_sz);
            deque[1] = alloc.allocate(inner_sz);
            last = deque_pos(0, 31);
            first = deque_pos(1, 0);
            iterator_refresh();
        } catch (...) {
            deque.clear();
            throw;
        }
    }

    Deque(int _sz) {
        try {
            sz = _sz;
            size_t cap = (sz % inner_sz == 0) ? (sz / inner_sz) : ((sz / inner_sz) + 1);
            T *new_arr = AllocTraits::allocate(alloc, inner_sz);
            deque.push_back(new_arr);
            for (size_t i = 0; i < cap; ++i) {
                T *new_arr = AllocTraits::allocate(alloc, inner_sz);
                deque.push_back(new_arr);
            }
            first = deque_pos(1, 0);
            last = deque_pos(cap, sz - inner_sz * (cap - 1) - 1);
            iterator_refresh();
        } catch(...) {
            deque.clear();
            throw;
        }
    }

    Deque(const Deque<T>& another) {
        try {
            sz = another.sz;
            deque.resize(another.deque.size());
            for (size_t i = 0; i < another.deque.size(); ++i) {
                T *copy_vec = alloc.allocate(inner_sz);
                for (size_t j = 0; j < inner_sz; ++j) {
                    alloc.construct(copy_vec + j, another.deque[i][j]);
                }
                deque[i] = copy_vec;
            }
            first = another.first;
            last = another.last;
            iterator_refresh();
        } catch(...) {
            deque.clear();
            throw;
        }
    }

    Deque& operator=(const Deque<T>& another) {
        sz = another.sz;
        deque.resize(another.deque.size());
        for (size_t i = 0; i < another.deque.size(); ++i) {
            T* copy_vec = alloc.allocate(inner_sz);
            for (size_t j = 0; j < inner_sz; ++j) {
                alloc.construct(copy_vec + j, another.deque[i][j]);
            }
            deque[i] = copy_vec;
        }
        first = another.first;
        last = another.last;
        const_begin = const_iterator (first + 0, deque[(first + 0).outer_arr_pos] + (first + 0).inner_arr_pos, deque);
        const_end = const_iterator (last, deque[last.outer_arr_pos] + inner_sz, deque);
        _begin = iterator (first + 0, deque[(first + 0).outer_arr_pos] + (first + 0).inner_arr_pos, deque);
        _end = iterator (last, deque[last.outer_arr_pos] + inner_sz, deque);
        return *this;
    }

    Deque(int _sz, const T& val){
        sz = _sz;
        size_t cap = (sz % inner_sz == 0) ? (sz / inner_sz) : ((sz / inner_sz) + 1);
        T* new_arr = alloc.allocate(inner_sz);
        deque.push_back(new_arr);
        try {
            for (size_t i = 0; i < cap - 1; ++i) {
                T* arr = alloc.allocate(inner_sz);
                for (size_t j = 0; j < inner_sz; ++i) {
                    alloc.construct(arr + i, val);
                }
                deque.push_back(arr);          /// Exception
            }
            T* new_arr = alloc.allocate(inner_sz);
            deque.push_back(new_arr);
            for (size_t i = 0; i < sz - (cap - 1) * inner_sz; ++i) {
                alloc.construct(deque[deque.size() - 1] + i, val);
            }
            first = deque_pos(1, 0);
            last = deque_pos(cap, sz - inner_sz * (cap - 1) - 1);
            const_begin = const_iterator (first, deque[(first).outer_arr_pos] + (first).inner_arr_pos, deque);
            const_end = const_iterator (last + 1, deque[(last + 1).outer_arr_pos] + (last + 1).inner_arr_pos, deque);
            _begin = iterator (first, deque[(first).outer_arr_pos] + (first).inner_arr_pos, deque);
            _end = iterator (last + 1, deque[(last + 1).outer_arr_pos] + (last + 1).inner_arr_pos, deque);
        } catch (...) {
            deque.clear();
            throw;
        }
    }

    size_t size() const noexcept{
        return sz;
    }

    T& operator[](size_t index) {
        deque_pos pos = first + 0;
        pos = pos + index;
        return deque[pos.outer_arr_pos][pos.inner_arr_pos];
    }

    T operator[](size_t index) const{
        deque_pos pos = first + 0;
        return deque[(pos + index).outer_arr_pos][(pos + index).inner_arr_pos];
    }

    T& at(size_t index) {
        if ((index >= sz) || (index < 0)) throw std::out_of_range("out of range");
        deque_pos pos = first + 0;
        return deque[(pos + index).outer_arr_pos][(pos + index).inner_arr_pos];
    }

    T at(size_t index) const{
        if ((index >= sz) || (index < 0)) throw std::out_of_range("out of range");
        deque_pos pos = first + 0;
        return deque[(pos + index).outer_arr_pos][(pos + index).inner_arr_pos];
    }

    void push_back(const T& val){
        bool resized = false;
        if (last.outer_arr_pos == deque.size() - 1 && last.inner_arr_pos == inner_sz - 2) {
            size_t old_sz = deque.size();
            deque.resize(deque.size() * 2);
            for (size_t i = 0; i < old_sz; ++i) {
                deque[i + old_sz] = alloc.allocate(inner_sz);
            }
            resized = true;
        }
        try {
            alloc.construct(deque[(last  + 1).outer_arr_pos] + (last + 1).inner_arr_pos, val);
            last = last + 1;
            ++const_end;
            ++_end;
            ++sz;
        } catch(...) {
            if (resized) {
                deque.resize(deque.size() / 2);
            }
            throw;
        }
    }

    void pop_back() {
        last = last + -1;
        --const_end;
        --_end;
        --sz;
    }

    void push_front(const T& val){
        bool resized = false;
        if (first.outer_arr_pos == 0 && first.inner_arr_pos == 0) {
            size_t old_sz = deque.size();
            deque.resize(deque.size() * 2);
            for (size_t i = 0; i < old_sz; ++i) {
                deque[i + old_sz] = alloc.allocate(inner_sz);
                std::swap(deque[i + old_sz], deque[i]);
            }
            first.outer_arr_pos = old_sz;
            last.outer_arr_pos += old_sz;
            _begin.pos = first;
            const_begin.pos = first;
            _end.pos = last + 1;
            const_end.pos = last + 1;
            resized = true;
        }
        try {
            alloc.construct(deque[(first + -1).outer_arr_pos] + (first + -1).inner_arr_pos, val);
            first = first + -1;
            --_begin;
            --const_begin;
            ++sz;
        } catch(...) {
            if (resized) {
                for (size_t i = deque.size() / 2; i < deque.size(); ++i) {
                    deque[i - deque.size() / 2] = std::move(deque[i]);
                    deque.resize(deque.size() / 2);
                }
            }
            throw;
        }
    }

    void pop_front(){
        first = first + 1;
        ++_begin;
        ++const_begin;
        --sz;
    }

    ~Deque() {
        for (size_t i = 0; i < deque.size(); ++i) {
            alloc.deallocate(deque[i], 1);
        }
    }
};


#endif //DEQUE_DEQUE_H
