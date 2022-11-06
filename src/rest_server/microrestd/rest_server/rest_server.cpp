// This file is part of MicroRestD <http://github.com/ufal/microrestd/>.
//
// Copyright 2015 Institute of Formal and Applied Linguistics, Faculty of
// Mathematics and Physics, Charles University in Prague, Czech Republic.
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.

#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <memory>
#include <thread>

#if defined(_WIN32) && !defined(__CYGWIN__)
#include <condition_variable>
#include <mutex>
#include <ws2tcpip.h>
#include <windows.h>
#define MHD_socket_close(fd) closesocket((fd))
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <unistd.h>
#define MHD_socket_close(fd) close((fd))
#endif

#include "response_generator.h"
#include "rest_server.h"
#include "../libmicrohttpd/microhttpd.h"

namespace ufal {
namespace microrestd {

using namespace libmicrohttpd;
using namespace std;

class MHD_ResponseDeleter {
 public:
  void operator()(MHD_Response* response) {
    MHD_destroy_response(response);
  }
};

class MHD_PostProcessorDeleter {
 public:
  void operator()(MHD_PostProcessor* post_processor) {
    MHD_destroy_post_processor(post_processor);
  }
};

// Class rest_server::microhttpd_request
class rest_server::microhttpd_request : public rest_request {
 public:
  static bool initialize();

  microhttpd_request(const rest_server& server, MHD_Connection* connection, const char* url, const char* content_type, const char* method);

  int handle(rest_service* service);
  bool process_request_body(const char* request_body, size_t request_body_len);

  const sockaddr* address() const;
  const char* forwarded_for() const;

  virtual bool respond(const char* content_type, string_piece body,
                       const std::vector<std::pair<const char*, const char*>>& headers = {}) override;
  virtual bool respond(const char* content_type, response_generator* generator,
                       const std::vector<std::pair<const char*, const char*>>& headers = {}) override;
  virtual bool respond_not_found() override;
  virtual bool respond_method_not_allowed(const char* comma_separated_allowed_methods) override;
  virtual bool respond_error(string_piece error, int code = 400) override;

 private:
  const rest_server& server;
  MHD_Connection* connection;

  unique_ptr<MHD_PostProcessor, MHD_PostProcessorDeleter> post_processor;
  bool need_post_processor;
  bool unsupported_multipart_encoding;
  unsigned remaining_request_body_size;

  unique_ptr<response_generator> generator;
  bool generator_end;
  unsigned generator_offset;

  static MHD_Response* create_response(string_piece data, const char* content_type,
                                       const std::vector<std::pair<const char*, const char*>>& headers = {});
  static MHD_Response* create_generator_response(microhttpd_request* request, const char* content_type,
                                                 const std::vector<std::pair<const char*, const char*>>& headers = {});
  static MHD_Response* create_plain_permanent_response(const string& data);
  static void response_headers(unique_ptr<MHD_Response, MHD_ResponseDeleter>& response, const char* content_type,
                               const std::vector<std::pair<const char*, const char*>>& headers = {});

  static int get_iterator(void* cls, MHD_ValueKind kind, const char* key, const char* value);
  static int post_iterator(void* cls, MHD_ValueKind kind, const char* key, const char* filename, const char* content_type, const char* transfer_encoding, const char* data, uint64_t off, size_t size);
  static ssize_t generator_callback(void* cls, uint64_t pos, char* buf, size_t max);

  static bool valid_utf8(const string& text);

  static bool http_value_compare(const char* string, const char* pattern);

  static unique_ptr<MHD_Response, MHD_ResponseDeleter> response_not_allowed, response_not_found, response_too_large, response_unsupported_multipart_encoding, response_invalid_utf8;
};
unique_ptr<MHD_Response, MHD_ResponseDeleter> rest_server::microhttpd_request::response_not_allowed,
                                              rest_server::microhttpd_request::response_not_found,
                                              rest_server::microhttpd_request::response_too_large,
                                              rest_server::microhttpd_request::response_unsupported_multipart_encoding,
                                              rest_server::microhttpd_request::response_invalid_utf8;

rest_server::microhttpd_request::microhttpd_request(const rest_server& server, MHD_Connection* connection, const char* url, const char* content_type, const char* method)
  : server(server), connection(connection), unsupported_multipart_encoding(false), remaining_request_body_size(server.max_request_body_size + 1) {
  // Initialize rest_request fields
  this->url = url;
  this->method = method;
  this->content_type = content_type;

  // Create post processor if needed
  need_post_processor = this->method == MHD_HTTP_METHOD_POST &&
      (http_value_compare(content_type, MHD_HTTP_POST_ENCODING_FORM_URLENCODED) ||
       http_value_compare(content_type, MHD_HTTP_POST_ENCODING_MULTIPART_FORMDATA));
  if (need_post_processor) {
    post_processor.reset(MHD_create_post_processor(connection, 32 << 10, &post_iterator, this));
    if (!post_processor) cerr << "Cannot allocate new post processor!" << endl;
  }

  // Collect GET arguments
  MHD_get_connection_values(connection, MHD_GET_ARGUMENT_KIND, get_iterator, this);
}

bool rest_server::microhttpd_request::initialize() {
  static string not_allowed = "Requested method is not allowed.\n";
  static string not_found = "Requested URL was not found.\n";
  static string too_large = "Request was too large.\n";
  static string unsupported_multipart_encoding = "Unsupported transfer-encoding of multipart/form-data POST request part. Currently only 7bit, 8bit or binary is supported.\n";
  static string invalid_utf8 = "The request arguments are not valid UTF-8.\n";

  response_not_allowed.reset(create_plain_permanent_response(not_allowed));
  if (!response_not_allowed) return false;
  if (MHD_add_response_header(response_not_allowed.get(), MHD_HTTP_HEADER_ALLOW, "HEAD, GET, POST, PUT, DELETE") != MHD_YES) return false;

  response_not_found.reset(create_plain_permanent_response(not_found));
  if (!response_not_found) return false;

  response_too_large.reset(create_plain_permanent_response(too_large));
  if (!response_too_large) return false;

  response_unsupported_multipart_encoding.reset(create_plain_permanent_response(unsupported_multipart_encoding));
  if (!response_unsupported_multipart_encoding) return false;

  response_invalid_utf8.reset(create_plain_permanent_response(invalid_utf8));
  if (!response_invalid_utf8) return false;

  return true;
}

int rest_server::microhttpd_request::handle(rest_service* service) {
  // Check that method is supported
  if (method != MHD_HTTP_METHOD_HEAD && method != MHD_HTTP_METHOD_GET && method != MHD_HTTP_METHOD_POST && method != MHD_HTTP_METHOD_PUT && method != MHD_HTTP_METHOD_DELETE)
    return MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response_not_allowed.get());

  // Close post_processor if exists
  if (post_processor) post_processor.reset();

  // Was the multipart/form-data transfer-encoding supported
  if (unsupported_multipart_encoding)
    return MHD_queue_response(connection, MHD_HTTP_UNSUPPORTED_MEDIA_TYPE, response_unsupported_multipart_encoding.get());

  // Was the request too large?
  if (server.max_request_body_size && !remaining_request_body_size)
    return MHD_queue_response(connection, MHD_HTTP_REQUEST_ENTITY_TOO_LARGE, response_too_large.get());

  // Are all arguments legal utf-8?
  for (auto&& param : params)
    if (!valid_utf8(param.first) || !valid_utf8(param.second))
      return MHD_queue_response(connection, MHD_HTTP_UNSUPPORTED_MEDIA_TYPE, response_invalid_utf8.get());

  // Let the service handle the request and respond with one of the respond_* methods.
  return service->handle(*this) ? MHD_YES : MHD_NO;
}

bool rest_server::microhttpd_request::process_request_body(const char* request_body, size_t request_body_len) {
  if (!server.max_request_body_size || remaining_request_body_size > request_body_len) {
    if (need_post_processor) {
      if (!post_processor || MHD_post_process(post_processor.get(), request_body, request_body_len) != MHD_YES)
        return false;
    } else {
      body.append(request_body, request_body_len);
    }
    if (server.max_request_body_size) remaining_request_body_size -= request_body_len;
  } else {
    remaining_request_body_size = 0;
  }
  return true;
}

const sockaddr* rest_server::microhttpd_request::address() const {
  auto info = MHD_get_connection_info(connection, MHD_CONNECTION_INFO_CLIENT_ADDRESS);
  return info ? info->client_addr : nullptr;
}

const char* rest_server::microhttpd_request::forwarded_for() const {
  return MHD_lookup_connection_value(connection, MHD_HEADER_KIND, "X-Forwarded-For");
}

bool rest_server::microhttpd_request::respond(const char* content_type, string_piece body,
                                              const std::vector<std::pair<const char*, const char*>>& headers) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(create_response(body, content_type, headers));
  if (!response) return false;
  return MHD_queue_response(connection, MHD_HTTP_OK, response.get()) == MHD_YES;
}

bool rest_server::microhttpd_request::respond(const char* content_type, response_generator* generator,
                                              const std::vector<std::pair<const char*, const char*>>& headers) {
  this->generator.reset(generator);
  this->generator_end = false;
  this->generator_offset = 0;
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(create_generator_response(this, content_type, headers));
  if (!response) return false;
  return MHD_queue_response(connection, MHD_HTTP_OK, response.get()) == MHD_YES;
}

bool rest_server::microhttpd_request::respond_not_found() {
  return MHD_queue_response(connection, MHD_HTTP_NOT_FOUND, response_not_found.get()) == MHD_YES;
}

bool rest_server::microhttpd_request::respond_method_not_allowed(const char* comma_separated_allowed_methods) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(create_response("Requested method is not allowed.\n", "text/plain"));
  if (!response) return false;
  if (MHD_add_response_header(response.get(), MHD_HTTP_HEADER_ALLOW, comma_separated_allowed_methods) != MHD_YES) return response.reset(), false;
  return MHD_queue_response(connection, MHD_HTTP_METHOD_NOT_ALLOWED, response.get()) == MHD_YES;
}

bool rest_server::microhttpd_request::respond_error(string_piece error, int code) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(create_response(error, "text/plain"));
  if (!response) return false;
  return MHD_queue_response(connection, code, response.get()) == MHD_YES;
}

MHD_Response* rest_server::microhttpd_request::create_plain_permanent_response(const string& data) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(MHD_create_response_from_buffer(data.size(), (void*) data.c_str(), MHD_RESPMEM_PERSISTENT));
  response_headers(response, "text/plain");
  return response.release();
}

MHD_Response* rest_server::microhttpd_request::create_response(string_piece data, const char* content_type,
                                                               const std::vector<std::pair<const char*, const char*>>& headers) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(MHD_create_response_from_buffer(data.len, (void*) data.str, MHD_RESPMEM_MUST_COPY));
  response_headers(response, content_type, headers);
  return response.release();
}

MHD_Response* rest_server::microhttpd_request::create_generator_response(microhttpd_request* request, const char* content_type,
                                                                         const std::vector<std::pair<const char*, const char*>>& headers) {
  unique_ptr<MHD_Response, MHD_ResponseDeleter> response(MHD_create_response_from_callback(-1, 32 << 10, generator_callback, request, nullptr));
  response_headers(response, content_type, headers);
  return response.release();
}

void rest_server::microhttpd_request::response_headers(unique_ptr<MHD_Response, MHD_ResponseDeleter>& response, const char* content_type,
                                                       const std::vector<std::pair<const char*, const char*>>& headers) {
  if (!response) return;
  if (MHD_add_response_header(response.get(), MHD_HTTP_HEADER_CONTENT_TYPE, content_type) != MHD_YES ||
      MHD_add_response_header(response.get(), MHD_HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN, "*") != MHD_YES ||
      MHD_add_response_header(response.get(), MHD_HTTP_HEADER_CONNECTION, "close") != MHD_YES) {
    response.reset();
    return;
  }
  for (auto&& header : headers)
    if (response && MHD_add_response_header(response.get(), header.first, header.second) != MHD_YES) {
      response.reset();
      return;
    }
}


int rest_server::microhttpd_request::get_iterator(void* cls, MHD_ValueKind kind, const char* key, const char* value) {
  auto self = (microhttpd_request*) cls;
  if (kind == MHD_GET_ARGUMENT_KIND && key)
    self->params.emplace(key, value ? value : string());

  return MHD_YES;
}

int rest_server::microhttpd_request::post_iterator(void* cls, MHD_ValueKind kind, const char* key, const char* /*filename*/, const char* /*content_type*/, const char* transfer_encoding, const char* data, uint64_t off, size_t size) {
  auto self = (microhttpd_request*) cls;
  if (kind == MHD_POSTDATA_KIND && key && !self->unsupported_multipart_encoding) {
    // Check that transfer_encoding is supported
    if (transfer_encoding && !(http_value_compare(transfer_encoding, "binary") ||
                               http_value_compare(transfer_encoding, "7bit") ||
                               http_value_compare(transfer_encoding, "8bit"))) {
      self->unsupported_multipart_encoding = true;
    } else {
      string& value = self->params[key];
      if (!off) value.clear();
      if (size) value.append(data, size);
    }
  }

  return MHD_YES;
}

ssize_t rest_server::microhttpd_request::generator_callback(void* cls, uint64_t /*pos*/, char* buf, size_t max) {
  auto request = (microhttpd_request*) cls;
  string_piece data = request->generator->current();
  unsigned minimum = request->server.min_generated < max ? request->server.min_generated : max;
  while (data.len - request->generator_offset < minimum && !request->generator_end) {
    request->generator_end = !request->generator->generate();
    data = request->generator->current();
  }

  // End of data?
  if (data.len <= request->generator_offset) return MHD_CONTENT_READER_END_OF_STREAM;

  // Copy generated data and remove them from the generator
  size_t data_len = min(data.len - request->generator_offset, max);
  memcpy(buf, data.str + request->generator_offset, data_len);
  request->generator_offset += data_len;
  if (data.len - request->generator_offset < minimum) {
    request->generator->consume(request->generator_offset);
    request->generator_offset = 0;
  }
  return data_len;
}

bool rest_server::microhttpd_request::valid_utf8(const string& text) {
  for (auto str = (const unsigned char*) text.c_str(); *str; str++)
    if (*str >= 0x80) {
      if (*str < 0xC0) return false;
      else if (*str < 0xE0) {
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
      } else if (*str < 0xF0) {
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
      } else if (*str < 0xF8) {
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
        str++; if (*str < 0x80 || *str >= 0xC0) return false;
      } else return false;
    }

  return true;
}

bool rest_server::microhttpd_request::http_value_compare(const char* string, const char* pattern) {
  // While there are pattern characters.
  while (*pattern) {
    // Skip spaces.
    while (*string && isspace(*string)) string++;
    if (!*string) return false;

    // Match the next character ignoring case.
    if (tolower(*string++) != tolower(*pattern++)) return false;
  }

  // Skip final spaces.
  while (*string && isspace(*string)) string++;

  // Succeed if there are no characters in string left or if there is a semicolon.
  return !*string || *string == ';';
}


// Class rest_server
void rest_server::set_log_file(ostream* log_file, unsigned max_log_size) {
  lock_guard<decltype(log_file_mutex)> log_file_lock(log_file_mutex);

  this->log_file = log_file;
  this->max_log_size = max_log_size;
}
void rest_server::set_min_generated(unsigned min_generated) { this->min_generated = min_generated; }
void rest_server::set_max_connections(unsigned max_connections) { this->max_connections = max_connections; }
void rest_server::set_max_request_body_size(unsigned max_request_body_size) { this->max_request_body_size = max_request_body_size; }
void rest_server::set_threads(unsigned threads) { this->threads = threads; }
void rest_server::set_timeout(unsigned timeout) { this->timeout = timeout; }

bool rest_server::start(rest_service* service, unsigned port) {
  if (!service) return false;
  this->service = service;

  if (!microhttpd_request::initialize()) return false;

  for (int use_poll = 1; use_poll >= 0; use_poll--) {
    MHD_OptionItem threadpool_size[] = {
      { threads ? MHD_OPTION_THREAD_POOL_SIZE : MHD_OPTION_END, int(threads), nullptr },
      { MHD_OPTION_END, 0, nullptr }
    };
    MHD_OptionItem connection_limit[] = {
      { max_connections ? MHD_OPTION_CONNECTION_LIMIT : MHD_OPTION_END, int(max_connections), nullptr },
      { MHD_OPTION_END, 0, nullptr }
    };

    daemon = MHD_start_daemon((threads ? MHD_USE_SELECT_INTERNALLY : MHD_USE_THREAD_PER_CONNECTION) | (use_poll ? MHD_USE_POLL : 0) | MHD_USE_PIPE_FOR_SHUTDOWN,
                              port, nullptr, nullptr, &handle_request, this,
                              MHD_OPTION_LISTENING_ADDRESS_REUSE, 1,
                              MHD_OPTION_ARRAY, threadpool_size,
                              MHD_OPTION_ARRAY, connection_limit,
                              MHD_OPTION_CONNECTION_MEMORY_LIMIT, size_t(64 << 10),
                              MHD_OPTION_CONNECTION_TIMEOUT, timeout,
                              MHD_OPTION_NOTIFY_COMPLETED, &request_completed, this,
                              MHD_OPTION_END);

    if (daemon) {
      log("REST server starting, port ", port, ", max connections ", max_connections, ", timeout ", timeout, ", max request body size ", max_request_body_size, ", min generated ", min_generated, '.');
      return true;
    }
  }

  return false;
}

void rest_server::stop() {
  if (!daemon) return;

  // Quiesce the daemon and wait for current requests to be handled.
  log("REST server closing listening port and waiting for current requests to finish.");
  MHD_socket socket = MHD_quiesce_daemon(daemon);
  if (socket != MHD_INVALID_SOCKET) MHD_socket_close(socket);
  while (true) {
    unsigned connections = *(unsigned*) MHD_get_daemon_info(daemon, MHD_DAEMON_INFO_CURRENT_CONNECTIONS);
    log("There are ", connections, " current connections.");
    if (!connections) break;
    this_thread::sleep_for(chrono::milliseconds(500));
  }
  log("REST server stopped.");

  MHD_stop_daemon(daemon);
  daemon = nullptr;
  service = nullptr;
  log_file = nullptr;
}

#if defined(_WIN32) && !defined(__CYGWIN__)
mutex rest_server_ctrl_c_mutex;
condition_variable rest_server_ctrl_c_cv;
unsigned rest_server_ctrl_c_handlers;
bool rest_server_ctrl_c_signalled;
BOOL WINAPI RestServerCtrlCHandler(DWORD dwCtrlType) {
  if (dwCtrlType == CTRL_C_EVENT) {
    if (!rest_server_ctrl_c_signalled) {
      rest_server_ctrl_c_mutex.lock();
      rest_server_ctrl_c_signalled = true;
      rest_server_ctrl_c_mutex.unlock();
      rest_server_ctrl_c_cv.notify_all();
    }
    return TRUE;
  }
  return FALSE;
}

bool rest_server::wait_until_signalled() {
  log("Waiting until Ctrl+C is pressed.");
  unique_lock<mutex> lock(rest_server_ctrl_c_mutex);
  if (!rest_server_ctrl_c_handlers) {
    rest_server_ctrl_c_signalled = false;
    if (!SetConsoleCtrlHandler(RestServerCtrlCHandler, TRUE))
      return false;
  }
  rest_server_ctrl_c_handlers++;
  if (!rest_server_ctrl_c_signalled)
    rest_server_ctrl_c_cv.wait(lock, []{ return rest_server_ctrl_c_signalled; });
  rest_server_ctrl_c_handlers--;
  if (!rest_server_ctrl_c_handlers) {
    SetConsoleCtrlHandler(RestServerCtrlCHandler, FALSE);
  }
  return true;
}
#else
bool rest_server::wait_until_signalled() {
  log("Waiting until SIGUSR1 or SIGINT is received.");

  sigset_t set;
  if (sigemptyset(&set) != 0) return false;
  if (sigaddset(&set, SIGUSR1) != 0) return false;
  if (sigaddset(&set, SIGINT) != 0) return false;
  if (sigprocmask(SIG_BLOCK, &set, nullptr) != 0) return false;

  // Wait for SIGINT or SIGUSR1
  int signal;
  do {
    if (sigwait(&set, &signal) != 0) return false;
  } while (signal != SIGINT && signal != SIGUSR1);

  return true;
}
#endif

int rest_server::handle_request(void* cls, struct MHD_Connection* connection, const char* url, const char* method, const char* /*version*/, const char* upload_data, size_t* upload_data_size, void** con_cls) {
  auto self = (rest_server*) cls;
  auto request = (microhttpd_request*) *con_cls;

  // Do we have a new request?
  if (!request) {
    const char* content_type = MHD_lookup_connection_value (connection, MHD_HEADER_KIND, MHD_HTTP_HEADER_CONTENT_TYPE);
    if (!content_type) content_type = "";

    if (!(request = new microhttpd_request(*self, connection, url, content_type, method)))
      return cerr << "Cannot allocate new request!" << endl, MHD_NO;

    *con_cls = request;
    return MHD_YES;
  }

  // Are we processing request body?
  if (*upload_data_size) {
    if (!request->process_request_body(upload_data, *upload_data_size))
      return MHD_NO;

    *upload_data_size = 0;
    return MHD_YES;
  }

  // Log complete request
  self->log_request(request);

  // Handle complete request
  return request->handle(self->service) ? MHD_YES : MHD_NO;
}

void rest_server::request_completed(void* /*cls*/, struct MHD_Connection* /*connection*/, void** con_cls, int /*toe*/) {
  auto request = (const microhttpd_request*) *con_cls;
  if (request) delete request;
}

template <typename... Args> void rest_server::log(Args&&... args) {
  if (!log_file) return;

  // Prepare timestamp
  char timestamp[32];
  time_t time_now;
  time_now = time(nullptr);
  tm tm_now;
#if defined(_WIN32) && !defined(__CYGWIN__)
  localtime_s(&tm_now, &time_now);
#else
  localtime_r(&time_now, &tm_now);
#endif
  size_t len = strftime(timestamp, sizeof(timestamp), "%a %d. %b %Y %H:%M:%S\t", &tm_now);

  // Locked using the log_file_mutex
  {
    lock_guard<decltype(log_file_mutex)> log_file_lock(log_file_mutex);

    if (len) *log_file << timestamp;
    log_append(forward<Args>(args)...);
    *log_file << endl;
  }
}

void rest_server::log_append() {}
template <typename Arg, typename... Args> void rest_server::log_append(Arg&& arg, Args&&... args) {
  *log_file << arg;
  log_append(forward<Args>(args)...);
}

void rest_server::log_append_pair(string& message, const char* key, const string& value) {
  size_t to_clean = message.size();

  message.append(key);
  {
    char length[39/*128-bit number*/ + 3/*():*/ + 1/*\0*/];
    sprintf(length, "(%u):", unsigned(value.size()));
    message.append(length);
  }

  if (!max_log_size || value.size() < max_log_size) {
    message.append(value);
  } else {
    struct utf8_helper {
      static bool valid_start(char chr) { return uint8_t(chr) < uint8_t(0x80) || uint8_t(chr) >= uint8_t(0xE0); }
    };

    size_t utf8_border = max_log_size >> 1;
    while (utf8_border && !utf8_helper::valid_start(value[utf8_border])) utf8_border--;
    message.append(value, 0, utf8_border);

    message.append(" ... ");

    utf8_border = value.size() - (max_log_size >> 1);
    while (utf8_border < value.size() && !utf8_helper::valid_start(value[utf8_border])) utf8_border++;
    message.append(value, utf8_border, value.size() - utf8_border);
  }

  // Map the \t, \r, \n in the appended message.
  for (; to_clean < message.size(); to_clean++)
    switch (message[to_clean]) {
      case '\t':
      case '\r': message[to_clean] = ' '; break;
      case '\n': message[to_clean] = '\r'; break;
    }
}

void rest_server::log_request(const microhttpd_request* request) {
  if (!log_file) return;

  auto sock_addr = request->address();
  char address[64] = "";
  if (sock_addr && sock_addr->sa_family == AF_INET) getnameinfo(sock_addr, sizeof(sockaddr_in), address, sizeof(address), nullptr, 0, NI_NUMERICHOST);
  if (sock_addr && sock_addr->sa_family == AF_INET6) getnameinfo(sock_addr, sizeof(sockaddr_in6), address, sizeof(address), nullptr, 0, NI_NUMERICHOST);

  auto forwarded_for = request->forwarded_for();

  string data;
  log_append_pair(data, "body", request->body);
  for (auto&& param : request->params) {
    data.push_back('\t');
    log_append_pair(data, param.first.c_str(), param.second);
  }

  log("Request\t", address, '\t', forwarded_for ? forwarded_for : "", '\t', request->url, '\t', data);
}

} // namespace microrestd
} // namespace ufal
