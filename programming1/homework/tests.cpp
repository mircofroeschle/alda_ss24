#include "graph.hpp"
#include "msf.hpp"

#include "testing.hpp"

#define run_test_all_ufs(func)                                                 \
  run_test(func<UnionFindNoPCNoRank>);                                         \
  run_test(func<UnionFindPCOnly>);                                             \
  run_test(func<UnionFindRankOnly>);                                           \
  run_test(func<UnionFindPCAndRank>);

template <typename UnionFind> bool test_union_find_small_hardcoded() {
  UnionFind uf(4);

  fail_unless_eq(uf.find(0), Node(0));
  fail_unless_eq(uf.find(1), Node(1));
  fail_unless_eq(uf.find(2), Node(2));
  fail_unless_eq(uf.find(3), Node(3));

  uf.combine(0, 1);
  fail_unless_eq(uf.find(0), uf.find(1));
  fail_unless(uf.find(0) <= Node(1));
  fail_unless_eq(uf.find(2), Node(2));
  fail_unless_eq(uf.find(3), Node(3));

  uf.combine(2, 3);
  fail_unless_eq(uf.find(0), uf.find(1));
  fail_unless_eq(uf.find(2), uf.find(3));
  fail_if_eq(uf.find(0), uf.find(2));

  uf.combine(1, 3);
  fail_unless_eq(uf.find(0), uf.find(1));
  fail_unless_eq(uf.find(2), uf.find(3));
  fail_unless_eq(uf.find(0), uf.find(3));

  return true;
}

template <typename UnionFind> bool test_find_combine() {
  UnionFind uf(5);

  uf.combine(0, 1);
  uf.combine(1, 2);

  fail_unless(uf.find(0) == uf.find(1));
  fail_unless(uf.find(1) == uf.find(2));


  if (UnionFind::path_compression) {
    // Pfadkompression genutzt
      uf.find(2); // This should compress the path for 2
      fail_unless(uf.get_parent(0) == uf.find(0));
      fail_unless(uf.get_parent(1) == uf.find(0));
      fail_unless(uf.get_parent(2) == uf.find(0));

  } else { 
    // Pfadkompression deaktiviert
    Node root0 = uf.find(0);
      Node parent1 = uf.get_parent(1);
      Node parent2 = uf.get_parent(2);

      fail_unless(parent1 == root0 || parent1 == 0);
      fail_unless(parent2 == parent1);
  }
  // Test num_groups
  fail_unless(uf.number_of_groups() == 3); 

  // Combine remaining elements
  uf.combine(3, 4);
  fail_unless(uf.number_of_groups() == 2);

  uf.combine(2, 3);
  fail_unless(uf.number_of_groups() == 1); 

  return true;
} 

int main() {
  run_test(test_gilbert_graph);

  run_test_all_ufs(test_union_find_small_hardcoded);
  run_test_all_ufs(test_find_combine);

  return 0;
}
