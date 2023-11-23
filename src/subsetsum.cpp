#include <utility>
#include <algorithm>
#include <iostream>
#include <time.h>
#include "subsetsum.hpp"

namespace subsetsum {

std::vector<long long> mutatingArgsort(std::vector<long long>& a) {
  const long long n = static_cast<long long>(a.size());
  std::vector<std::pair<long long, long long>> a_indexed(n);
  std::vector<long long> indices(n);

  for (long long i = 0; i < n; ++i) {
    a_indexed[i] = {a[i], i};
  }
  std::stable_sort(a_indexed.begin(), a_indexed.end(),
    [](const std::pair<long long, long long>& x, const std::pair<long long, long long>& y) {
      return x.first < y.first;
  });
  for (long long i = 0; i < n; ++i) {
    a[i] = a_indexed[i].first;
    indices[i] = a_indexed[i].second;
  }

  return indices;
}

Solver::Solver(const std::vector<long long>& nums, long long target)
    : nums_(nums), target_(target), has_sol_(true), dp_(nullptr) {
  n_ = static_cast<long long>(nums_.size());
  if (n_ == 0) {
    has_sol_ = false;
    return;
  }

  if (target_ < 0) {
    flipSign();
  }

  remapping_ = mutatingArgsort(nums_);

  long long negative_sum = 0;
  long long positive_sum = 0;

  for (long long i = 0; i < n_; ++i) {
    if (nums_[i] < 0) {
      negative_sum += nums_[i];
    } else {
      positive_sum += nums_[i];
    }
  }

  if (target_ < negative_sum || target_ > positive_sum) {
    has_sol_ = false;
    return;
  }

  a_ = negative_sum;
  b_ = target_;
  nrows_ = n_;
  ncols_ = b_ - a_ + 1;
  dp_ = new uint8_t[nrows_ * ncols_];
  fillDPTable();
  if (!dp_[nrows_ * ncols_ - 1]) {
    has_sol_ = false;
  }
}

Solver::~Solver() {
  if (dp_ != nullptr) {
    delete[] dp_;
  }
}

void Solver::flipSign() {
  target_ *= -1;
  for (long long i = 0; i < n_; ++i) {
    nums_[i] *= -1;
  }
}

void Solver::fillDPTable() {
  // Fill the first row of "dp_"
  for (long long j = 0; j < ncols_; ++j) {
    dp_[j] = nums_[0] == (a_ + j);
  }
  // Fill the remaining rows of "dp_"
  for (long long i = 1; i < nrows_; ++i) {
    for (long long j = 0; j < ncols_; ++j) {
      const long long cur = i * ncols_ + j;
      const long long prev = (i - 1) * ncols_ + j;
      const long long s = a_ + j;

      dp_[cur] = dp_[prev] || nums_[i] == s;
      if (!dp_[cur]) {
        const long long next_col = s - nums_[i] - a_;
        if (0 <= next_col && next_col < ncols_) {
          dp_[cur] = dp_[prev - j + next_col];
        }
      }
    }
  }
}

std::vector<long long> Solver::genSolution(const QueueItem& item) const {
  const long long n = static_cast<long long>(item.take.size());
  std::vector<long long> sol(n);
  for (long long i = 0; i < n; ++i) {
    sol[i] = remapping_.at(item.take.at(i));
  }
  std::sort(sol.begin(), sol.end());
  return sol;
}

void Solver::initSolutionIterator() {
  if (has_sol_) {
    const std::vector<long long> take = {nrows_ - 1};
    const long long togo = target_ - nums_[nrows_ - 1];
    queue_ = {QueueItem(nrows_ - 1, ncols_ - 1, take, togo)};
  }
}

std::vector<long long> Solver::getNextSolution() {
  if (!has_sol_) {
    return {};
  }

  while (queue_.size() > 0) {
    const QueueItem item(queue_.back());
    queue_.pop_back();

    const long long row = item.row;
    const long long col = item.col;

    if (row > 0 && dp_[(row - 1) * ncols_ + col]) {
      std::vector<long long> take = item.take;
      take[take.size() - 1] = row - 1;
      const long long togo = item.togo + nums_[row] - nums_[row - 1];
      queue_.emplace_back(row - 1, col, take, togo);
    }

    const long long next_col = col - nums_[row];
    if (row > 0 && 0 <= next_col && next_col < ncols_) {
      if (dp_[(row - 1) * ncols_ + next_col]) {
        std::vector<long long> take = item.take;
        take.emplace_back(row - 1);
        const long long togo = item.togo - nums_[row - 1];
        queue_.emplace_back(row - 1, next_col, take, togo);
      }
    }

    if (item.togo == 0) {
      return genSolution(item);
    }
  }

  return {};
}

}  // end namespace subsetsum
