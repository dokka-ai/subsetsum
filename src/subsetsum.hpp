#pragma once

#include <vector>

namespace subsetsum {

class Solver {
 public:
  Solver(const std::vector<long long>& nums, long long target);

  virtual ~Solver();

  inline bool hasSolution() const {
    return has_sol_;
  }

  void initSolutionIterator();

  std::vector<long long> getNextSolution();

 private:
  class QueueItem {
   public:
    QueueItem(long long r, long long c, const std::vector<long long>& i, long long t)
        : row(r), col(c), take(i), togo(t) {}
    long long row, col;
    std::vector<long long> take;
    long long togo;
  };

  void flipSign();

  void fillDPTable();

  std::vector<long long> genSolution(const QueueItem& item) const;

  long long n_;
  std::vector<long long> nums_;
  std::vector<long long> remapping_;
  long long target_;
  bool has_sol_;
  long long a_, b_;
  long long nrows_, ncols_;
  uint8_t* dp_;
  std::vector<QueueItem> queue_;
};

}  // end namespace subsetsum
