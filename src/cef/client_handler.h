#ifndef CEF_CLIENT_HANDLER_H
#define CEF_CLIENT_HANDLER_H
#pragma once

#include "include/cef_client.h"
#include "include/cef_task.h"
#include <assert.h>  // NOLINT(build/include_order)

#ifndef NDEBUG
#define ASSERT(condition) if (!(condition)) { assert(false); }
#else
#define ASSERT(condition) ((void)0)
#endif

#define REQUIRE_UI_THREAD()   ASSERT(CefCurrentlyOn(TID_UI));
#define REQUIRE_IO_THREAD()   ASSERT(CefCurrentlyOn(TID_IO));
#define REQUIRE_FILE_THREAD() ASSERT(CefCurrentlyOn(TID_FILE));

// ClientHandler implementation.
class ClientHandler : public CefClient
{

 public:
  ClientHandler();
  virtual ~ClientHandler();

		IMPLEMENT_REFCOUNTING(ClientHandler);
		IMPLEMENT_LOCKING(ClientHandler);
};

#endif  // CEF_TESTS_CEFCLIENT_CLIENT_HANDLER_H_
