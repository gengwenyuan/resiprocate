#if defined(HAVE_CONFIG_H)
#include "resiprocate/config.hxx"
#endif

#include "resiprocate/os/compat.hxx"
#include "resiprocate/os/Logger.hxx"

#include "resiprocate/DnsInterface.hxx"
#include "resiprocate/DnsHandler.hxx"
#include "resiprocate/DnsResult.hxx"

using namespace resip;

#define RESIPROCATE_SUBSYSTEM resip::Subsystem::DNS

#if !defined(USE_ARES)
#error Must have ARES
#endif

const Data DnsInterface::UdpOnly[] =
{
   Data("SIP+D2U"),
   Data::Empty
};

const Data DnsInterface::TcpAndUdp[] =
{
   Data("SIP+D2T"),
   Data("SIP+D2U"),
   Data::Empty
};

const Data DnsInterface::AllTransports[] =
{
   Data("SIPS+D2T"),
   Data("SIP+D2T"),
   Data("SIP+D2U"),
   Data::Empty
};

DnsInterface::DnsInterface(bool sync)
   : mSupportedTransports(&TcpAndUdp)
{
   assert(sync == false);
   int status=0;
   if ((status = ares_init(&mChannel)) != ARES_SUCCESS)
   {
      ErrLog (<< "Failed to initialize async dns library (ares)");
      char* errmem=0;
      ErrLog (<< ares_strerror(status, &errmem));
      ares_free_errmem(errmem);
      throw Exception("failed to initialize ares", __FILE__,__LINE__);
   }
}

DnsInterface::~DnsInterface()
{
   ares_destroy(mChannel);
}

void 
DnsInterface::setSupportedTransports(TransportArray& transports)
{
   mSupportedTransports = &transports;
}

bool
DnsInterface::isSupported(const Data& service)
{
   for (int i=0; !mSupportedTransports[i]->empty(); i++)
   {
      if (service == (*mSupportedTransports)[i]) 
      {
         return true;
      }
   }
   return false;
}

void 
DnsInterface::buildFdSet(FdSet& fdset)
{
#if defined(USE_ARES)
   int size = ares_fds(mChannel, &fdset.read, &fdset.write);
   if ( size > fdset.size )
   {
      fdset.size = size;
   }
#endif
}

void 
DnsInterface::process(FdSet& fdset)
{
#if defined(USE_ARES)
   ares_process(mChannel, &fdset.read, &fdset.write);
#endif
}


DnsResult*
DnsInterface::lookup(const Uri& uri, DnsHandler* handler)
{
   DnsResult* result = new DnsResult(*this, handler);
   result->lookup(uri);
   return result;
}

DnsResult* 
DnsInterface::lookup(const Via& via, DnsHandler* handler)
{
   assert(0);
   //DnsResult* result = new DnsResult(*this);
   return NULL;
}

DnsHandler::~DnsHandler()
{
}

//  Copyright (c) 2003, Jason Fischl 
/* ====================================================================
 * The Vovida Software License, Version 1.0 
 * 
 * Copyright (c) 2000 Vovida Networks, Inc.  All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 
 * 3. The names "VOCAL", "Vovida Open Communication Application Library",
 *    and "Vovida Open Communication Application Library (VOCAL)" must
 *    not be used to endorse or promote products derived from this
 *    software without prior written permission. For written
 *    permission, please contact vocal@vovida.org.
 *
 * 4. Products derived from this software may not be called "VOCAL", nor
 *    may "VOCAL" appear in their name, without prior written
 *    permission of Vovida Networks, Inc.
 * 
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, TITLE AND
 * NON-INFRINGEMENT ARE DISCLAIMED.  IN NO EVENT SHALL VOVIDA
 * NETWORKS, INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT DAMAGES
 * IN EXCESS OF $1,000, NOR FOR ANY INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 * 
 * ====================================================================
 * 
 * This software consists of voluntary contributions made by Vovida
 * Networks, Inc. and many individuals on behalf of Vovida Networks,
 * Inc.  For more information on Vovida Networks, Inc., please see
 * <http://www.vovida.org/>.
 *
 */

