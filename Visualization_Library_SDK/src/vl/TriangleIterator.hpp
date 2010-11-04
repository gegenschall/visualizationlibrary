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

#ifndef TriangleIterator_INCLUDE_ONCE
#define TriangleIterator_INCLUDE_ONCE

#include <vl/Array.hpp>
#include <vl/vlnamespace.hpp>

namespace vl
{
//-----------------------------------------------------------------------------
// TriangleIteratorAbstract
//-----------------------------------------------------------------------------
  class TriangleIteratorAbstract: public Object
  {
  public:
    virtual bool next() = 0;
    virtual bool isEnd() const = 0;
    virtual int a() const = 0;
    virtual int b() const = 0;
    virtual int c() const = 0;
  };
//-----------------------------------------------------------------------------
// TriangleIteratorIndexed
//-----------------------------------------------------------------------------
  template<class TArray>
  class TriangleIteratorIndexed: public TriangleIteratorAbstract
  {
  public:
    TriangleIteratorIndexed(TArray* idx_array, EPrimitiveType prim_type, int base_vert, bool prim_restart_on, int prim_restart_idx)
    {
      mCurrentIndex = 0;
      mEnd    = 0;
      mA = mB = mC = -1;
      mEven = true;
      mLastTriFanPoly = true;
      mIndex0 = 0;
      mArray            = idx_array;
      mPrimRestartIndex = prim_restart_idx;
      mPrimRestartOn    = prim_restart_on;
      mBaseVertex       = base_vert;
      mPrimType         = prim_type;
    }

    bool isEnd() const { return mCurrentIndex == mEnd; }

    bool operator==(const TriangleIteratorIndexed& other) { return mCurrentIndex == other.mCurrentIndex; }

    virtual int a() const { return mA; }
    virtual int b() const { return mB; }
    virtual int c() const { return mC; }

    void initialize(int start=0, int end=-1)
    {
      VL_CHECK( start >= 0 )
      VL_CHECK( end <= (int)mArray->size() )

      if (end == -1)
        end = mArray->size();

      mCurrentIndex = end; // end
      mA = mB = mC = -1;
      mEven = true;
      mLastTriFanPoly = true;
      mIndex0 = start;
      mEnd    = end;
      if (mArray->size())
      {
        switch(mPrimType)
        {
        case PT_TRIANGLES:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_TRIANGLE_STRIP:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_TRIANGLE_FAN:
          mCurrentIndex = start + 1;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_POLYGON:
          mCurrentIndex = start + 1;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_QUADS:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        case PT_QUAD_STRIP:
          mCurrentIndex = start;
          mA = mArray->at(start+0);
          mB = mArray->at(start+1);
          mC = mArray->at(start+2);
          break;
        }
      }
      // if we are not at the end then add base vertices
      if ( mCurrentIndex != mEnd )
      {
        mA += mBaseVertex;
        mB += mBaseVertex;
        mC += mBaseVertex;
      }
      else
      {
        mA = mB = mC = -1;
      }
    }

    bool next()
    {
      // reached the end
      if ( mCurrentIndex == mEnd )
        return false;

      switch(mPrimType)
      {

      case PT_TRIANGLES:
        mCurrentIndex += 3;
        // check for the end
        if ( mCurrentIndex >= mEnd )
          mCurrentIndex = mEnd;
        else
        if ( isPrimRestart(mCurrentIndex) )
        {
          mCurrentIndex += 1;
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        else
        {
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        break;

      case PT_QUAD_STRIP:
      case PT_TRIANGLE_STRIP:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 2 >= mEnd )
          mCurrentIndex = mEnd;
        else
        if ( isPrimRestart(mCurrentIndex + 2) )
        {
          mCurrentIndex += 3;
          mEven = true;
          mA = mArray->at(mCurrentIndex + 0);
          mB = mArray->at(mCurrentIndex + 1);
          mC = mArray->at(mCurrentIndex + 2);
        }
        else
        {
          mEven = !mEven;
          if (mEven)
          {
            mA = mArray->at(mCurrentIndex + 0);
            mB = mArray->at(mCurrentIndex + 1);
            mC = mArray->at(mCurrentIndex + 2);
          }
          else
          {
            mA = mArray->at(mCurrentIndex + 0);
            mB = mArray->at(mCurrentIndex + 2);
            mC = mArray->at(mCurrentIndex + 1);
          }
        }
        break;

      case PT_TRIANGLE_FAN:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 1 >= mEnd )
        {
          if (mLastTriFanPoly)
          {
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex);
            mC = mArray->at(mIndex0 + 1);
            mLastTriFanPoly = false;
            mCurrentIndex--;
          }
          else
            mCurrentIndex = mEnd;
        }
        else
        if (isPrimRestart(mCurrentIndex + 1))
        {
          if (mLastTriFanPoly)
          {
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex);
            mC = mArray->at(mIndex0 + 1);
            mLastTriFanPoly = false;
            mCurrentIndex--;
          }
          else
          {
            mLastTriFanPoly = true;
            mIndex0 = mCurrentIndex + 2;
            mCurrentIndex = mIndex0 + 1;
            mA = mArray->at(mIndex0);
            mB = mArray->at(mCurrentIndex + 0);
            mC = mArray->at(mCurrentIndex + 1);
          }
        }
        else
        {
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        break;

      case PT_POLYGON:
        mCurrentIndex += 1;
        if ( mCurrentIndex + 1 >= mEnd )
        {
          mCurrentIndex = mEnd;
        }
        else
        if ( isPrimRestart(mCurrentIndex + 1) )
        {
          mIndex0 = mCurrentIndex + 2;
          mCurrentIndex = mIndex0 + 1;
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        else
        {
          mA = mArray->at(mIndex0);
          mB = mArray->at(mCurrentIndex + 0);
          mC = mArray->at(mCurrentIndex + 1);
        }
        break;

      case PT_QUADS:
        mCurrentIndex += 2;
        if ( mCurrentIndex >= mEnd )
        {
          mCurrentIndex = mEnd;
        }
        else
        if ( isPrimRestart(mCurrentIndex) )
        {
          mCurrentIndex += 1;
          mEven = true;
          mA = mArray->at(mCurrentIndex+0);
          mB = mArray->at(mCurrentIndex+1);
          mC = mArray->at(mCurrentIndex+2);
        }
        else
        {
          mEven = !mEven;
          if ( mEven )
          {
            mA = mArray->at(mCurrentIndex+0);
            mB = mArray->at(mCurrentIndex+1);
            mC = mArray->at(mCurrentIndex+2);
          }
          else
          {
            mA = mArray->at(mCurrentIndex+0);
            mB = mArray->at(mCurrentIndex+1);
            mC = mArray->at(mCurrentIndex-2);
          }
        }
        break;
      }

      // if we are not at the end then add base vertices
      if (mCurrentIndex != mEnd)
      {
        mA += mBaseVertex;
        mB += mBaseVertex;
        mC += mBaseVertex;
        return true;
      }
      else
      {
        mA = mB = mC = -1;
        return false;
      }
    }

  private:
    bool isPrimRestart(int i) const { return mPrimRestartOn && (int)mArray->at(i) == mPrimRestartIndex; }

  private:
    ref<TArray> mArray;
    EPrimitiveType mPrimType;
    int  mA, mB, mC;
    int  mCurrentIndex;
    int  mIndex0;
    int  mEnd;
    int  mBaseVertex;
    int  mPrimRestartIndex;
    bool mPrimRestartOn;
    bool mEven;
    bool mLastTriFanPoly;
  };
//-----------------------------------------------------------------------------
// TriangleIterator
//-----------------------------------------------------------------------------
  class TriangleIterator
  {
  public:
    TriangleIterator(TriangleIteratorAbstract* it): mIterator(it) { }

    bool next() { return mIterator->next(); }

    bool isEnd() { return mIterator->isEnd(); }

    int a() const { return mIterator->a(); }

    int b() const { return mIterator->b(); }

    int c() const { return mIterator->c(); }

  protected:
    ref<TriangleIteratorAbstract> mIterator;
  };
//-----------------------------------------------------------------------------
}

#endif