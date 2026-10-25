template <typename T, int offset>
struct Box {
    T value;
    explicit Box(T v) : value(v) {}
};

template <typename T>
Box(T) -> Box<T, 0>;

int main() {
    Box b(42);
    return b.value;
}
