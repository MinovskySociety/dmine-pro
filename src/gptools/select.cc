#include "select.h"

#include <random>

namespace gptools {

int ReadIDList(const std::filesystem::path& list_file,
                     std::set<ID>& id_list) {
  std::cout << list_file << std::endl;

  std::ifstream fin(list_file, std::ios::in);
  if (!fin.is_open()) return -2;

  id_list.clear();
  while (!fin.eof()) {
    ID id;
    fin >> id;
    id_list.emplace(id);
  }

  std::cout << "ID count - " << id_list.size() << std::endl;
  return static_cast<int>(id_list.size());
}

int SamplingVertexByIDList(Graph& g, const Label& label,
                            const std::set<ID>& id_list) {
  std::set<ID> remove_list;
  for (auto it_v = g.VertexBegin(label); !it_v.IsDone(); ++it_v) {
    if (id_list.find(it_v->id()) == id_list.end()) {
      remove_list.emplace(it_v->id());
    }
  }

  std::cout << "Remove vertices:" << std::endl;
  auto count = RemoveVertexByIDList(g, remove_list);

  return static_cast<int>(count);
}

int SamplingVertexRandom(Graph& g, const Label& label, size_t num) {
  std::set<ID> remove_list;

  size_t rn = g.CountVertex(label);
  size_t rm = num;
  std::random_device rd;
  std::mt19937_64 gen(rd());
  for (auto it_v = g.VertexBegin(label); !it_v.IsDone(); ++it_v) {
    std::uniform_int_distribution<size_t> dis(0, rn - 1);
    if (rm >= rn || dis(gen) < rm) {
      --rm;
    } else {
      remove_list.emplace(it_v->id());
    }
    --rn;
  }

  std::cout << "Remove vertices:" << std::endl;
  auto count = RemoveVertexByIDList(g, remove_list);

  return static_cast<int>(count);
}

}  // namespace gptools