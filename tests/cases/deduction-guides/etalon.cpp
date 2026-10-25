template<typename T>
struct Container {
    T value;
    Container(T v) : value(v) {}
};

template<typename T>
struct Pair {
    T first;
    T second;
    Pair(T a, T b) : first(a), second(b) {}
};

template<typename T, typename U>
struct Tuple {
    T first;
    U second;
    Tuple(T a, U b) : first(a), second(b) {}
};

template<typename T> Container(T) -> Container<T>;
template<typename T> Pair(T, T) -> Pair<T>;
template<typename T, typename U> Tuple(T, U) -> Tuple<T, U>;

int main() {
    Container c(42);
    Pair p(1, 2);
    Tuple t(3, 4.0);
    
    return 0;
}

