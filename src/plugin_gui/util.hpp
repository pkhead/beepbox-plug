#pragma once
#include <cmath>
#include <atomic>
#include <imgui.h>

// b must be a positive number
inline float fwrapf(float a, float b) {
    float mod = fmodf(a, b);
    if (mod < 0.0) return b + mod;
    return mod;
}

template <typename T>
T clamp(T v, T min = 0, T max = 1) {
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

template <typename T>
T min(T a, T b) {
    return (a < b) ? a : b;
}

template <typename T>
T max(T a, T b) {
    return (a > b) ? a : b;
}

struct Hsv {
public:
    float h;
    float s;
    float v;

    inline constexpr Hsv() : h(0.f), s(0.f), v(0.f) {}
    inline constexpr Hsv(float h, float s, float v) : h(h), s(s), v(v) {}
}; // struct Hsv

struct Color {
public:
    float r;
    float g;
    float b;

    inline constexpr Color() : r(0.f), g(0.f), b(0.f) {}
    inline constexpr Color(float r, float g = 0.f, float b = 0.f) noexcept : r(r), g(g), b(b) {}

    Hsv toHsv() const;
    static Color fromHsv(const Hsv &hsv);

    inline constexpr ImVec4 toImVec4(float a) {
        return ImVec4(r, g, b, a);
    }

    inline constexpr operator ImVec4() {
        return ImVec4(r, g, b, 1.0f);
    }
}; // struct Color

inline constexpr Color operator*(const Color &col, float mul) {
    return Color(col.r * mul, col.g * mul, col.b * mul);
}

inline constexpr Color operator/(const Color &col, float mul) {
    return Color(col.r / mul, col.g / mul, col.b / mul);
}

template <typename T, size_t SIZE>
struct EventQueue {
private:
    T *data;
    std::atomic_size_t write_ptr;
    std::atomic_size_t read_ptr;

    static constexpr size_t MASK = SIZE - 1;

public:
    static_assert(SIZE != 0 && (SIZE & (SIZE - 1)) == 0, "EventQueue size is not a power of two!");
    
    EventQueue() noexcept : data(new T[SIZE]) {
        write_ptr.store(0);
        read_ptr.store(0);
    }
    
    EventQueue(EventQueue&) = delete;
    EventQueue operator=(EventQueue&) = delete;
    ~EventQueue() noexcept { delete[] data; }

    void enqueue(const T &item) {
        uint32_t ptr = write_ptr.load();
        data[ptr] = item;
        ptr = (ptr + 1) & MASK;
        write_ptr.store(ptr);
    }

    bool dequeue(T &item) {
        size_t read = read_ptr.load();
        size_t write = write_ptr.load();
        if (read == write) return false;

        item = data[read];
        read = (read + 1) & MASK;
        read_ptr.store(read);

        return true;
    }

    size_t items_queued() const {
        size_t read = read_ptr.load();
        size_t write = write_ptr.load();

        if (write >= read) {
            return write - read;
        } else {
            return SIZE - read + write;
        }
    }
}; // struct EventQueue