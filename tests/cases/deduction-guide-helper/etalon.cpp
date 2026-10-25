template <typename T>
constexpr int offset_v = sizeof(T) > 4 ? 1 : 0;

template <typename T, int offset>
struct Box {
    T value;
    explicit Box(T v) : value(v) {}
};

template <typename T>
Box(T) -> Box<T, offset_v<T>>;

int main() {
    Box b(42);
    return b.value;
}
