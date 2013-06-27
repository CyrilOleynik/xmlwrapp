/*
 * Copyright (C) 2001-2003 Peter J Jones (pjones@pmade.org)
 * All Rights Reserved
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _xmlwrapp_utility_h_
#define _xmlwrapp_utility_h_

#include <xmlwrapp/node.h>

// standard includes
#include <string>
#include <cstdarg>

// libxml2 includes
#include <libxml/tree.h>

namespace xml
{

namespace impl
{

// exception safe wrapper around xmlChar*s that are returned from some
// of the libxml functions that the user must free.
class xmlchar_helper
{
public:
    xmlchar_helper(xmlChar *ptr) : ptr_(ptr) {}

    ~xmlchar_helper()
        { if (ptr_) xmlFree(ptr_); }

    const char* get() const
        { return reinterpret_cast<const char*>(ptr_); }

private:
    xmlChar *ptr_;
};

// Formats given message with arguments into a std::string
void printf2string(std::string& s, const char *message, va_list ap);

// Sun CC uses ancient C++ standard library that doesn't have standard
// std::distance(). Work around it here
#if defined(__SUNPRO_CC) && !defined(_STLPORT_VERSION)
template<typename T>
inline size_t distance(T a, const T&b)
{
    size_t n = 0;
    for ( ; a != b; ++a )
        ++n;
    return n;
}
#endif // defined(__SUNPRO_CC) && !defined(_STLPORT_VERSION)

} // namespace impl

} // namespace xml


#endif // _xmlwrapp_utility_h_