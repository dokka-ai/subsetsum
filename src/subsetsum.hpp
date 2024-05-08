#pragma once

#include <vector>

namespace subsetsum {

    class Solver {
    public:
        typedef std::vector<int> Ints;
        typedef int Cell;
        Solver  ( const Ints& nums, const int target );

        virtual ~Solver();

        void initSolutionIterator   ( );
        Ints getNextSolution        ( );

        bool    hasSolution ( ) const { return has_sol_; }
        int     sizeMemory  ( ) const { return nrows_ * (_Over*sizeof(Cell) + sizeof(void*)); }
        int     getGCD      ( ) const { return _GCD; }

    private:
        class QueueItem {
        public:
            QueueItem ( const int r, const int c, const Ints& i, const int t )
                : row(r), col(c), take(i), togo(t) {}
            int     row, col;
            Ints    take;
            int     togo;
        };

        void fillDPTable    ( ) ;
        Ints genSolution    ( const QueueItem& item ) const;

        const   int _GCD;
        const   int target_;

        Ints    nums_;
        Ints    remapping_;
        bool    has_sol_    = true;
        int     a_;
        int     nrows_, ncols_;

        Cell*   _pAll;
        Cell**  _ppRows;
        int     _Over;

        std::vector<QueueItem> queue_;

    };

}  // end namespace subsetsum
