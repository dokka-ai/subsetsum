#pragma once

#include <vector>

namespace subsetsum {

    class Solver {
    public:
        typedef long long Cell;
        typedef std::vector<Cell> Ints;
        Solver  ( const Ints& nums, const Cell target );

        virtual ~Solver();

        void initSolutionIterator   ( );
        Ints getNextSolution        ( );

        bool    hasSolution ( ) const { return has_sol_; }
        Cell    sizeMemory  ( ) const { return nrows_ * (_Over*sizeof(Cell) + sizeof(void*)); }
        Cell    getGCD      ( ) const { return _GCD; }

    private:
        class QueueItem {
        public:
            QueueItem ( const Cell r, const Cell c, const Ints& i, const Cell t )
                : row(r), col(c), take(i), togo(t) {}
            Cell    row, col;
            Ints    take;
            Cell    togo;
        };

        void fillDPTable    ( ) ;
        Ints genSolution    ( const QueueItem& item ) const;

        const   Cell _GCD;
        const   Cell target_;

        Ints    nums_;
        Ints    remapping_;
        bool    has_sol_    = true;
        Cell    a_;
        Cell    nrows_, ncols_;

        Cell*   _pAll;
        Cell**  _ppRows;
        Cell    _Over;

        std::vector<QueueItem> queue_;

    };

}  // end namespace subsetsum
