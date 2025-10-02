/*-----------------------------------------------------------------------------
 * Umicom AuthorEngine AI (uaengine)
 * File: src/serve.c
 * PURPOSE: Tiny cross-platform static file HTTP server
 *
 * Created by: Umicom Foundation (https://umicom.foundation/)
 * Author: Sammy Hegab
 * License: MIT
 *
 * Notes for contributors:
 * - This server is intentionally tiny (no threads, no HTTP/2, no TLS).
 * - It's only used for local preview of the generated site folder.
 * - We keep it cross‑platform using WinSock on Windows and BSD sockets elsewhere.
 * - Newcomer tip: read handle_client() to see the end-to-end flow of one request.
 *---------------------------------------------------------------------------*/
#include "ueng/serve.h"
#include "ueng/common.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
typedef SOCKET ueng_socket_t;
#pragma comment(lib, "Ws2_32.lib")
#define closesock closesocket
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
typedef int ueng_socket_t;
#define closesock close
#endif

/* ---------------------------- small helpers -------------------------------- */

/* Case-insensitive equality for ASCII (kept local to this file) */
static int strequal_ci(const char *a, const char *b)
{
  if (!a || !b)
    return a == b;
  while (*a && *b)
  {
    unsigned char ca = (unsigned char)*a, cb = (unsigned char)*b;
    if (ca >= 'A' && ca <= 'Z')
      ca = (unsigned char)(ca - 'A' + 'a');
    if (cb >= 'A' && cb <= 'Z')
      cb = (unsigned char)(cb - 'A' + 'a');
    if (ca != cb)
      return 0;
    ++a;
    ++b;
  }
  return *a == '\0' && *b == '\0';
}

/* Very small traversal guard: reject any path containing ".." */
static int path_is_traversal(const char *s)
{
  if (!s)
    return 1;
  return strstr(s, "..") != NULL;
}

/* Map file extension to a Content-Type. Keep this list simple but useful. */
static const char *mime_from_ext(const char *path)
{
  const char *dot = strrchr(path, '.');
  if (!dot || !dot[1])
    return "application/octet-stream";
  const char *ext = dot + 1;

  if (strequal_ci(ext, "html") || strequal_ci(ext, "htm"))
    return "text/html; charset=utf-8";
  if (strequal_ci(ext, "css"))
    return "text/css; charset=utf-8";
  if (strequal_ci(ext, "js") || strequal_ci(ext, "mjs"))
    return "application/javascript; charset=utf-8";
  if (strequal_ci(ext, "json"))
    return "application/json; charset=utf-8";

  if (strequal_ci(ext, "svg"))
    return "image/svg+xml";
  if (strequal_ci(ext, "png"))
    return "image/png";
  if (strequal_ci(ext, "jpg") || strequal_ci(ext, "jpeg"))
    return "image/jpeg";
  if (strequal_ci(ext, "gif"))
    return "image/gif";
  if (strequal_ci(ext, "webp"))
    return "image/webp";
  if (strequal_ci(ext, "ico"))
    return "image/x-icon";

  if (strequal_ci(ext, "woff"))
    return "font/woff";
  if (strequal_ci(ext, "woff2"))
    return "font/woff2";
  if (strequal_ci(ext, "ttf"))
    return "font/ttf";
  if (strequal_ci(ext, "otf"))
    return "font/otf";

  if (strequal_ci(ext, "map"))
    return "application/json; charset=utf-8";
  if (strequal_ci(ext, "pdf"))
    return "application/pdf";

  if (strequal_ci(ext, "txt") || strequal_ci(ext, "md"))
    return "text/plain; charset=utf-8";
  return "application/octet-stream";
}

/* Compose and send a small text response (e.g., 404). */
static void http_send_simple(ueng_socket_t cs, const char *status, const char *body)
{
  char hdr[512];
  size_t blen = body ? strlen(body) : 0;
  int n = snprintf(hdr, sizeof(hdr),
                   "HTTP/1.1 %s\r\n"
                   "Content-Type: text/plain; charset=utf-8\r\n"
                   "Content-Length: %zu\r\n"
                   "Connection: close\r\n\r\n",
                   status, blen);
#ifdef _WIN32
  send(cs, hdr, n, 0);
  if (body && blen)
    send(cs, body, (int)blen, 0);
#else
  send(cs, hdr, (size_t)n, 0);
  if (body && blen)
    send(cs, body, blen, 0);
#endif
}

/* Send a 200 response header before streaming a file body. */
static void http_send_200_head(ueng_socket_t cs, const char *mime, size_t len)
{
  char hdr[512];
  int n = snprintf(hdr, sizeof(hdr),
                   "HTTP/1.1 200 OK\r\n"
                   "Content-Type: %s\r\n"
                   "Content-Length: %zu\r\n"
                   "Connection: close\r\n\r\n",
                   mime, len);
#ifdef _WIN32
  send(cs, hdr, n, 0);
#else
  send(cs, hdr, (size_t)n, 0);
#endif
}

/* Stream a file to the client; head_only skips the body for HEAD requests. */
static int send_file(ueng_socket_t cs, const char *fs_path, const char *mime, int head_only)
{
  FILE *f = ueng_fopen(fs_path, "rb");
  if (!f)
  {
    http_send_simple(cs, "404 Not Found", "404 Not Found\n");
    return -1;
  }

  fseek(f, 0, SEEK_END);
  long sz = ftell(f);
  if (sz < 0)
    sz = 0;
  fseek(f, 0, SEEK_SET);

  http_send_200_head(cs, mime, (size_t)sz);
  if (!head_only)
  {
    char buf[16 * 1024];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf), f)) > 0)
    {
#ifdef _WIN32
      int sent = send(cs, (const char *)buf, (int)n, 0);
      if (sent <= 0)
        break;
#else
      ssize_t sent = send(cs, (const char *)buf, n, 0);
      if (sent <= 0)
        break;
#endif
    }
  }
  fclose(f);
  return 0;
}

/* -------------------------------- core ------------------------------------- */

/* Handle a single HTTP/1.1 GET/HEAD request from socket cs. */
static void handle_client(ueng_socket_t cs, const char *root)
{
  char req[4096];
#ifdef _WIN32
  int rn = recv(cs, req, (int)sizeof(req) - 1, 0);
#else
  ssize_t rn = recv(cs, req, sizeof(req) - 1, 0);
#endif
  if (rn <= 0)
  {
    closesock(cs);
    return;
  }
  req[rn] = '\0';

  /* Minimal parse: METHOD PATH HTTP/x.y  (we ignore headers for now) */
  char method[8] = {0};
  char path[2048] = {0};
  char httpver[16] = {0};
  if (sscanf(req, "%7s %2047s %15s", method, path, httpver) != 3)
  {
    http_send_simple(cs, "400 Bad Request", "400 Bad Request\n");
    closesock(cs);
    return;
  }

  /* Allow only GET and HEAD for this tiny static server */
  int head_only = 0;
  if (strcmp(method, "GET") == 0)
  {
    head_only = 0;
  }
  else if (strcmp(method, "HEAD") == 0)
  {
    head_only = 1;
  }
  else
  {
    http_send_simple(cs, "405 Method Not Allowed", "405 Method Not Allowed\n");
    closesock(cs);
    return;
  }

  /* Guard against directory traversal (../) */
  if (path_is_traversal(path))
  {
    http_send_simple(cs, "400 Bad Request", "400 Bad Request\n");
    closesock(cs);
    return;
  }

  /* Build a filesystem path from root + requested path */
  char fs_path[PATH_MAX];

  if (strcmp(path, "/") == 0)
  {
    /* Root → serve index.html */
    snprintf(fs_path, sizeof(fs_path), "%s%cindex.html", root, PATH_SEP);
  }
  else
  {
    /* Strip leading '/' and normalize separators */
    const char *p = (path[0] == '/') ? (path + 1) : path;
    char rel[PATH_MAX];
    size_t j = 0;
    for (size_t i = 0; p[i] && j < sizeof(rel) - 1; ++i)
    {
      char c = p[i];
      if (c == '/')
      {
        c = PATH_SEP;
      }
      rel[j++] = c;
    }
    rel[j] = '\0';
    snprintf(fs_path, sizeof(fs_path), "%s%c%s", root, PATH_SEP, rel);
  }

  /* If not a file but a directory, try directory index fallback */
  if (!file_exists(fs_path) && dir_exists(fs_path))
  {
    char candidate[PATH_MAX];
    snprintf(candidate, sizeof(candidate), "%s%cindex.html", fs_path, PATH_SEP);
    if (file_exists(candidate))
    {
      strcpy(fs_path, candidate);
    }
  }

  /* If still not found, 404 */
  if (!file_exists(fs_path))
  {
    http_send_simple(cs, "404 Not Found", "404 Not Found\n");
    closesock(cs);
    return;
  }

  /* Pick a simple content type from extension and stream the file */
  const char *mime = mime_from_ext(fs_path);
  send_file(cs, fs_path, mime, head_only);
  closesock(cs);
}

/* Public entry point: start a blocking HTTP loop that serves 'root'. */
int serve_run(const char *root, const char *host, int port)
{
  if (!root || !*root)
  {
    return 1;
  }
  if (!dir_exists(root))
  {
    fprintf(stderr, "[serve] ERROR: site root not found: %s\n", root);
    return 1;
  }

#ifdef _WIN32
  WSADATA wsa;
  if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
  {
    fprintf(stderr, "[serve] ERROR: WSAStartup failed\n");
    return 1;
  }
#else
  /* Reset default SIGINT/SIGTERM handling so Ctrl+C stops the process. */
  signal(SIGINT, SIG_DFL);
  signal(SIGTERM, SIG_DFL);
#endif

  ueng_socket_t s = (ueng_socket_t)socket(AF_INET, SOCK_STREAM, 0);
  if (s < 0)
  {
    fprintf(stderr, "[serve] ERROR: socket(): %s\n", strerror(errno));
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }

  int on = 1;
#ifdef _WIN32
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, (const char *)&on, sizeof(on));
#else
  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
#endif

  struct sockaddr_in addr;
  memset(&addr, 0, sizeof(addr));
  addr.sin_family = AF_INET;
  addr.sin_port = htons((unsigned short)port);
  if (inet_pton(AF_INET, host, &addr.sin_addr) != 1)
  {
    fprintf(stderr, "[serve] ERROR: invalid host: %s\n", host);
    closesock(s);
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }

  if (bind(s, (struct sockaddr *)&addr, sizeof(addr)) < 0)
  {
    fprintf(stderr, "[serve] ERROR: bind(): %s\n", strerror(errno));
    closesock(s);
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }
  if (listen(s, 16) < 0)
  {
    fprintf(stderr, "[serve] ERROR: listen(): %s\n", strerror(errno));
    closesock(s);
#ifdef _WIN32
    WSACleanup();
#endif
    return 1;
  }

  printf("[serve] Serving %s at http://%s:%d (Ctrl+C to stop)\n", root, host, port);

  for (;;)
  {
    struct sockaddr_in cli;
    socklen_t clen = (socklen_t)sizeof(cli);
    ueng_socket_t cs = (ueng_socket_t)accept(s, (struct sockaddr *)&cli, &clen);
    if (cs < 0)
    {
      /* Transient accept error; keep serving. */
      continue;
    }
    handle_client(cs, root);
  }

  /* Unreachable in normal flow */
  closesock(s);
#ifdef _WIN32
  WSACleanup();
#endif
  return 0;
}
