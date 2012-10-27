// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// This file is shared by cefclient and cef_unittests so don't include using
// a qualified path.
#include "client_app.h"  // NOLINT(build/include)
#include "appjs.h"
#include <string>

#include "include/cef_cookie.h"
#include "include/cef_process_message.h"
#include "include/cef_task.h"
#include "include/cef_v8.h"

// Forward declarations.
void SetList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target);
void SetList(CefRefPtr<CefListValue> source, CefRefPtr<CefV8Value> target);

// Transfer a V8 value to a List index.
void SetListValue(CefRefPtr<CefListValue> list, int index,
                  CefRefPtr<CefV8Value> value) {
  if (value->IsArray()) {
    CefRefPtr<CefListValue> new_list = CefListValue::Create();
    SetList(value, new_list);
    list->SetList(index, new_list);
  } else if (value->IsString()) {
    list->SetString(index, value->GetStringValue());
  } else if (value->IsBool()) {
    list->SetBool(index, value->GetBoolValue());
  } else if (value->IsInt()) {
    list->SetInt(index, value->GetIntValue());
  } else if (value->IsDouble()) {
    list->SetDouble(index, value->GetDoubleValue());
  }
}

// Transfer a V8 array to a List.
void SetList(CefRefPtr<CefV8Value> source, CefRefPtr<CefListValue> target) {
  ASSERT(source->IsArray());

  int arg_length = source->GetArrayLength();
  if (arg_length == 0)
    return;

  // Start with null types in all spaces.
  target->SetSize(arg_length);

  for (int i = 0; i < arg_length; ++i)
    SetListValue(target, i, source->GetValue(i));
}

// Transfer a List value to a V8 array index.
void SetListValue(CefRefPtr<CefV8Value> list, int index,
                  CefRefPtr<CefListValue> value) {
  CefRefPtr<CefV8Value> new_value;

  CefValueType type = value->GetType(index);
  switch (type) {
    case VTYPE_LIST: {
      CefRefPtr<CefListValue> list = value->GetList(index);
      new_value = CefV8Value::CreateArray(list->GetSize());
      SetList(list, new_value);
      } break;
    case VTYPE_BOOL:
      new_value = CefV8Value::CreateBool(value->GetBool(index));
      break;
    case VTYPE_DOUBLE:
      new_value = CefV8Value::CreateDouble(value->GetDouble(index));
      break;
    case VTYPE_INT:
      new_value = CefV8Value::CreateInt(value->GetInt(index));
      break;
    case VTYPE_STRING:
      new_value = CefV8Value::CreateString(value->GetString(index));
      break;
    default:
      break;
  }

  if (new_value.get()) {
    list->SetValue(index, new_value);
  } else {
    list->SetValue(index, CefV8Value::CreateNull());
  }
}

// Transfer a List to a V8 array.
void SetList(CefRefPtr<CefListValue> source, CefRefPtr<CefV8Value> target) {
  ASSERT(target->IsArray());

  int arg_length = source->GetSize();
  if (arg_length == 0)
    return;

  for (int i = 0; i < arg_length; ++i)
    SetListValue(target, i, source);
}

namespace appjs {
ClientApp::ClientApp()
    : proxy_type_(CEF_PROXY_TYPE_DIRECT) {
  CreateRenderDelegates(render_delegates_);

  // Default schemes that support cookies.
  cookieable_schemes_.push_back("http");
  cookieable_schemes_.push_back("https");
}

void ClientApp::OnContextInitialized() {
  // Register cookieable schemes with the global cookie manager.
  CefRefPtr<CefCookieManager> manager = CefCookieManager::GetGlobalManager();
  ASSERT(manager.get());
  manager->SetSupportedSchemes(cookieable_schemes_);
}

void ClientApp::GetProxyForUrl(const CefString& url,
                               CefProxyInfo& proxy_info) {
  proxy_info.proxyType = proxy_type_;
  if (!proxy_config_.empty())
    CefString(&proxy_info.proxyList) = proxy_config_;
}

void ClientApp::OnRenderThreadCreated() {
  // Execute delegate callbacks.
  fprintf(stderr, "%s\n", "renderer created");
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnRenderThreadCreated(this);
}

void ClientApp::OnWebKitInitialized() {
  // Execute delegate callbacks.
  fprintf(stderr, "%s\n", "webkit init");
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnWebKitInitialized(this);
}

void ClientApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                               CefRefPtr<CefFrame> frame,
                               CefRefPtr<CefV8Context> context) {
  // Execute delegate callbacks.
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextCreated(this, browser, frame, context);
}

void ClientApp::OnContextReleased(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) {
  // Execute delegate callbacks.
  fprintf(stderr, "%s\n", "context released");
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnContextReleased(this, browser, frame, context);

  // Remove any JavaScript callbacks registered for the context that has been
  // released.
  if (!callback_map_.empty()) {
    CallbackMap::iterator it = callback_map_.begin();
    for (; it != callback_map_.end();) {
      if (it->second.first->IsSame(context))
        callback_map_.erase(it++);
      else
        ++it;
    }
  }
}

void ClientApp::OnFocusedNodeChanged(CefRefPtr<CefBrowser> browser,
                                     CefRefPtr<CefFrame> frame,
                                     CefRefPtr<CefDOMNode> node) {
  // Execute delegate callbacks.
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end(); ++it)
    (*it)->OnFocusedNodeChanged(this, browser, frame, node);
}

bool ClientApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  ASSERT(source_process == PID_BROWSER);
fprintf(stderr, "%s\n", "process message");
  bool handled = false;

  // Execute delegate callbacks.
  RenderDelegateSet::iterator it = render_delegates_.begin();
  for (; it != render_delegates_.end() && !handled; ++it) {
    handled = (*it)->OnProcessMessageReceived(this, browser, source_process,
                                              message);
  }

  if (handled)
    return true;

  // Execute the registered JavaScript callback if any.
  if (!callback_map_.empty()) {
    CefString message_name = message->GetName();
    CallbackMap::const_iterator it = callback_map_.find(
        std::make_pair(message_name.ToString(),
                       browser->GetIdentifier()));
    if (it != callback_map_.end()) {
      // Enter the context.
      it->second.first->Enter();

      CefV8ValueList arguments;

      // First argument is the message name.
      arguments.push_back(CefV8Value::CreateString(message_name));

      // Second argument is the list of message arguments.
      CefRefPtr<CefListValue> list = message->GetArgumentList();
      CefRefPtr<CefV8Value> args = CefV8Value::CreateArray(list->GetSize());
      SetList(list, args);
      arguments.push_back(args);

      // Execute the callback.
      CefRefPtr<CefV8Value> retval =
          it->second.second->ExecuteFunction(NULL, arguments);
      if (retval.get()) {
        if (retval->IsBool())
          handled = retval->GetBoolValue();
      }

      // Exit the context.
      it->second.first->Exit();
    }
  }

  return handled;
}

};