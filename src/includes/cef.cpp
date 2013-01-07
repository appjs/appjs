#include "include/cef_app.h"
#include "includes/cef_scheme_handler.h"
#include "includes/cef_handler.h"
#include "includes/cef.h"

#define O_ENABLE(N) !initOptions->getBoolean(#N, true)
#define O_DISABLE(N) !initOptions->getBoolean(#N, false)
#define O_STRING(N) initOptions->getString(#N, (char *)"")
#define O_INTEGER(N) initOptions->getInteger(#N, 0)

extern CefRefPtr<ClientHandler> g_handler;

#if defined(__WIN__)
DWMEFICA DwmExtendFrameIntoClientArea;
DWMEBBW DwmEnableBlurBehindWindow;
DWMWP DwmDefWindowProc;
HMODULE dwmapiDLL;
#endif


namespace appjs {

bool Cef::initialized_;

void Cef::Init(Settings* initOptions) {
  if (!Cef::initialized_) {
    CefLoop::Init();
    CefRefPtr<CefApp> app;
    CefSettings o;

    CefString(&o.pack_file_path)   = O_STRING(ChromePakPath);
    CefString(&o.locales_dir_path) = O_STRING(LocalesPakPath);
    CefString(&o.log_file)         = O_STRING(LogFilePath);
    CefString(&o.cache_path)       = O_STRING(CachePath);
    CefString(&o.javascript_flags) = O_STRING(JSFlags);
    CefString(&o.user_agent)       = O_STRING(UserAgent);
    CefString(&o.product_version)  = O_STRING(ProductVersion);
    CefString(&o.locale)           = O_STRING(Locale);

    o.pack_loading_disabled        = O_ENABLE(PakLoading);
    o.multi_threaded_message_loop  = false;
    o.log_severity = (cef_log_severity_t)initOptions->getInteger("LogLevel", LOGSEVERITY_DISABLE);

    //extra_plugin_paths
    //local_storage_quota
    //session_storage_quota
    //auto_detect_proxy_settings_enabled

    CefBrowserSettings b;

    CefString(&b.standard_font_family)        =  O_STRING(StandardFont);
    CefString(&b.fixed_font_family)           =  O_STRING(FixedFont);
    CefString(&b.serif_font_family)           =  O_STRING(SerifFont);
    CefString(&b.sans_serif_font_family)      =  O_STRING(SansSerifFont);
    CefString(&b.cursive_font_family)         =  O_STRING(CursiveFont);
    CefString(&b.fantasy_font_family)         =  O_STRING(FantasyFont);
    CefString(&b.user_style_sheet_location)   =  O_STRING(UserStylesheetPath);
    CefString(&b.default_encoding)            =  O_STRING(DefaultEncoding);

    b.default_font_size                       =  O_INTEGER(DefaultFontSize);
    b.default_fixed_font_size                 =  O_INTEGER(FixedFontSize);
    b.minimum_font_size                       =  O_INTEGER(MinFontSize);
    b.minimum_logical_font_size               =  O_INTEGER(MinLogicalFontSize);

    b.site_specific_quirks_disabled           =  O_DISABLE(SiteSpecificQuirks);
    b.caret_browsing_enabled                  = !O_DISABLE(CaretBrowsing);
    b.user_style_sheet_enabled                = !O_DISABLE(UserStylesheets);
    b.accelerated_compositing_enabled         = !O_DISABLE(AcceleratedCompositing);

    b.xss_auditor_enabled                     = !O_ENABLE(XSSAuditer);
    b.hyperlink_auditing_disabled             =  O_ENABLE(HyperlinkAuditing);
    b.web_security_disabled                   =  O_ENABLE(WebSecurity);
    b.plugins_disabled                        =  O_ENABLE(Plugins);
    b.java_disabled                           =  O_ENABLE(Java);
    b.page_cache_disabled                     =  O_ENABLE(PageCache);
    b.image_load_disabled                     =  O_ENABLE(ImageLoading);
    b.encoding_detector_enabled               = !O_ENABLE(EncodingDetector);
    b.remote_fonts_disabled                   =  O_ENABLE(RemoteFonts);
    b.developer_tools_disabled                =  O_ENABLE(DevTools);
    b.tab_to_links_disabled                   =  O_ENABLE(TabToLinks);
    b.text_area_resize_disabled               =  O_ENABLE(ResizingTextareas);
    b.shrink_standalone_images_to_fit         = !O_ENABLE(FitStandaloneImages);

    b.accelerated_2d_canvas_disabled          =  O_ENABLE(Accelerated2dCanvas);
    b.accelerated_filters_disabled            =  O_ENABLE(AcceleratedFilters);
    b.accelerated_layers_disabled             =  O_ENABLE(AcceleratedLayers);
    b.accelerated_painting_disabled           =  O_ENABLE(AcceleratedPainting);
    b.accelerated_plugins_disabled            =  O_ENABLE(AcceleratedPlugins);
    b.accelerated_video_disabled              =  O_ENABLE(AcceleratedVideo);

    b.javascript_disabled                     =  O_ENABLE(JSCanRun);
    b.javascript_access_clipboard_disallowed  =  O_ENABLE(JSCanAccessClipboard);
    b.javascript_close_windows_disallowed     =  O_ENABLE(JSCanCloseWindows);
    b.javascript_open_windows_disallowed      =  O_ENABLE(JSCanOpenWindows);

    b.file_access_from_file_urls_allowed      = !O_ENABLE(FileAccessFromFileURLs);
    b.universal_access_from_file_urls_allowed = !O_ENABLE(UniversalFileURLAccess);
    b.author_and_user_styles_disabled         =  O_ENABLE(UserStyles);
    b.history_disabled                        =  O_ENABLE(HistoryAPI);
    b.application_cache_disabled              =  O_ENABLE(AppCacheAPI);
    b.databases_disabled                      =  O_ENABLE(DatabaseAPI);
    b.dom_paste_disabled                      =  O_ENABLE(DOMPasteAPI);
    b.drag_drop_disabled                      =  O_ENABLE(DragAndDropAPI);
    b.load_drops_disabled                     =  O_ENABLE(LoadDrops);
    b.fullscreen_enabled                      = !O_ENABLE(FullscreenAPI);
    b.local_storage_disabled                  =  O_ENABLE(LocalStorageAPI);
    b.webgl_disabled                          =  O_ENABLE(WebGLAPI);


    g_handler = new ClientHandler(b);
    CefInitialize(o, app);

    // http://code.google.com/p/chromiumembedded/issues/detail?id=404
    CefRegisterSchemeHandlerFactory("http", "appjs", new AppjsSchemeHandlerFactory());

#if defined(__LINUX__)
    g_thread_init(NULL);
    gdk_threads_init();
    gtk_init(NULL,NULL);
#endif

#if defined(__WIN__)
    dwmapiDLL = LoadLibrary(TEXT("dwmapi.dll"));
    if (dwmapiDLL != NULL) {
      DwmExtendFrameIntoClientArea = (DWMEFICA)GetProcAddress(dwmapiDLL, "DwmExtendFrameIntoClientArea");
      DwmEnableBlurBehindWindow = (DWMEBBW)GetProcAddress(dwmapiDLL, "DwmEnableBlurBehindWindow");
      DwmDefWindowProc = (DWMWP)GetProcAddress(dwmapiDLL, "DwmDefWindowProc");
    }

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
#endif

    Cef::initialized_ = true;
  }
}

} /* appjs */
