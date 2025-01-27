//===- Software-Analysis-Teaching Assignment 3-------------------------------------//
//
//    SVF: Static Value-Flow Analysis Framework for Source Code
//
// Copyright (C) <2013->
//

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//===-----------------------------------------------------------------------===//

/*
 // Software-Analysis-Teaching Assignment 3 : Andersen's pointer analysis
 //
 //
 */

#include "SVF-LLVM/LLVMUtil.h"
#include "SVF-LLVM/SVFIRBuilder.h"
#include "WPA/Andersen.h"
#include "Assignment-3.h"
using namespace SVF;
using namespace llvm;
using namespace std;

// TODO: Implement your Andersen's Algorithm here
void AndersenPTA::solveWorklist()
{
    processAllAddr();

    // Keep solving until workList is empty.
    while (!isWorklistEmpty())
    {
        NodeID pID = AndersenBase::popFromWorklist();
        ConstraintNode *p = consCG->getConstraintNode(pID);

        for (NodeID oID : BVDataPTAImpl::getPts(pID)) // for each memory object in pts(p)
        {
            ConstraintNode *o = consCG->getConstraintNode(oID);

            for (ConstraintEdge *edge : p->getStoreInEdges()) // store rule
            {
                ConstraintNode *q = edge->getSrcNode();
                NodeID qID = q->getId();
                if (!consCG->hasEdge(q, o, ConstraintEdge::Copy))
                {
                    addCopyEdge(qID, oID); // add copy edge
                    AndersenBase::pushIntoWorklist(qID);
                }
            }

            for (ConstraintEdge *edge : p->getLoadOutEdges()) // load rule
            {
                ConstraintNode *r = edge->getDstNode();
                NodeID rID = r->getId();
                if (!consCG->hasEdge(o, r, ConstraintEdge::Copy))
                {
                    addCopyEdge(oID, rID); // add copy edge
                    AndersenBase::pushIntoWorklist(oID);
                }
            }
        }

        for (ConstraintEdge *edge : p->getDirectOutEdges()) // copy rule
        {
            NodeID xID = edge->getDstID();
            if (BVDataPTAImpl::unionPts(xID, pID))
            {
                AndersenBase::pushIntoWorklist(xID);
            }
        }
    }
}

// Process all address constraints to initialize pointers' points-to sets
void AndersenPTA::processAllAddr()
{
    for (ConstraintGraph::const_iterator nodeIt = consCG->begin(), nodeEit = consCG->end(); nodeIt != nodeEit; nodeIt++)
    {
        ConstraintNode *cgNode = nodeIt->second;
        for (ConstraintEdge *edge : cgNode->getAddrInEdges())
        {
            const AddrCGEdge *addr = SVFUtil::cast<AddrCGEdge>(edge);
            NodeID dst = addr->getDstID();
            NodeID src = addr->getSrcID();
            if (addPts(dst, src))
                pushIntoWorklist(dst);
        }
    }
}
