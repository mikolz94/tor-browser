/* -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 4 -*-
 *
 * The contents of this file are subject to the Netscape Public License
 * Version 1.0 (the "NPL"); you may not use this file except in
 * compliance with the NPL.  You may obtain a copy of the NPL at
 * http://www.mozilla.org/NPL/
 *
 * Software distributed under the NPL is distributed on an "AS IS" basis,
 * WITHOUT WARRANTY OF ANY KIND, either express or implied. See the NPL
 * for the specific language governing rights and limitations under the
 * NPL.
 *
 * The Initial Developer of this code under the NPL is Netscape
 * Communications Corporation.  Portions created by Netscape are
 * Copyright (C) 1998 Netscape Communications Corporation.  All Rights
 * Reserved.
 */

#include "nsFileChannel.h"
#include "nsFileProtocolHandler.h"
#include "nsIURL.h"
#include "nsCRT.h"
#include "nsIComponentManager.h"
#include "nsIServiceManager.h"
#include "nsIEventSinkGetter.h"
#include "nsIProgressEventSink.h"

static NS_DEFINE_CID(kStandardURLCID,            NS_STANDARDURL_CID);

////////////////////////////////////////////////////////////////////////////////

nsFileProtocolHandler::nsFileProtocolHandler() {
    NS_INIT_REFCNT();
}

nsFileProtocolHandler::~nsFileProtocolHandler() {
}

NS_IMPL_ISUPPORTS(nsFileProtocolHandler, nsIProtocolHandler::GetIID());

NS_METHOD
nsFileProtocolHandler::Create(nsISupports *aOuter, REFNSIID aIID, void **aResult)
{
    if (aOuter)
        return NS_ERROR_NO_AGGREGATION;

    nsFileProtocolHandler* ph = new nsFileProtocolHandler();
    if (ph == nsnull)
        return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(ph);
    nsresult rv = ph->QueryInterface(aIID, aResult);
    NS_RELEASE(ph);
    return rv;
}

////////////////////////////////////////////////////////////////////////////////
// nsIProtocolHandler methods:

NS_IMETHODIMP
nsFileProtocolHandler::GetScheme(char* *result)
{
    *result = nsCRT::strdup("ftp");
    if (*result == nsnull)
        return NS_ERROR_OUT_OF_MEMORY;
    return NS_OK;
}

NS_IMETHODIMP
nsFileProtocolHandler::GetDefaultPort(PRInt32 *result)
{
    *result = 21; 
    return NS_OK;
}

NS_IMETHODIMP
nsFileProtocolHandler::MakeAbsolute(const char* aSpec,
                                   nsIURI* aBaseURI,
                                   char* *result)
{
    // XXX optimize this to not needlessly construct the URL

    nsresult rv;
    nsIURI* url;
    rv = NewURI(aSpec, aBaseURI, &url);
    if (NS_FAILED(rv)) return rv;

    rv = url->GetSpec(result);
    NS_RELEASE(url);
    return rv;
}

NS_IMETHODIMP
nsFileProtocolHandler::NewURI(const char *aSpec, nsIURI *aBaseURI,
                             nsIURI **result)
{
    nsresult rv;

    // Ftp URLs (currently) have no additional structure beyond that provided by standard
    // URLs, so there is no "outer" given to CreateInstance 

    nsIURI* url;
    if (aBaseURI) {
        rv = aBaseURI->Clone(&url);
    }
    else {
        rv = nsComponentManager::CreateInstance(kStandardURLCID, nsnull,
                                                nsIURI::GetIID(),
                                                (void**)&url);
    }
    if (NS_FAILED(rv)) return rv;

    rv = url->SetSpec((char*)aSpec);
    if (NS_FAILED(rv)) {
        NS_RELEASE(url);
        return rv;
    }

    // XXX this is the default port for ftp. we need to strip out the actual
    // XXX requested port.
    rv = url->SetPort(21);
    if (NS_FAILED(rv)) {
        NS_RELEASE(url);
        return rv;
    }

    *result = url;
    return rv;
}

NS_IMETHODIMP
nsFileProtocolHandler::NewChannel(const char* verb, nsIURI* url,
                                 nsIEventSinkGetter* eventSinkGetter,
                                 nsIEventQueue* eventQueue,
                                 nsIChannel* *result)
{
    nsresult rv;

    nsIProgressEventSink* eventSink;
    rv = eventSinkGetter->GetEventSink(verb, nsIProgressEventSink::GetIID(), 
                                       (nsISupports**)&eventSink);
    if (NS_FAILED(rv)) return rv;

    nsFileChannel* channel = new nsFileChannel();
    if (channel == nsnull)
        return NS_ERROR_OUT_OF_MEMORY;
    NS_ADDREF(channel);

    rv = channel->Init(url, eventSinkGetter, eventQueue);
    if (NS_FAILED(rv)) {
        NS_RELEASE(channel);
        return rv;
    }

    *result = channel;
    return NS_OK;
}

////////////////////////////////////////////////////////////////////////////////
