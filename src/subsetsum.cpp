#include <utility>
#include <algorithm>
#include <cstdlib> 
#include <numeric>
#include "subsetsum.hpp"

namespace subsetsum {
    typedef Solver::Ints Ints;
    typedef Solver::Cell Cell;
    #define BYTES   sizeof(Cell)
    #define BITS    (BYTES*8)
    #define ONE     Cell(1)
    #define GET_BIT(p,k)    (p[(k)/BITS]>>((k)%BITS)) & 1
    #define SET_ONE(p,k)    p[(k)/BITS] |= (ONE<<((k)%BITS))

    static int gcd(const Ints& nums, int target)
    {
        int ret = std::abs ( target );
        for ( const auto& num: nums ) {
            auto cur = std::abs(num);
            if ( ret < cur )
                std::swap ( ret, cur );
            while ( cur ) {
                const auto tmp  = ret % cur;
                ret             = cur;
                cur             = tmp;
            }
        }
        return ret;
    }

    Ints mutatingArgsort ( Ints& a ) {
        const int n = static_cast<int>(a.size());
        std::vector<std::pair<int, int>> a_indexed(n);
        Ints indices(n);

        for (int i = 0; i < n; ++i) {
            a_indexed[i] = { a[i], i };
        }
        std::stable_sort(a_indexed.begin(), a_indexed.end(),
            [](const std::pair<int, int>& x, const std::pair<int, int>& y) {
                return x.first < y.first;
            });
        for (int i = 0; i < n; ++i) {
            a[i] = a_indexed[i].first;
            indices[i] = a_indexed[i].second;
        }

        return indices;
    }

    Solver::Solver (const Ints& nums, const int target)
        : nums_     ( nums )
        , _GCD      ( gcd ( nums, target ) )
        , target_   ( std::abs(target / _GCD) )
    {
        nrows_ = static_cast<int> ( nums_.size() );
        if ( nrows_ == 0 ) {
            has_sol_ = false;
            return;
        }

        if ( target < 0) 
            for ( auto& num : nums_ )
                num *= -1;

        if (_GCD > 1) 
            for ( auto& num : nums_ )
                num /= _GCD;

        remapping_ = mutatingArgsort(nums_);

        int negative_sum = 0 ;
        int positive_sum = 0;

        for ( const auto& num : nums_ )
            *(num < 0 ? &negative_sum : &positive_sum) += num;

        if (target_ < negative_sum or target_ > positive_sum) {
            has_sol_ = false;
            return;
        }

        a_      = negative_sum;
        ncols_  = target_ - a_ + 1;
        const auto last = std::find_if(nums_.rbegin(), nums_.rend(), [this] (int i) { return i < this->ncols_; });
        nrows_  = std::distance(last, nums_.rend());
        _Over   = (ncols_ + BITS) / BITS;
        _pAll   = new Cell [nrows_ * _Over];

        _ppRows = new Cell* [nrows_];
        for (int i = 0; i < nrows_; i++)
            _ppRows[i] = _pAll + i * _Over;

        fillDPTable();
        has_sol_ = GET_BIT(_ppRows[nrows_ - 1], ncols_ - 1);
    }

    Solver::~Solver() {
        delete _pAll;
        delete _ppRows; 
    }

    void Solver::fillDPTable() {
        // Fill the first row of "dp_"
        std::fill(_ppRows[0], _ppRows[1], 0);
        SET_ONE(_ppRows[0], nums_[0] - a_);
 
        // Fill the remaining rows of "dp_"
        for (int i = 1; i < nrows_; ++i) {
            auto* const       pCur = _ppRows[i];
            const auto* const pPre = _ppRows[i - 1];
            std::memcpy(pCur, pPre, _Over * BYTES);
            for (int j = 0; j < ncols_; ++j) {
                if (nums_[i] == a_ + j)
                    SET_ONE(pCur, j);
                else {
                    const int next_col = j - nums_[i] ;
                    if (0 <= next_col and next_col < ncols_ and GET_BIT(pPre,next_col) )
                        SET_ONE(pCur, j);
                }
            }
        }
    }

    Solver::Ints Solver::genSolution(const QueueItem& item) const {
        const int   n   = static_cast<int>(item.take.size());
        Ints        sol (n);
        for ( int i = 0; i < n; ++i ) {
            sol [i] = remapping_.at(item.take.at(i));
        }
        std::sort(sol.begin(), sol.end());
        return sol;
    }

    void Solver::initSolutionIterator() {
        if (has_sol_) {
            const Ints  take = { nrows_ - 1 };
            const int   togo = target_ - nums_[nrows_ - 1];
            queue_ = { QueueItem(nrows_ - 1, ncols_ - 1, take, togo) };
        }
    }

    Solver::Ints Solver::getNextSolution() {
        if (!has_sol_) 
            return {};

        while ( queue_.size() ) {
            const QueueItem item(queue_.back());
            queue_.pop_back();

            const int row = item.row;
            if ( row ) {
                const int col = item.col;
                const auto* const pPre = _ppRows[row - 1];

                if ( GET_BIT(pPre,col) ) {
                    Ints take = item.take;
                    take[take.size() - 1] = row - 1;
                    const int togo = item.togo + nums_[row] - nums_[row - 1];
                    queue_.emplace_back(row - 1, col, take, togo);
                }

                const int next_col = col - nums_[row];
                if ( 0 <= next_col and next_col < ncols_) {
                    if (GET_BIT(pPre,next_col)) {
                        Ints      take = item.take;
                        take.emplace_back(row - 1);
                        const int togo = item.togo - nums_[row - 1];
                        queue_.emplace_back(row - 1, next_col, take, togo);
                    }
                }
            }

            if ( not item.togo ) 
                return genSolution(item);
        }

        return {};
    }

}  // end namespace subsetsum
