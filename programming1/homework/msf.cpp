#include "msf.hpp"
#include "graph.hpp"
#include <fstream>
#include <random>

int main() {
  constexpr Node min_n = 1 << 5;
  constexpr Node max_n = 1 << 21;
  constexpr uint64_t repeats = 5;

  // Öffne eine Datei zum Schreiben der Ergebnisse
  std::ofstream output;
  output.open("kruskal.csv");
  // Schreibe die Kopfzeile für die CSV-Datei
  output << "n,m,avg_deg,unionfind,accesses\n"; 

  const double avg_deg = 5; // Durchschnittlicher Grad der Knoten
  std::mt19937_64 gen(123456); // Zufallsgenerator mit festem Seed für Reproduzierbarkeit

  for (uint64_t rep = 0; rep < repeats; ++rep) {  // Wiederhole die Messungen mehrmals
    for (Node n = min_n; n <= max_n; n *= 2) {    // Verdopple die Anzahl der Knoten in jeder Iteration
      // Generiere einen zufälligen Graphen mit n Knoten und durchschnittlichem Grad avg_deg
      auto edges = generate_gilbert_graph(gen, n, avg_deg); 
      const auto m = edges.size(); // Anzahl der Kanten im Graphen

      // Führe Kruskal's Algorithmus nur für kleinere Graphen aus (siehe Aufgabe)
      if (n < max_n / 100) {
        auto res = kruskal<UnionFindNoPCNoRank>(edges); // Führe Kruskal's Algorithmus mit der naiven Union-Find-Variante aus
        output << n << ',' << m << ',' << avg_deg << ",naive,"
               << res.parent_accesses << std::endl;         // Schreibe die Ergebnisse in die CSV-Datei
      }

      // UnionFind mit Pfadkompression
      {
        auto res = kruskal<UnionFindPCOnly>(edges);
        output << n << ',' << m << ',' << avg_deg << ",pc,"
               << res.parent_accesses << std::endl;
      }

      // UnionFind mit Union-by-Rank
      {
        auto res = kruskal<UnionFindRankOnly>(edges);
        output << n << ',' << m << ',' << avg_deg << ",rank,"
               << res.parent_accesses << std::endl;
      }

      // UnionFind mit Pfadkompression und Union-by-Rank
      {
        auto res = kruskal<UnionFindPCAndRank>(edges);
        output << n << ',' << m << ',' << avg_deg << ",pc+rank,"
               << res.parent_accesses << std::endl;
      }
    }
  }

  return 0;
}
