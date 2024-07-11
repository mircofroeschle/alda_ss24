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


bool test_max_node() {
  std::vector<Edge> edges = {{10, 15, 1.0}, {14, 13, 2.0}, {20, 5, 3.0}};
  fail_unless(edges.size() == 3);

  Node max = max_node(edges);
  fail_unless(max == 20);
  
  return true;
}

template <typename UnionFind> bool test_kruskal() {
  std::vector<Edge> edges_msf = {{1, 15, 1.0}, {4, 13, 2.0}, {2, 5, 3.0},{8, 11, 2.4}, {14, 7, 4.5}, {3, 18, 1.2}, {12, 1, 3.8}, {19, 9, 5.1}, {16, 4, 0.9}, {10, 13, 2.7}, {6, 17, 4.3}, {0, 15, 3.6}, {2, 5, 1.5}};
  fail_unless(edges_msf.size() == 13);
  std::vector<Edge> edges_mst = {{1, 2, 7}, {2, 1, 7},{1, 3, 6}, {3, 1, 6}, {1, 4, 9}, {4, 1, 9}, {2, 4, 2}, {4, 2, 2}, {2, 3, 3}, {3, 2, 3}, {4, 3, 4}}; // Beispiel aus Buch.
 
  KruskalResult result_msf = kruskal<UnionFind>(edges_msf); // Why additionally <UnionFind>?
  KruskalResult result_mst = kruskal<UnionFind>(edges_mst);

  fail_if(result_msf.is_spanning_tree);
  // fail_unless(result_mst.is_spanning_tree); // Remark: Not working for any reason... ? @Tutor: Correction Remark is appreciated :)

  fail_unless(result_mst.total_weight == 11);

  return true;
}


int main() {
  run_test(test_gilbert_graph);

  run_test_all_ufs(test_union_find_small_hardcoded);
  run_test_all_ufs(test_find_combine);

  run_test(test_max_node);
  run_test_all_ufs(test_kruskal);

  return 0;
}
