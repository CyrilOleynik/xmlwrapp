/*
 * Copyright (C) 2011 Jonas Weber <mail@jonasw.de>
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


//xmlwrapp includes
#include "xmlwrapp/namespaces.h"
#include "xmlwrapp/exception.h"
#include "pimpl_base.h"

// std
#include <new>
#include <algorithm>

// libxml2
#include <libxml/tree.h>

namespace xml
{
    namespace impl
    {


        ///////
        // nsdefs_impl
        ///////
        struct nsdefs_impl : public pimpl_base<nsdefs_impl>
        {
            xmlNodePtr node_;
            nsdefs_impl() : node_(NULL) { };

            void set_data(void* data) { node_ = reinterpret_cast<xmlNodePtr> (data); }

            xml::namespaces::definitions::iterator begin()
            {
                if (node_ == NULL)
                    return xml::namespaces::definitions::iterator(NULL);
                else
                    return xml::namespaces::definitions::iterator(node_->nsDef);
            }
            xml::namespaces::definitions::iterator end()
            {
                return xml::namespaces::definitions::iterator(NULL);
            }
        };

        struct nsdef_it_impl : public pimpl_base<nsdef_it_impl>
        {
            xmlNsPtr data;
            xml::namespaces::ns myval;

            nsdef_it_impl(void* dt) : myval(dt)
            {
                data = reinterpret_cast<xmlNsPtr>(dt);
            };

            void* get_data() const { return reinterpret_cast<void*> (data); };
            void set_data(void* dt) { data = reinterpret_cast<xmlNsPtr> (dt); myval.set_data(data); };

            void go_next()
            {
                data = data->next;
                myval.set_data(data);

            }
        };
    } // namespace impl

    //////
    // iterator
    //////

    namespaces::definitions::iterator::iterator (void *ns)
    {
        impl = new impl::nsdef_it_impl(ns);
    }
    namespaces::definitions::iterator::~iterator()
    {
        delete impl;
    }

    namespaces::definitions::iterator::value_type& namespaces::definitions::iterator::operator*()
    { return impl->myval; }

    namespaces::definitions::iterator::value_type* namespaces::definitions::iterator::operator->()
    { return &(impl->myval); }

    namespaces::definitions::iterator& namespaces::definitions::iterator::operator++()
    {
        impl->go_next();
        return *this;
    }

    namespaces::definitions::iterator namespaces::definitions::iterator::operator++(int)
    {
        iterator tmp = *this;
        impl->go_next();
        return tmp;
    }

    namespaces::definitions::iterator::iterator(const namespaces::definitions::iterator& other)
    {
        impl = new impl::nsdef_it_impl (other.get_ns());
    }

    namespaces::definitions::iterator& namespaces::definitions::iterator::operator= (const namespaces::definitions::iterator& other)
    {
        delete impl;
        impl = new impl::nsdef_it_impl (other.get_ns());
        return *this;
    }

namespace namespaces {
    bool XMLWRAPP_API operator== (const xml::namespaces::definitions::iterator& lhs, const xml::namespaces::definitions::iterator& rhs)
    {
        return (lhs.get_ns() == rhs.get_ns());
    }

    bool XMLWRAPP_API operator!=(const xml::namespaces::definitions::iterator& lhs, const xml::namespaces::definitions::iterator& rhs)
    {
        return (lhs.get_ns() != rhs.get_ns());
    }
}
    void* namespaces::definitions::iterator::get_ns() const { return impl->get_data(); }



    /////
    // nsdef
    /////

    namespaces::ns::ns(void* data)
    {
        set_data(data);
    }
    namespaces::ns::ns(const char* href, const char* prefix) : href(href), prefix(prefix)
    {
    }
    namespaces::ns::~ns()
    {
    }
    void namespaces::ns::set_data(void* data)
    {
        if (data == NULL)
        {
            href = "";
            prefix = "";
            return;
        }
        xmlNsPtr ns = reinterpret_cast<xmlNsPtr> (data);

        href = reinterpret_cast<const char*> (ns->href);
        if (ns->prefix != NULL)
            prefix = reinterpret_cast<const char*> (ns->prefix);
        else
            prefix = "";
    }
    const char* namespaces::ns::get_href() const
    {
        return href.c_str();
    }

    const char* namespaces::ns::get_prefix() const
    {
        return prefix.c_str();
    }



    namespaces::definitions::definitions(int) : impl(NULL)
    {
    };



    namespaces::definitions::~definitions()
    {
        if (impl != NULL)
            delete impl;
    }

    void namespaces::definitions::set_data(void* data)
    {
        if (impl == NULL)
            impl = new impl::nsdefs_impl();
        impl->set_data(data);
    }

    namespaces::definitions::iterator namespaces::definitions::begin()
    {
        return impl->begin();
    }

    namespaces::definitions::iterator namespaces::definitions::end()
    {
        return impl->end();
    }

    bool namespaces::definitions::empty() const
    {
        return (impl->node_->nsDef == NULL);
    }

    void namespaces::definitions::push_back(const ns& ns)
    {
        xmlNsPtr newns = xmlNewNs (impl->node_, reinterpret_cast<const xmlChar*> (ns.get_href()), reinterpret_cast<const xmlChar*> (ns.get_prefix()));
	if (newns == NULL) throw xml::exception("creation of namespace failed");
    }

    namespaces::definitions::iterator namespaces::definitions::find (const char* prefix)
    {
        xmlNsPtr ns = xmlSearchNs (impl->node_->doc, impl->node_, reinterpret_cast<const xmlChar*> (prefix));
        return iterator(ns);
    }
    namespaces::definitions::iterator namespaces::definitions::findHref (const char* href)
    {
        xmlNsPtr ns = xmlSearchNsByHref (impl->node_->doc, impl->node_, reinterpret_cast<const xmlChar*> (href));
        return iterator(ns);
    }
    /*namespaces::definitions::iterator namespaces::definitions::erase (const namespaces::definitions::iterator& to_erase)
    {
        xmlNsPtr toeraseptr = to_erase.get_ns();
        for (xmlNsPtr runner = impl->node_->nsDef; runner != NULL; runner = runner->next)
        {
            if (runner == toeraseptr)
            {
            }
        }
    }*/
} // namespace xml
