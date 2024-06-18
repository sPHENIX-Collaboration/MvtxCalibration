#ifndef MVTXFHR_MVTXHITMAP_H
#define MVTXFHR_MVTXHITMAP_H

#include "MvtxPixelDefs.h"

#include <ffarawobjects/MvtxRawHit.h>

#include <vector>
#include <climits>
#include <memory>
#include <utility>


class MvtxHitMap
{
    public:

        MvtxHitMap(){}
        ~MvtxHitMap(){ clear();}

        typedef std::pair<MvtxPixelDefs::pixelkey, uint32_t> pixel_hits_pair_t;
        typedef std::vector<MvtxHitMap::pixel_hits_pair_t> pixel_hit_vector_t;

        void add_hit(MvtxPixelDefs::pixelkey key, uint32_t nhits=1);
        void clear() { m_pixel_hit_vector.clear(); is_sorted = false; }

        int npixels() const { return m_pixel_hit_vector.size(); }

        uint32_t get_nhits(MvtxPixelDefs::pixelkey key) const;
        MvtxHitMap::pixel_hit_vector_t get_pixel_hit_vector() const { return m_pixel_hit_vector; }
        MvtxPixelDefs::pixelkey get_most_significant_pixel(); 

        uint32_t sum_hits(unsigned int nmasked = 0); 

    private:

        MvtxHitMap::pixel_hit_vector_t m_pixel_hit_vector {};

        void sort_by_hits();
        bool is_sorted{false};
       

};

#endif // MVTXHITMAP_H
