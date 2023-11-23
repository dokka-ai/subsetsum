#include <vector>
#include <iostream>

#include "subsetsum.hpp"

/*
g++ tool.cpp subsetsum.cpp -o tool -std=c++17 -Wall -Werror
*/

void printVec(const std::vector<long long>& v) {
  const long long n = static_cast<long long>(v.size());
  std::cout << "[ ";
  for (long long i = 0; i < n; ++i) {
    std::cout << v[i] << (i == n - 1 ? " ]" : ", ");
  }
}

long long main(long long argc, char** argv) {
  const long long target = std::stoi(argv[1]);
  std::vector<long long> nums;
  for (long long i = 2; i < argc; ++i) {
    nums.emplace_back(std::stoi(argv[i]));
  }

  std::cout << "target = " << target << std::endl;
  printVec(nums);
  std::cout << std::endl << "------" << std::endl;

  subsetsum::Solver solver(nums, target);
  solver.initSolutionIterator();

  std::vector<long long> solution = solver.getNextSolution();
  while (solution.size() > 0) {
    std::vector<long long> subset;
    long long sum = 0;
    for (long long index : solution) {
      subset.push_back(nums[index]);
      sum += nums[index];
    }

    printVec(subset);
    std::cout << " = " << sum << std::endl;

    solution = solver.getNextSolution();
  }

  return 0;
}
