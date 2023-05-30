#include "bulk.h"

int main() {
    size_t N = 3;
    std::cin >> N;

    Bulk bulk(N);
    ToPrint print(bulk);
    ToFile file(bulk);
    bulk.result();

}
