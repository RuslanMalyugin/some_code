#include <atomic>

int main() {
  std::atomic<int> a;
  a.store(1);
  return 0;
}