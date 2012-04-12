// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "cef/client_handler.h"
#include "include/cef_browser.h"
#include "include/cef_frame.h"

ClientHandler::ClientHandler()
{
}

ClientHandler::~ClientHandler() {
}

CefRefPtr<ClientHandler> g_handler;
