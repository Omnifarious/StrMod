/* -*-c-file-style: "hopper";-*- */
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

const STR_ClassIdent GraphVizVisitor::identifier(48UL);

void GraphVizVisitor::visit(const StrChunkPtr &root, ostream &out)
{
   static char name = 'A';
   if (root)
   {
      data_ = new DynamicBuffer;
      data_->resize(root->Length());
      rootpos_ = 0;
      edges_.clear();
      out_ = &out;
      out << "digraph " << (name++) << " {\n";
      startVisit(root);
      {
         const void *rootptr = root.GetPtr();
         const void *dataptr = data_->getVoidP();
         out << "n_" << root.GetPtr() << " -> d_" << dataptr << ";\n";
         printData(data_->getVoidP(), data_->Length());
         out.flush();
         assert(data_->Length() == rootpos_);
      }
      out << "}\n";
      out_ = NULL;
      rootpos_ = 0;
      delete data_;
      data_ = 0;
      edges_.clear();
   }
}

void GraphVizVisitor::use_visitStrChunk(const StrChunkPtr &chunk,
                                        const LinearExtent &used)
   throw(halt_visitation)
{
   const void *parent = getParent().GetPtr();
   if (parent)
   {
      const void *me = chunk.GetPtr();
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
   const void *parent = getParent().GetPtr();
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
         (*out_) << '\\' << oct << val;
      }
   }
   (*out_) << "\"];\n";
}

