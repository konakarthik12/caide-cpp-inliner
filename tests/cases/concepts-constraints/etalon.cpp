#include <concepts>


// Basic concept usage
template <typename T>
concept Add = requires(T a, T b) {
    { a + b };
};

template <typename T>
    requires Add<T>
T add(T a, T b) {
    return a + b;
}

template <typename T>
concept AlwaysTrue = true;

template <typename T>
    requires AlwaysTrue<T>
void f(T) {}

// Concept used ONLY in requires requires clause (key test case)
template <typename T>
concept HasIter = requires(T& t) {
    t.begin();
    t.end();
};

// Unused concept - should be removed


// Concept used in requires requires with multiple concepts
template <typename T>
concept HasSize = requires(T& t) {
    t.size();
};

template <typename T>
class Container {
public:
    // Concept used ONLY in requires requires clause
    template <std::integral First>
    requires requires(T& container) {
        { HasIter<T> };
    }
    explicit Container(First first) {
        (void)first;
    }

    // Multiple concepts in requires requires
    template <std::integral First>
    requires requires(T& container) {
        { HasSize<T> };
        { HasIter<T> };
    }
    explicit Container(First first, int) {
        (void)first;
    }

    // Concept used in requires requires in method
    template <typename U>
    requires requires(U& u) {
        { HasIter<U> };
    }
    void process(U& item) {
        (void)item;
    }

    // Concept used in both requires requires AND if constexpr
    template <typename U>
    requires requires(U& u) {
        { HasIter<U> };
    }
    void processWithIfConstexpr(U& item) {
        if constexpr (HasIter<U>) {
            item.begin();
        }
    }
};

// Concept used in requires requires with nested requires
template <typename T>
concept NestedConcept = requires(T& t) {
    requires HasIter<T>;
    t.something();
};

template <typename T>
class NestedContainer {
public:
    template <std::integral First>
    requires requires(T& container) {
        { NestedConcept<T> };
    }
    explicit NestedContainer(First first) {
        (void)first;
    }
};

struct WithIter {
    int* begin() { return nullptr; }
    int* end() { return nullptr; }
    
};


int main() {
    // Basic concept usage
    add(1, 2);
    f(1);
    
    // Concept used in requires requires
    Container<WithIter> c1(5);
    Container<WithIter> c2(10, 0);
    
    // Concept used in method requires requires
    WithIter w;
    Container<int> c3(5);
    c3.process(w);
    c3.processWithIfConstexpr(w);
    
    // Nested concept
    NestedContainer<WithIter> nc(5);
    
    return 0;
}
