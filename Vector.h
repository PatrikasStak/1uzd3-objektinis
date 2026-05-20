#ifndef VECTOR_H
#define VECTOR_H

#include <algorithm>
#include <cstddef>
#include <initializer_list>
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
    using value_type     = T;
    using size_type      = std::size_t;
    using iterator       = T*;
    using const_iterator = const T*;

private:
    T*        data_;
    size_type size_;
    size_type cap_;

    // Perkelia elementus į naują buferį, dvigubindamas talpumą
    void reallocate(size_type newCap) {
        T* newData = new T[newCap];
        for (size_type i = 0; i < size_; ++i)
            newData[i] = std::move(data_[i]);
        delete[] data_;
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

    /// Konstruktorius su n elementų
    explicit Vector(size_type n, const T& val = T()) : data_(nullptr), size_(n), cap_(n) {
        if (n == 0) return;
        data_ = new T[n];
        for (size_type i = 0; i < n; ++i)
            data_[i] = val;
    }

    /// Inicializavimo sąrašo konstruktorius
    Vector(std::initializer_list<T> il) : data_(new T[il.size()]), size_(il.size()), cap_(il.size()) {
        size_type i = 0;
        for (const auto& v : il)
            data_[i++] = v;
    }

    /// Diapazono konstruktorius
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    Vector(InputIt first, InputIt last) : data_(nullptr), size_(0), cap_(0) {
        for (auto it = first; it != last; ++it)
            push_back(*it);
    }

    /// Kopijavimo konstruktorius
    Vector(const Vector& o) : data_(new T[o.cap_]), size_(o.size_), cap_(o.cap_) {
        for (size_type i = 0; i < size_; ++i)
            data_[i] = o.data_[i];
    }

    /// Perkėlimo konstruktorius
    Vector(Vector&& o) : data_(o.data_), size_(o.size_), cap_(o.cap_) {
        o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
    }

    /// Destruktorius
    ~Vector() { delete[] data_; }

    // ── Priskyrimo operatoriai ─────────────────────────────────────────────────

    /// Kopijavimo priskyrimas
    Vector& operator=(const Vector& o) {
        if (this != &o) { Vector tmp(o); swap(tmp); }
        return *this;
    }

    /// Perkėlimo priskyrimas
    Vector& operator=(Vector&& o) {
        if (this != &o) {
            delete[] data_;
            data_ = o.data_; size_ = o.size_; cap_ = o.cap_;
            o.data_ = nullptr; o.size_ = 0; o.cap_ = 0;
        }
        return *this;
    }

    /// Priskyrimas iš inicializavimo sąrašo
    Vector& operator=(std::initializer_list<T> il) {
        delete[] data_;
        data_ = new T[il.size()];
        cap_  = il.size();
        size_ = 0;
        for (const auto& v : il)
            data_[size_++] = v;
        return *this;
    }

    // ── Prieiga prie elementų ─────────────────────────────────────────────────

    T&       operator[](size_type i)       { return data_[i]; }
    const T& operator[](size_type i) const { return data_[i]; }

    T& at(size_type i) {
        if (i >= size_) throw std::out_of_range("Vector::at");
        return data_[i];
    }
    const T& at(size_type i) const {
        if (i >= size_) throw std::out_of_range("Vector::at");
        return data_[i];
    }

    T&       front()       { return data_[0]; }
    const T& front() const { return data_[0]; }
    T&       back()        { return data_[size_ - 1]; }
    const T& back()  const { return data_[size_ - 1]; }

    T*       data()       { return data_; }
    const T* data() const { return data_; }

    // ── Iteratoriai ───────────────────────────────────────────────────────────

    iterator       begin()        { return data_; }
    iterator       end()          { return data_ + size_; }
    const_iterator begin()  const { return data_; }
    const_iterator end()    const { return data_ + size_; }
    const_iterator cbegin() const { return data_; }
    const_iterator cend()   const { return data_ + size_; }

    // ── Talpumas ──────────────────────────────────────────────────────────────

    bool      empty()    const { return size_ == 0; }
    size_type size()     const { return size_; }
    size_type capacity() const { return cap_; }

    /// Rezervuoja atmintį n elementams
    void reserve(size_type n) {
        if (n > cap_) reallocate(n);
    }

    // ── Modifikatoriai ────────────────────────────────────────────────────────

    /// Išvalo visus elementus
    void clear() { size_ = 0; }

    /// Prideda elementą kopijavimu
    void push_back(const T& val) {
        if (size_ == cap_) reallocate(nextCap());
        data_[size_++] = val;
    }

    /// Prideda elementą perkėlimu
    void push_back(T&& val) {
        if (size_ == cap_) reallocate(nextCap());
        data_[size_++] = std::move(val);
    }

    /// Pašalina paskutinį elementą
    void pop_back() { if (size_ > 0) --size_; }

    /// Keičia dydį
    void resize(size_type n, const T& val = T()) {
        if (n > cap_) reallocate(n);
        for (size_type i = size_; i < n; ++i)
            data_[i] = val;
        size_ = n;
    }

    /// Sukeičia turinį su kitu vektoriumi
    void swap(Vector& o) {
        std::swap(data_, o.data_);
        std::swap(size_, o.size_);
        std::swap(cap_,  o.cap_);
    }

    // ── Įterpimas ─────────────────────────────────────────────────────────────

    /// Įterpia elementą prieš pos
    iterator insert(const_iterator pos, const T& val) {
        size_type idx = pos - data_;
        if (size_ == cap_) reallocate(nextCap());
        for (size_type i = size_; i > idx; --i)
            data_[i] = std::move(data_[i - 1]);
        data_[idx] = val;
        ++size_;
        return data_ + idx;
    }

    /// Įterpia elementą perkėlimu prieš pos
    iterator insert(const_iterator pos, T&& val) {
        size_type idx = pos - data_;
        if (size_ == cap_) reallocate(nextCap());
        for (size_type i = size_; i > idx; --i)
            data_[i] = std::move(data_[i - 1]);
        data_[idx] = std::move(val);
        ++size_;
        return data_ + idx;
    }

    /// Įterpia diapazoną prieš pos
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    iterator insert(const_iterator pos, InputIt first, InputIt last) {
        size_type idx = pos - data_;
        Vector tmp(first, last);
        size_type n = tmp.size_;
        if (n == 0) return data_ + idx;
        if (size_ + n > cap_) reallocate(size_ + n);
        for (size_type i = size_ + n - 1; i >= idx + n; --i)
            data_[i] = std::move(data_[i - n]);
        for (size_type i = 0; i < n; ++i)
            data_[idx + i] = std::move(tmp.data_[i]);
        size_ += n;
        return data_ + idx;
    }

    /// Užpildo iš diapazono
    template <typename InputIt,
              typename = typename std::enable_if<!std::is_integral<InputIt>::value>::type>
    void assign(InputIt first, InputIt last) {
        size_ = 0;
        for (auto it = first; it != last; ++it)
            push_back(*it);
    }

    // ── Šalinimas ─────────────────────────────────────────────────────────────

    /// Pašalina elementą pos pozicijoje
    iterator erase(const_iterator pos) {
        size_type idx = pos - data_;
        for (size_type i = idx + 1; i < size_; ++i)
            data_[i - 1] = std::move(data_[i]);
        --size_;
        return data_ + idx;
    }

    /// Pašalina elementus diapazone [first, last)
    iterator erase(const_iterator first, const_iterator last) {
        size_type f = first - data_;
        size_type n = last - first;
        if (n == 0) return data_ + f;
        for (size_type i = f + n; i < size_; ++i)
            data_[i - n] = std::move(data_[i]);
        size_ -= n;
        return data_ + f;
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
