#ifndef LAYER_TAG_MASK_HDR
#define LAYER_TAG_MASK_HDR

#include "layer_tag_collection.hpp"

class LayerTagMask {

public:

    bool isNone();

    bool isSome();

    bool isAny();

    /**
     * method for compatibility with boost::bimap
     */
    bool operator<(LayerTagMask other) const;

    /**
     * created union of two layertaglists
     */
    friend LayerTagMask createUnion(
        LayerTagMask mask1,
        LayerTagMask mask2
    );

    /**
     * created intersection of two layertaglists
     */
    friend LayerTagMask createIntersection(
        LayerTagMask mask1,
        LayerTagMask mask2
    );

    friend class LayerTagManager;

private:
    /**
     * private constructor! only LayerTagManager can be used to create layer tag masks
     */
    LayerTagMask(LayerTagCollection tags) : any_(false), none_(false), tags_(tags) { }

    /**
     * private constructor! only LayerTagManager can be used to create layer tag masks.
     * Creates mask ANY if val==1 and mask NONE if val==0.
     */
    LayerTagMask(bool val) : any_(val), none_(!val) { }

    LayerTagCollection tags_;

    bool any_;
    bool none_;

};


#endif