/**************************************************************************************/
/*                                                                                    */
/*  Visualization Library                                                             */
/*  http://www.visualizationlibrary.com                                               */
/*                                                                                    */
/*  Copyright (c) 2005-2010, Michele Bosi                                             */
/*  All rights reserved.                                                              */
/*                                                                                    */
/*  Redistribution and use in source and binary forms, with or without modification,  */
/*  are permitted provided that the following conditions are met:                     */
/*                                                                                    */
/*  - Redistributions of source code must retain the above copyright notice, this     */
/*  list of conditions and the following disclaimer.                                  */
/*                                                                                    */
/*  - Redistributions in binary form must reproduce the above copyright notice, this  */
/*  list of conditions and the following disclaimer in the documentation and/or       */
/*  other materials provided with the distribution.                                   */
/*                                                                                    */
/*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND   */
/*  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED     */
/*  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE            */
/*  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR  */
/*  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES    */
/*  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;      */
/*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON    */
/*  ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT           */
/*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS     */
/*  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                      */
/*                                                                                    */
/**************************************************************************************/

#if !defined(ioPNG_INCLUDE_ONCE)
#define ioPNG_INCLUDE_ONCE

#include <vl/ResourceLoadWriter.hpp>
#include <vl/ResourceDatabase.hpp>

namespace vl
{
  class VirtualFile;
  class String;
  class Image;

  ref<Image> loadPNG(VirtualFile* file);
  ref<Image> loadPNG(const String& path);
  bool isPNG(VirtualFile* file);
  bool savePNG(const Image* src, const String& path, int compression = 6);
  bool savePNG(const Image* src, VirtualFile* file, int compression = 6);

  //---------------------------------------------------------------------------
  // LoadWriterPNG
  //---------------------------------------------------------------------------
  /**
   * The LoadWriterPNG class is a ResourceLoadWriter capable of reading PNG files.
   */
  class LoadWriterPNG: public ResourceLoadWriter
  {
  public:
    virtual const char* className() { return "LoadWriterPNG"; }
    LoadWriterPNG(): ResourceLoadWriter("|png|", "|png|"), mCompression(6) {}

    ref<ResourceDatabase> loadResource(const String& path) const 
    {
      ref<ResourceDatabase> res_db = new ResourceDatabase;
      ref<Image> img = loadPNG(path);
      if (img)
        res_db->resources().push_back(img);
      return res_db;
    }

    ref<ResourceDatabase> loadResource(VirtualFile* file) const
    {
      ref<ResourceDatabase> res_db = new ResourceDatabase;
      ref<Image> img = loadPNG(file);
      if (img)
        res_db->resources().push_back(img);
      return res_db;
    }

    bool writeResource(const String& path, ResourceDatabase* resource) const
    {
      bool ok = true;
      for(unsigned i=0; i<resource->count<Image>(); ++i)
        ok &= savePNG(resource->get<Image>(i), path, compression());
      return ok;
    }

    bool writeResource(VirtualFile* file, ResourceDatabase* resource) const
    {
      bool ok = true;
      for(unsigned i=0; i<resource->count<Image>(); ++i)
        ok &= savePNG(resource->get<Image>(i), file, compression());
      return ok;
    }

    int compression() const { return mCompression; }
    //! Sets the compression level used when saving a file. Should be between 0 and 9
    void setCompression(int compression) { mCompression = compression; }

  protected:
    int mCompression;
  };
}

#endif
