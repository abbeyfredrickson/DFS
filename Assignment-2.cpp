//===- Software-Analysis-Teaching Assignment 2-------------------------------------//
//
//     SVF: Static Value-Flow Analysis Framework for Source Code
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
 // Software-Analysis-Teaching Assignment 2 : Source Sink ICFG DFS Traversal
 //
 //
 */

#include <set>
#include "Assignment-2.h"
#include <iostream>
using namespace SVF;
using namespace std;

/// TODO: print each path once this method is called, and
/// add each path as a string into std::set<std::string> paths
/// Print the path in the format "START->1->2->4->5->END", where -> indicate an ICFGEdge connects two ICFGNode IDs

void ICFGTraversal::collectICFGPath(std::vector<unsigned> &path)
{
  string pathStr = "START->";
  for (unsigned nodeID : path)
  {
    pathStr += to_string(nodeID) + "->";
  }
  pathStr += "END";

  cout << pathStr << "\n";
  paths.insert(pathStr);
}

/// TODO: Implement your context-sensitive ICFG traversal here to traverse each program path (once for any loop) from src to dst
void ICFGTraversal::reachability(const ICFGNode *src, const ICFGNode *dst)
{
  pair<const ICFGNode *, CallStack> pair = {src, callstack};

  if (visited.count(pair) != 0)
  { // if pair has been visited
    return;
  }

  visited.insert(pair);
  path.push_back(src->getId()); // path type std::vector<SVF::NodeID>

  if (src == dst)
  {
    collectICFGPath(path);
  }

  for (ICFGEdge *edge : src->getOutEdges())
  {
    ICFGNode *edgeDst = edge->getDstNode();

    if (edge->isIntraCFGEdge())
    {
      reachability(edgeDst, dst);
    }
    else if (edge->isCallCFGEdge())
    {
      callstack.push_back(src);
      reachability(edgeDst, dst);
      callstack.pop_back();
    }
    else if (edge->isRetCFGEdge())
    {
      RetICFGNode *retNode = SVFUtil::dyn_cast<RetICFGNode>(edgeDst);
      const CallICFGNode *callNode = retNode->getCallICFGNode();

      if (!(callstack.empty()) && callstack.back() == callNode)
      {
        callstack.pop_back();
        reachability(edgeDst, dst);
        callstack.push_back(callNode);
      }
      else if (callstack.empty())
      {
        reachability(edgeDst, dst);
      }
    }
  }

  visited.erase(pair);
  path.pop_back();
}
