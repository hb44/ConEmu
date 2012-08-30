
/*
Copyright (c) 2009-2012 Maximus5
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the authors may not be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#pragma once

#ifndef _MARRAY_HEADER_
#define _MARRAY_HEADER_

//#pragma warning( disable : 4101 )

#ifndef MCHKHEAP
	#define MCHKHEAP
#endif

#define _ARRAY_ASSERTE(x)

template<class _Ty>
class MArray
{
    public:
        MArray()
        {
            MCHKHEAP;
            mp_Elements = NULL; mn_TySize=sizeof(_Ty);
            mn_Elements = 0; mn_MaxSize = 0;
            #ifdef _WIN64
            mn_Reserved = 0xCCCCCCCCCCCCCCCCLL; // Debug purposes
            #else
            mn_Reserved = 0xCCCCCCCC; // Debug purposes
            #endif
            MCHKHEAP;
            if (mn_TySize==0)
            {
            	_ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            MCHKHEAP;
        };
        ~MArray()
        {
            clear();
        };

        const _Ty & operator[](INT_PTR _P) const
        {
            #ifdef _DEBUG
            if (_P<0 || _P>=mn_Elements)
            {
            	_ARRAY_ASSERTE(!(_P<0 || _P>=mn_Elements));
            }
            _Ty _p = ((_Ty*)mp_Elements)[_P];
            #endif
            return ((_Ty*)mp_Elements)[_P];
        }
        _Ty & operator[](INT_PTR _P)
        {
            #ifdef _DEBUG
            if (_P<0 || _P>=mn_Elements)
            {
                _ARRAY_ASSERTE(!(_P<0 || _P>=mn_Elements));
            }
            _Ty _p = ((_Ty*)mp_Elements)[_P];
            #endif
            return ((_Ty*)mp_Elements)[_P];
        }

        void push_back(const _Ty& _X)
        {
            if (mn_TySize==0)
            {
                _ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            MCHKHEAP;
            if (mn_MaxSize<=mn_Elements)
            {
            	_ARRAY_ASSERTE(mn_MaxSize==mn_Elements);
                addsize(max(256,mn_MaxSize));
            }
            memmove(
                ((_Ty*)mp_Elements)+
                (mn_Elements++),
                &_X, mn_TySize);
            MCHKHEAP;
        }
        void pop_back(_Ty& _X)
        {
            if (mn_TySize==0)
            {
                _ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            MCHKHEAP;
            memmove(
                 &_X,
                 ((_Ty*)mp_Elements)+(mn_Elements-1),
                 mn_TySize);
            MCHKHEAP;
            mn_Elements--;
        }
        _Ty* detach()
        {
            _Ty* p = mp_Elements;  mp_Elements = NULL;
            clear();
            return p;
        }
        void clear()
        {
            if (mp_Elements)
            {
            	free(mp_Elements);
            	mp_Elements = NULL;
            }
            mn_MaxSize = 0; mn_Elements = 0;
            MCHKHEAP;
        }
        void erase(INT_PTR _P)
        {
            #ifdef _DEBUG
            if (_P<0 || _P>=mn_Elements)
            {
                _ARRAY_ASSERTE(!(_P<0 || _P>=mn_Elements));
            }
            #endif

            if ((_P+1)<mn_Elements)
            {
                MCHKHEAP;
                memmove(((_Ty*)mp_Elements)+_P,((_Ty*)mp_Elements)+_P+1,
                    mn_TySize*(mn_Elements - _P - 1));
                //TODO: ���������, ��������� �� ���������� �������
                memset(((_Ty*)mp_Elements)+mn_Elements-1,0,mn_TySize);
                MCHKHEAP;
            }
            mn_Elements--;
        }
        void eraseall()
        {
            mn_Elements = 0;
        };

        INT_PTR size() const
        {
            return mn_Elements;
        };

        void addsize(INT_PTR nElements)
        {
            if (mn_TySize==0)
            {
                _ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            if (nElements < 0)
            {
            	_ARRAY_ASSERTE(nElements > 0);
            	return;
            }
            MCHKHEAP;
            INT_PTR nOldMaxSize = mn_MaxSize;
            mn_MaxSize += nElements;
            if (mp_Elements)
            {
                _Ty* ptrNew = (_Ty*)calloc(mn_MaxSize, mn_TySize);
                MCHKHEAP;
                if (ptrNew==NULL)
                {
                    mn_MaxSize = nOldMaxSize;
                    _ARRAY_ASSERTE(ptrNew!=NULL);
                    return;
                }
                memmove(ptrNew, mp_Elements, nOldMaxSize*mn_TySize);
                MCHKHEAP;
                if (mp_Elements)
                {
                	free(mp_Elements);
                	mp_Elements = NULL;
                }
                mp_Elements = ptrNew;
                ptrNew = NULL;
            }
            else
            {
                mp_Elements = (_Ty*)calloc(mn_MaxSize, mn_TySize);
                if (mp_Elements==NULL)
                {
                    mn_MaxSize = nOldMaxSize;
                    _ARRAY_ASSERTE(mp_Elements!=NULL);
                    return;
                }
            }
            MCHKHEAP;
        };

        #ifndef NOMARRAYSORT
        void sort(int(*MARRAYSORTCALLBACK)(_Ty &e1, _Ty &e2))
        {
            MCHKHEAP;
            int liMin, liCmp;
            _Ty tmp;
            for (INT_PTR i=0; i<mn_Elements-1; i++)
            {
                liMin = i;
                for (INT_PTR j=i+1; j<mn_Elements; j++)
                {
                    liCmp = MARRAYSORTCALLBACK(((_Ty*)mp_Elements)[liMin], 
                        ((_Ty*)mp_Elements)[j]);
                    if (liCmp>0)
                        liMin = j;
                }
                if (liMin!=i)
                {
                    MCHKHEAP;
                    memmove(&tmp, &((_Ty*)mp_Elements)[liMin], sizeof(_Ty));
                    MCHKHEAP;
                    memmove(&((_Ty*)mp_Elements)[liMin], &((_Ty*)mp_Elements)[i], sizeof(_Ty));
                    MCHKHEAP;
                    memmove(&((_Ty*)mp_Elements)[i], &tmp, sizeof(_Ty));
                    MCHKHEAP;
                }
            }
            MCHKHEAP;
        };
        #endif

        void alloc(INT_PTR _nCount)
        {
            if (mn_TySize==0)
            {
                _ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            if (_nCount>mn_Elements)
            {
                if (_nCount>mn_MaxSize)
                {
                    INT_PTR n1=min(4096,(_nCount-mn_MaxSize));
                    addsize(max(n1,10));
                }
                // 01.03.2005 !!! Only allocate memory, Not initialize values!
                //mn_Elements = min(_nCount,mn_MaxSize); 
            }
        }
        void set_at(INT_PTR _P, _Ty & _X)
        {
            if (mn_TySize==0)
            {
                _ARRAY_ASSERTE(!(mn_TySize==0));
                return;
            }
            #ifdef _DEBUG
            if (_P<0/* || _P>=mn_Elements*/)
            {
                _ARRAY_ASSERTE!((_P<0/* || _P>=mn_Elements*/));
            }
            #endif
            
            if (_P>=mn_Elements)
            {
                if (_P>=mn_MaxSize)
                {
                    addsize(max(256,(_P-mn_MaxSize+1)));
                }
                mn_Elements = _P+1;
            }

            #ifdef _DEBUG
            _Ty _p = ((_Ty*)mp_Elements)[_P];
            #endif

            MCHKHEAP;
            memmove(
                ((_Ty*)mp_Elements)+(_P),
                &_X, mn_TySize);
            MCHKHEAP;
        }

    protected:
        INT_PTR mn_TySize;
        INT_PTR mn_Elements;
        INT_PTR mn_MaxSize;
        INT_PTR mn_Reserved; // Debug purposes
        _Ty *mp_Elements;
};

//#pragma warning( default : 4101 )

#endif // #ifndef _MARRAY_HEADER_
