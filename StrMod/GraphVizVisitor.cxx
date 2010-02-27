/* -*-c-file-style: "hopper";-*- */

/*
 * Copyright 2000 by Eric M. Hopper <hopper@omnifarious.mn.org>
 * 
 *     This program is free software; you can redistribute it and/or modify it
 *     under the terms of the GNU Lesser General Public License as published
 *     by the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful, but
 *     WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *     Lesser General Public License for more details.
 * 
 *     You should have received a copy of the GNU Lesser General Public
 *     License along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Header$ */

// For a log, see ChangeLog

#ifdef __GNUG__
#  pragma implementation "GraphVizVisitor.h"
#endif

#include "StrMod/GraphVizVisitor.h"
#include "StrMod/DynamicBuffer.h"
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <cctype>
#include <cstring>

namespace strmod {
namespace strmod {

/*!
 * @param root The root of the StrChunk DAG to be printed.
 *
 * @param out The ostream to print the GraphViz parseable tree on.
 *
 * @return A pointer to a StrChunk containing the data in the DAG flattened
 * into a single buffer.
 */
const StrChunkPtr GraphVizVisitor::visit(const StrChunkPtr &root, std::ostream &out)
{
   StrChunkPtr retval;
   static char name = 'A';
   if (root)
   {
      data_.reset(new DynamicBuffer);
      data_->resize(root->Length());
      rootpos_ = 0;
      edges_.clear();
      out_ = &out;
      try {
          out << "digraph " << (name++) << " {\n";
          startVisit(root);
          {
              const void *dataptr = data_->getVoidP();
              out << "n_" << root.get() << " -> d_" << dataptr << ";\n";
              printData(data_->getVoidP(), data_->Length());
              out.flush();
              assert(data_->Length() == rootpos_);
          }
          out << "}\n";
      }
      catch (...) {
          // Clean ourselves up if an exception happens.
          out_ = NULL;
          rootpos_ = 0;
          data_.reset();
          edges_.clear();
          throw;
      }
      out_ = NULL;
      rootpos_ = 0;
      retval = data_;
      data_.reset();
      edges_.clear();
   }
   return(retval);
}

void GraphVizVisitor::use_visitStrChunk(const StrChunkPtr &chunk,
                                        const LinearExtent &used)
   throw(halt_visitation)
{
   const void *parent = getParent().get();
   if (parent)
   {
      const void *me = chunk.get();
      const edge_t edge(me, parent);
      if (edges_.find(edge) == edges_.end())
      {
         edges_.insert(edge);
         (*out_) << "n_" << parent << " -> n_" << me << ";\n";
      }
   }
}

void GraphVizVisitor::use_visitDataBlock(const void *start, size_t len,
                                         const void *realstart, size_t reallen)
   throw(halt_visitation)
{
   const void *parent = getParent().get();
   const void *me = realstart;
   const edge_t edge(me, parent);
   if (edges_.find(edge) == edges_.end())
   {
      edges_.insert(edge);
      (*out_) << "n_" << parent << " -> d_" << me << ";\n";
      printData(realstart, reallen);
   }
   assert((rootpos_ + len) <= data_->Length());
   memcpy(data_->getCharP() + rootpos_, start, len);
   rootpos_ += len;
}

/*!
 * Prints out a chunk of data that may contain non-printable characters.  It
 * prints out the octal escape sequence for a non-printable character, or a
 * '"' character.  It captures '"' characters because those are used to
 * delimit strings in GraphViz.
 */
void GraphVizVisitor::printData(const void *data, size_t len)
{
   (*out_) << "d_" << data << "[shape=box,label=\"";
   const unsigned char *tmp = static_cast<const unsigned char *>(data);
   for (size_t i = 0; i < len; ++i)
   {
      if (tmp[i] == '"')
      {
         (*out_) << "\\042";
      }
      else if (isprint(tmp[i]))
      {
         (*out_) << tmp[i];
      }
      else
      {
         unsigned int val = tmp[i];
         (*out_) << '\\' << std::oct << val;
      }
   }
   (*out_) << "\"];\n";
}

};  // End namespace strmod
};  // End namespace strmod
