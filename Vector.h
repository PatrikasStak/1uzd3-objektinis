#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include <utility>

/**
 * @file Vector.h
 * @brief Dinaminio masyvo šabloninė klasė – std::vector pakaitalas.
 * @author Patrikas
 */

/**
 * @class Vector
 * @brief Dinaminis masyvas su automatišku atminties valdymu.
 * @tparam T Elementų tipas.
 */
template <typename T>
class Vector {
public:
    using value_type      = T;
    using size_type       = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference       = T&;
    using const_reference = const T&;
    using pointer         = T*;
    using const_pointer   = const T*;

    // ── Iteratorius ───────────────────────────────────────────────────────────
    class iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = T*;
        using reference         = T&;

        iterator() : ptr(nullptr) {}
        explicit iterator(T* p) : ptr(p) {}

        reference operator*()  const { return *ptr; }
        pointer   operator->() const { return ptr; }
        reference operator[](difference_type n) const { return ptr[n]; }

        iterator& operator++() { ++ptr; return *this; }
        iterator  operator++(int) { iterator t(*this); ++ptr; return t; }
        iterator& operator--() { --ptr; return *this; }
        iterator  operator--(int) { iterator t(*this); --ptr; return t; }

        iterator& operator+=(difference_type n) { ptr += n; return *this; }
        iterator& operator-=(difference_type n) { ptr -= n; return *this; }
        iterator  operator+(difference_type n) const { return iterator(ptr + n); }
        iterator  operator-(difference_type n) const { return iterator(ptr - n); }
        difference_type operator-(const iterator& o) const { return ptr - o.ptr; }

        friend iterator operator+(difference_type n, const iterator& it) {
            return iterator(it.ptr + n);
        }

        bool operator==(const iterator& o) const { return ptr == o.ptr; }
        bool operator!=(const iterator& o) const { return ptr != o.ptr; }
        bool operator< (const iterator& o) const { return ptr <  o.ptr; }
        bool operator<=(const iterator& o) const { return ptr <= o.ptr; }
        bool operator> (const iterator& o) const { return ptr >  o.ptr; }
        bool operator>=(const iterator& o) const { return ptr >= o.ptr; }

        T* base() const { return ptr; }
    private:
        T* ptr;
    };

    // ── Const iteratorius ─────────────────────────────────────────────────────
    class const_iterator {
    public:
        using iterator_category = std::random_access_iterator_tag;
        using value_type        = T;
        using difference_type   = std::ptrdiff_t;
        using pointer           = const T*;
        using reference         = const T&;

        const_iterator() : ptr(nullptr) {}
        explicit const_iterator(const T* p) : ptr(p) {}
        const_iterator(const iterator& it) : ptr(it.base()) {}

        reference operator*()  const { return *ptr; }
        pointer   operator->() const { return ptr; }
        reference operator[](difference_type n) const { return ptr[n]; }

        const_iterator& operator++() { ++ptr; return *this; }
        const_iterator  operator++(int) { const_iterator t(*this); ++ptr; return t; }
        const_iterator& operator--() { --ptr; return *this; }
        const_iterator  operator--(int) { const_iterator t(*this); --ptr; return t; }

        const_iterator& operator+=(difference_type n) { ptr += n; return *this; }
        const_iterator& operator-=(difference_type n) { ptr -= n; return *this; }
        const_iterator  operator+(difference_type n) const { return const_iterator(ptr + n); }
        const_iterator  operator-(difference_type n) const { return const_iterator(ptr - n); }
        difference_type operator-(const const_iterator& o) const { return ptr - o.ptr; }

        friend const_iterator operator+(difference_type n, const const_iterator& it) {
            return const_iterator(it.ptr + n);
        }

        bool operator==(const const_iterator& o) const { return ptr == o.ptr; }
        bool operator!=(const const_iterator& o) const { return ptr != o.ptr; }
        bool operator< (const const_iterator& o) const { return ptr <  o.ptr; }
        bool operator<=(const const_iterator& o) const { return ptr <= o.ptr; }
        bool operator> (const const_iterator& o) const { return ptr >  o.ptr; }
        bool operator>=(const const_iterator& o) const { return ptr >= o.ptr; }

        const T* base() const { return ptr; }
    private:
        const T* ptr;
    };

    using reverse_iterator       = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

private:
    T*        data_;
    size_type size_;
    size_type cap_;

    // Perkelia elementus į naują buferį, dvigubindamas talpumą
    void reallocate(size_type newCap) {
        T* newData = static_cast<T*>(::operator new(newCap * sizeof(T)));
        for (size_type i = 0; i < size_; ++i) {
            new(newData + i) T(std::move(data_[i]));
            data_[i].~T();
        }
        ::operator delete(data_);
        data_ = newData;
        cap_  = newCap;
    }

    // Skaičiuoja naują talpumą
    size_type nextCap() const {
        return cap_ == 0 ? 1 : cap_ * 2;
    }

public:
    // ── Konstruktoriai ────────────────────────────────────────────────────────

    /// Numatytasis konstruktorius
    Vector() : data_(nullptr), size_(0), cap_(0) {}

    /// Konstruktorius su n nuliniais elementais
    explicit Vector(size_type n) : data_(nullptr), size_(0), cap_(0) {
        if (n == 0) return;
        data_ = static_cast<T*>(::operator new(n * sizeof(T)));
        cap_  = n;
        for (; size_ < n; ++size_)
            new(data_ + size_) T();
    }

    /// Konstruktorius su n vienodais elementais
    Vector(size_type n, const T& val) : data_(nullptr), size_(0), cap_(0) {
        if (n == 0) return;
        data_ = static_cast<T*>(::operator new(n * sizeof(T)));
        cap_  = n;
        for (; size_ < n; ++size_)
            new(data_ + size_) T(val);
    }

    /// Diapazono konstruktorius
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    Vector(InputIt first, InputIt last) : data_(nullptr), size_(0), cap_(0) {
        for (auto it = first; it != last; ++it)
            push_back(*it);
    }

    /// Inicializavimo sąrašo konstruktorius
    Vector(std::initializer_list<T> il) : data_(nullptr), size_(0), cap_(0) {
        reserve(il.size());
        for (const auto& v : il)
            new(data_ + size_++) T(v);
    }

    /// Kopijavimo konstruktorius
    Vector(const Vector& o) : data_(nullptr), size_(0), cap_(0) {
        if (o.size_ == 0) return;
        data_ = static_cast<T*>(::operator new(o.size_ * sizeof(T)));
        cap_  = o.size_;
        for (; size_ < o.size_; ++size_)
            new(data_ + size_) T(o.data_[size_]);
    }

    /// Perkėlimo konstruktorius
    Vector(Vector&& o) : data_(o.data_), size_(o.size_), cap_(o.cap_) {
        o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
    }

    /// Destruktorius
    ~Vector() {
        for (size_type i = 0; i < size_; ++i)
            data_[i].~T();
        ::operator delete(data_);
    }

    // ── Priskyrimo operatoriai ─────────────────────────────────────────────────

    /// Kopijavimo priskyrimas
    Vector& operator=(const Vector& o) {
        if (this != &o) { Vector tmp(o); swap(tmp); }
        return *this;
    }

    /// Perkėlimo priskyrimas
    Vector& operator=(Vector&& o) {
        if (this != &o) {
            for (size_type i = 0; i < size_; ++i) data_[i].~T();
            ::operator delete(data_);
            data_ = o.data_; size_ = o.size_; cap_ = o.cap_;
            o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
        }
        return *this;
    }

    /// Priskyrimas iš inicializavimo sąrašo
    Vector& operator=(std::initializer_list<T> il) {
        assign(il); return *this;
    }

    /// Užpildo n kopijomis val
    void assign(size_type n, const T& val) {
        clear();
        reserve(n);
        for (size_type i = 0; i < n; ++i)
            new(data_ + size_++) T(val);
    }

    /// Užpildo iš diapazono
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        clear();
        for (auto it = first; it != last; ++it)
            push_back(*it);
    }

    /// Užpildo iš inicializavimo sąrašo
    void assign(std::initializer_list<T> il) {
        clear();
        reserve(il.size());
        for (const auto& v : il)
            new(data_ + size_++) T(v);
    }

    // ── Prieiga prie elementų ─────────────────────────────────────────────────

    /// Prieiga su ribų tikrinimu
    reference at(size_type pos) {
        if (pos >= size_) throw std::out_of_range("Vector::at");
        return data_[pos];
    }

    /// Prieiga su ribų tikrinimu (const)
    const_reference at(size_type pos) const {
        if (pos >= size_) throw std::out_of_range("Vector::at");
        return data_[pos];
    }

    reference       operator[](size_type pos)       { return data_[pos]; }
    const_reference operator[](size_type pos) const { return data_[pos]; }

    reference       front()       { return data_[0]; }
    const_reference front() const { return data_[0]; }
    reference       back()        { return data_[size_ - 1]; }
    const_reference back()  const { return data_[size_ - 1]; }

    T*       data()       { return data_; }
    const T* data() const { return data_; }

    // ── Iteratoriai ───────────────────────────────────────────────────────────

    iterator       begin()        { return iterator(data_); }
    iterator       end()          { return iterator(data_ + size_); }
    const_iterator begin()  const { return const_iterator(data_); }
    const_iterator end()    const { return const_iterator(data_ + size_); }
    const_iterator cbegin() const { return const_iterator(data_); }
    const_iterator cend()   const { return const_iterator(data_ + size_); }

    reverse_iterator       rbegin()        { return reverse_iterator(end()); }
    reverse_iterator       rend()          { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin()  const { return const_reverse_iterator(end()); }
    const_reverse_iterator rend()    const { return const_reverse_iterator(begin()); }
    const_reverse_iterator crbegin() const { return const_reverse_iterator(cend()); }
    const_reverse_iterator crend()   const { return const_reverse_iterator(cbegin()); }

    // ── Talpumas ──────────────────────────────────────────────────────────────

    bool      empty()    const { return size_ == 0; }
    size_type size()     const { return size_; }
    size_type capacity() const { return cap_; }
    size_type max_size() const { return std::numeric_limits<size_type>::max() / sizeof(T); }

    /// Rezervuoja atmintį n elementams
    void reserve(size_type n) {
        if (n > cap_) reallocate(n);
    }

    /// Sumažina talpumą iki dydžio
    void shrink_to_fit() {
        if (size_ == cap_) return;
        if (size_ == 0) {
            ::operator delete(data_); data_ = nullptr; cap_ = 0; return;
        }
        reallocate(size_);
    }

    // ── Modifikatoriai ────────────────────────────────────────────────────────

    /// Išvalo visus elementus
    void clear() {
        for (size_type i = 0; i < size_; ++i)
            data_[i].~T();
        size_ = 0;
    }

    /// Prideda elementą kopijavimu
    void push_back(const T& val) {
        if (size_ == cap_) reallocate(nextCap());
        new(data_ + size_++) T(val);
    }

    /// Prideda elementą perkėlimu
    void push_back(T&& val) {
        if (size_ == cap_) reallocate(nextCap());
        new(data_ + size_++) T(std::move(val));
    }

    /// Sukonstruoja elementą vietoje gale
    template <typename... Args>
    reference emplace_back(Args&&... args) {
        if (size_ == cap_) reallocate(nextCap());
        new(data_ + size_) T(std::forward<Args>(args)...);
        return data_[size_++];
    }

    /// Pašalina paskutinį elementą
    void pop_back() { data_[--size_].~T(); }

    /// Keičia dydį
    void resize(size_type n) {
        if (n < size_) {
            for (size_type i = n; i < size_; ++i) data_[i].~T();
            size_ = n;
        } else if (n > size_) {
            if (n > cap_) reallocate(n);
            for (; size_ < n; ++size_) new(data_ + size_) T();
        }
    }

    /// Keičia dydį, naujus elementus užpildant val
    void resize(size_type n, const T& val) {
        if (n < size_) {
            for (size_type i = n; i < size_; ++i) data_[i].~T();
            size_ = n;
        } else if (n > size_) {
            if (n > cap_) reallocate(n);
            for (; size_ < n; ++size_) new(data_ + size_) T(val);
        }
    }

    /// Sukeičia turinį su kitu vektoriumi
    void swap(Vector& o) {
        std::swap(data_, o.data_);
        std::swap(size_, o.size_);
        std::swap(cap_,  o.cap_);
    }

    // ── Įterpimas ─────────────────────────────────────────────────────────────

    /// Įterpia kopiją prieš pos
    iterator insert(const_iterator pos, const T& val) {
        size_type idx = static_cast<size_type>(pos.base() - data_);
        if (size_ == cap_) reallocate(nextCap());
        if (idx < size_) {
            new(data_ + size_) T(std::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i)
                data_[i] = std::move(data_[i - 1]);
            data_[idx] = val;
        } else {
            new(data_ + idx) T(val);
        }
        ++size_;
        return iterator(data_ + idx);
    }

    /// Įterpia perkeliant prieš pos
    iterator insert(const_iterator pos, T&& val) {
        size_type idx = static_cast<size_type>(pos.base() - data_);
        if (size_ == cap_) reallocate(nextCap());
        if (idx < size_) {
            new(data_ + size_) T(std::move(data_[size_ - 1]));
            for (size_type i = size_ - 1; i > idx; --i)
                data_[i] = std::move(data_[i - 1]);
            data_[idx] = std::move(val);
        } else {
            new(data_ + idx) T(std::move(val));
        }
        ++size_;
        return iterator(data_ + idx);
    }

    /// Įterpia n kopijų val prieš pos
    iterator insert(const_iterator pos, size_type n, const T& val) {
        size_type idx = static_cast<size_type>(pos.base() - data_);
        if (n == 0) return iterator(data_ + idx);
        Vector tmp;
        tmp.data_ = static_cast<T*>(::operator new((size_ + n) * sizeof(T)));
        tmp.cap_  = size_ + n;
        for (size_type i = 0; i < idx; ++i)
            new(tmp.data_ + tmp.size_++) T(std::move(data_[i]));
        for (size_type i = 0; i < n; ++i)
            new(tmp.data_ + tmp.size_++) T(val);
        for (size_type i = idx; i < size_; ++i)
            new(tmp.data_ + tmp.size_++) T(std::move(data_[i]));
        swap(tmp);
        return iterator(data_ + idx);
    }

    /// Įterpia diapazoną prieš pos
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type idx = static_cast<size_type>(pos.base() - data_);
        Vector vals(first, last);
        size_type n = vals.size_;
        if (n == 0) return iterator(data_ + idx);
        Vector tmp;
        tmp.data_ = static_cast<T*>(::operator new((size_ + n) * sizeof(T)));
        tmp.cap_  = size_ + n;
        for (size_type i = 0; i < idx; ++i)
            new(tmp.data_ + tmp.size_++) T(std::move(data_[i]));
        for (size_type i = 0; i < n; ++i)
            new(tmp.data_ + tmp.size_++) T(std::move(vals.data_[i]));
        for (size_type i = idx; i < size_; ++i)
            new(tmp.data_ + tmp.size_++) T(std::move(data_[i]));
        swap(tmp);
        return iterator(data_ + idx);
    }

    /// Įterpia inicializavimo sąrašą prieš pos
    iterator insert(const_iterator pos, std::initializer_list<T> il) {
        return insert(pos, il.begin(), il.end());
    }

    /// Sukonstruoja elementą vietoje prieš pos
    template <typename... Args>
    iterator emplace(const_iterator pos, Args&&... args) {
        T val(std::forward<Args>(args)...);
        return insert(pos, std::move(val));
    }

    // ── Šalinimas ─────────────────────────────────────────────────────────────

    /// Pašalina elementą pos pozicijoje
    iterator erase(const_iterator pos) {
        size_type idx = static_cast<size_type>(pos.base() - data_);
        for (size_type i = idx + 1; i < size_; ++i)
            data_[i - 1] = std::move(data_[i]);
        data_[--size_].~T();
        return iterator(data_ + idx);
    }

    /// Pašalina elementus diapazone [first, last)
    iterator erase(const_iterator first, const_iterator last) {
        size_type f = static_cast<size_type>(first.base() - data_);
        size_type l = static_cast<size_type>(last.base()  - data_);
        size_type n = l - f;
        if (n == 0) return iterator(data_ + f);
        for (size_type i = l; i < size_; ++i)
            data_[i - n] = std::move(data_[i]);
        for (size_type i = size_ - n; i < size_; ++i)
            data_[i].~T();
        size_ -= n;
        return iterator(data_ + f);
    }
};

// ── Palyginimo operatoriai ─────────────────────────────────────────────────────

template <typename T>
bool operator==(const Vector<T>& a, const Vector<T>& b) {
    if (a.size() != b.size()) return false;
    for (typename Vector<T>::size_type i = 0; i < a.size(); ++i)
        if (!(a[i] == b[i])) return false;
    return true;
}

template <typename T>
bool operator!=(const Vector<T>& a, const Vector<T>& b) { return !(a == b); }

template <typename T>
bool operator<(const Vector<T>& a, const Vector<T>& b) {
    return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
}

template <typename T>
bool operator<=(const Vector<T>& a, const Vector<T>& b) { return !(b < a); }

template <typename T>
bool operator>(const Vector<T>& a, const Vector<T>& b) { return b < a; }

template <typename T>
bool operator>=(const Vector<T>& a, const Vector<T>& b) { return !(a < b); }

template <typename T>
void swap(Vector<T>& a, Vector<T>& b) { a.swap(b); }

#endif // VECTOR_H
