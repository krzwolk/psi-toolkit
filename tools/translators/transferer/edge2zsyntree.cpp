/*
  Copyright (C) 2007-2011 Poleng Sp. z o.o.

  This file can be redistributed and/or modified under the terms of the
  GNU Lesser General Public Licence as published by the Free Software
  Foundation.
*/

#include "edge2zsyntree.hpp"

#include <ctype.h>

//#include "plstring.h"
//#include "dicaux.h"

#include <stack>

zsyntree* convertEdgeToZsyntree(
    Lattice& lattice,
    Lattice::EdgeDescriptor start_edge)
{
    zsymbolfactory* sym_fac = lattice.getAnnotationItemManager().getSymbolFactory();
    zobjects_holder* holder = lattice.getAnnotationItemManager().getZObjectsHolderPtr();

    Lattice::EdgeDescriptor edge = start_edge;

    size_t partition_size = 0;

    zsyntree* R;

    zsyntree* pR = NULL;

    std::stack<Lattice::EdgeDescriptor> edgeStack;
    std::stack<zsyntree*> nodeStack;
    std::stack<size_t> iStack;

    size_t i = 0;

#ifdef GETSYMBOL4STRING
#error Name conflict GETSYMBOL4STRING
#endif
#define GETSYMBOL4STRING(s) ( sym_fac->get_symbol( ( s ) . c_str() ) )

mloop:

    // ---------------- GENERATING zsyntree node ------------------

//    if(a_pnode == NULL)
//    {
//        R = NULL;
//        goto backtracking;
//    }

    R = zsyntree::generate(holder);

    (*R).setCategory(
        GETSYMBOL4STRING(lattice.getAnnotationCategory(edge)));

//    (*R).setSegmentInfo(a_pnode->segment_beg,
//                        a_pnode->segment_length);

//    if(a_pnode->is_variant)
//        (*R).makeVariant();

//    (*R).setAttr(sm_Beg, INTEGER_TO_ZVALUE(a_pnode->segment_beg));
//    (*R).setAttr(sm_Score, INTEGER_TO_ZVALUE(int(a_pnode->tscore)));

    typedef std::pair<std::string, zvalue> StrZValPair;
    std::list<StrZValPair> avPairs
        = lattice.getAnnotationItemManager().getValuesAsZvalues(
            lattice.getEdgeAnnotationItem(edge));
    BOOST_FOREACH(StrZValPair av, avPairs) {
        zsymbol* aname_sm = GETSYMBOL4STRING(av.first);
        (*R).setAttr(aname_sm, av.second);

            // if(aname_sm == sm_S ||
            //    aname_sm == sm_Sem1 ||
            //    aname_sm == sm_Sem2 ||
            //    aname_sm == sm_Sem3)
            //     (*R).setAttr(aname_sm, sm_any);
            // else
            //     (*R).setAttr(aname_sm, sm_nil);
    }


//     if(a_pnode->target_data != NULL)
//     {
//         zsyntree* eT = zsyntree::generate(holder);

//         (*eT).setAttr(sm_Inflection,
//                       GETSYMBOL4STRING(a_pnode->target_data
//                                        ->inflection));

//         (*eT).setAttr(sm_Equiv,
//                       GETSYMBOL4STRING(a_pnode->target_data
//                                        ->equiv));

//         (*eT).setAttr(sm_Context,
//                       GETSYMBOL4STRING(a_pnode->target_data
//                                        ->context));

//         if(!a_pnode->target_data->spec_prep.empty())
//             (*eT).setAttr(sm_SpecPrep,
// 			      GETSYMBOL4STRING(a_pnode->target_data
//                                                ->spec_prep));

//         if(!a_pnode->target_data->spec_cat.empty())
//             (*eT).setAttr(sm_SpecCat,
//                           GETSYMBOL4STRING(a_pnode->target_data
//                                            ->spec_cat));

//         if(!a_pnode->target_data->spec_def.empty())
//             (*eT).setAttr(sm_SpecDef,
//                           GETSYMBOL4STRING(a_pnode->target_data
//                                            ->spec_def));

// #define ENPLSYNTREE_COMPLEMENTATION(ic)                                 \
//                                                                         \
//         if(!a_pnode->target_data->compls[(ic) - 1].empty())             \
//             (*eT).setAttr(sm_Dop##ic,                                   \
//                           GETSYMBOL4STRING(a_pnode->target_data         \
//                                            ->compls[(ic) - 1]));        \
//         if(!a_pnode->target_data->preps[(ic) - 1].empty())              \
//             (*eT).setAttr(sm_Przyim##ic,                                \
//                           GETSYMBOL4STRING(a_pnode->target_data         \
//                                            ->preps[(ic) - 1]));         \
//         if(!a_pnode->target_data->defs[(ic) - 1].empty())               \
//             (*eT).setAttr(sm_Def##ic,                                   \
//                           GETSYMBOL4STRING(a_pnode->target_data         \
//                                            ->defs[(ic) - 1]));

// #define PLENSYNTREE_COMPLEMENTATION(ic)                                 \
// 	    								 \
//         if(!a_pnode->target_data->compls[(ic) - 1].empty())             \
//             (*eT).setAttr(sm_Compl##ic,                                 \
//                           GETSYMBOL4STRING(a_pnode->target_data         \
//                                            ->compls[(ic) - 1]));        \
// 		if(!a_pnode->target_data->preps[(ic) - 1].empty())	\
//                     (*eT).setAttr(sm_Prep##ic,                          \
//                                   GETSYMBOL4STRING(a_pnode->target_data \
//                                                    ->preps[(ic) - 1])); \
// 		if(!a_pnode->target_data->defs[(ic) - 1].empty())	\
//                     (*eT).setAttr(sm_Def##ic,                           \
//                                   GETSYMBOL4STRING(a_pnode->target_data \
//                                                    ->defs[(ic) - 1]));

//         PLENSYNTREE_COMPLEMENTATION(1);
//         PLENSYNTREE_COMPLEMENTATION(2);
//         PLENSYNTREE_COMPLEMENTATION(3);

//         std::string esyn = a_pnode->target_data->syntax;

//         if(esyn.length() != 0)
//         {
//             std::list<std::string> LR;

//             LR = daux::splitAttribute(esyn);

//             std::list<std::string>::const_iterator iter =
//                 LR.begin();

//             for( ; iter != LR.end() ; ++iter)
//             {
//                 aname = (*iter);
//                 aname[0] = toupper(aname[0]);

//                 size_t f = aname.find('=');

//                 if(f != std::string::npos)
//                 {
//                     std::string value =
//                         aname.substr(f+1);
//                     aname = aname.substr(0, f);

//                     char* ec;
//                     int v = strtol(value.c_str(), &ec, 10);

//                     if(*ec == '\0')
//                         (*eT).setAttr(GETSYMBOL4STRING(aname),
//                                       INTEGER_TO_ZVALUE(v));
//                     else
//                     {
//                         if(value.length() >= 2 &&
//                            value[0] == '\'' &&
// 			       value[value.length()-1] == '\'')
//                             value = value.substr(1, value.length()-2);

//                         (*eT).setAttr(GETSYMBOL4STRING(aname),
//                                       GETSYMBOL4STRING(value));
//                     }
//                 }
//                 else
//                 {
//                     (*eT).setAttr(GETSYMBOL4STRING(aname),
//                                   INTEGER_TO_ZVALUE(1));
//                 }
//             }
//         }

//         (*eT).setAttr(sm_Eid,
//                       INTEGER_TO_ZVALUE(a_pnode->target_data->eid));

//         (*R).setEquivTree(eT);
//     }

    // ----------------------- END (GENERATING) ------------------

    i = 0;

backtracking:

    partition_size = 0;

    if (!lattice.getEdgePartitions(edge).empty())
        partition_size =
            lattice.getEdgePartitions(edge).front().getSequence().size(lattice);

    if (i >= partition_size) {
        if(edgeStack.empty())
            goto finish;

        pR = nodeStack.top();
        nodeStack.pop();

        size_t pi = iStack.top();
        iStack.pop();

        Lattice::EdgeDescriptor ppnode = edgeStack.top();
        edgeStack.pop();

        if (pR != NULL && R != NULL)
            pR->addSubtree(R, NULL);

                // ( (ppnode->subnode_infos == NULL ||
                //    a_labels.getKey(
                //        ppnode->
                //        subnode_infos[pi-1].id_label).length() == 0)
                //   ?
                //   NULL :
//                  GETSYMBOL4STRING(
//                      a_labels.getKey(
//                          ppnode->
//                          subnode_infos[pi-1].id_label))) );

        R = pR;
        i = pi;
        edge = ppnode;

        goto backtracking;
    }
    else {
        edgeStack.push(edge);
        iStack.push(i+1);
        nodeStack.push(R);

        const Lattice::Partition& partition = lattice.getEdgePartitions(edge).front();
        edge = partition.getSequence().nthEdge(lattice, i);
        goto mloop;
    }

finish:

    return R;
}