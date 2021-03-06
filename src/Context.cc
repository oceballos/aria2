/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2013 Tatsuhiro Tsujikawa
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 *
 * In addition, as a special exception, the copyright holders give
 * permission to link the code of portions of this program with the
 * OpenSSL library under certain conditions as described in each
 * individual source file, and distribute linked combinations
 * including the two.
 * You must obey the GNU General Public License in all respects
 * for all of the code used other than OpenSSL.  If you modify
 * file(s) with this exception, you may extend this exception to your
 * version of the file(s), but you are not obligated to do so.  If you
 * do not wish to do so, delete this exception statement from your
 * version.  If you delete this exception statement from all source
 * files in the program, then also delete it here.
 */
/* copyright --> */
#include "Context.h"

#include <unistd.h>
#include <getopt.h>

#include <numeric>
#include <vector>
#include <iostream>

#include "LogFactory.h"
#include "Logger.h"
#include "util.h"
#include "FeatureConfig.h"
#include "MultiUrlRequestInfo.h"
#include "SimpleRandomizer.h"
#include "File.h"
#include "message.h"
#include "prefs.h"
#include "Option.h"
#include "a2algo.h"
#include "a2io.h"
#include "a2time.h"
#include "Platform.h"
#include "FileEntry.h"
#include "RequestGroup.h"
#include "download_helper.h"
#include "Exception.h"
#include "ProtocolDetector.h"
#include "RecoverableException.h"
#include "SocketCore.h"
#include "DownloadContext.h"
#include "fmt.h"
#include "console.h"
#include "UriListParser.h"
#ifdef ENABLE_BITTORRENT
# include "bittorrent_helper.h"
#endif // ENABLE_BITTORRENT
#ifdef ENABLE_METALINK
# include "metalink_helper.h"
# include "MetalinkEntry.h"
#endif // ENABLE_METALINK
#ifdef ENABLE_MESSAGE_DIGEST
# include "message_digest_helper.h"
#endif // ENABLE_MESSAGE_DIGEST

extern char* optarg;
extern int optind, opterr, optopt;

namespace aria2 {

#ifdef ENABLE_BITTORRENT
namespace {
void showTorrentFile(const std::string& uri)
{
  std::shared_ptr<Option> op(new Option());
  std::shared_ptr<DownloadContext> dctx(new DownloadContext());
  bittorrent::load(uri, dctx, op);
  bittorrent::print(*global::cout(), dctx);
}
} // namespace
#endif // ENABLE_BITTORRENT

#ifdef ENABLE_METALINK
namespace {
void showMetalinkFile
(const std::string& uri, const std::shared_ptr<Option>& op)
{
  auto fileEntries = MetalinkEntry::toFileEntry
    (metalink::parseAndQuery(uri, op.get(), op->get(PREF_METALINK_BASE_URI)));
  util::toStream(std::begin(fileEntries), std::end(fileEntries),
                 *global::cout());
  global::cout()->write("\n");
  global::cout()->flush();
}
} // namespace
#endif // ENABLE_METALINK

#if defined ENABLE_BITTORRENT || defined ENABLE_METALINK
namespace {
void showFiles
(const std::vector<std::string>& uris, const std::shared_ptr<Option>& op)
{
  ProtocolDetector dt;
  for(const auto & uri : uris) {
    printf(">>> ");
    printf(MSG_SHOW_FILES, (uri).c_str());
    printf("\n");
    try {
#ifdef ENABLE_BITTORRENT
      if(dt.guessTorrentFile(uri)) {
        showTorrentFile(uri);
      } else
#endif // ENABLE_BITTORRENT
#ifdef ENABLE_METALINK
        if(dt.guessMetalinkFile(uri)) {
          showMetalinkFile(uri, op);
        } else
#endif // ENABLE_METALINK
          {
            printf(MSG_NOT_TORRENT_METALINK);
            printf("\n\n");
          }
    } catch(RecoverableException& e) {
      global::cout()->printf("%s\n", e.stackTrace().c_str());
    }
  }
}
} // namespace
#endif // ENABLE_BITTORRENT || ENABLE_METALINK

extern error_code::Value option_processing(Option& option, bool standalone,
                                           std::vector<std::string>& uris,
                                           int argc, char** argv,
                                           const KeyVals& options);

Context::Context(bool standalone,
                 int argc, char** argv, const KeyVals& options)
{
  std::vector<std::string> args;
  std::shared_ptr<Option> op(new Option());
  error_code::Value rv;
  rv = option_processing(*op.get(), standalone, args, argc, argv, options);
  if(rv != error_code::FINISHED) {
    if(standalone) {
      exit(rv);
    } else {
      throw DL_ABORT_EX("Option processing failed");
    }
  }
  SimpleRandomizer::init();
#ifdef ENABLE_BITTORRENT
  bittorrent::generateStaticPeerId(op->get(PREF_PEER_ID_PREFIX));
#endif // ENABLE_BITTORRENT
  
#ifdef ENABLE_CDNVIDEO
  ProtocolDetector dt;
  if(dt.guessCDNVideo(op->get(PREF_CDNVIDEO_BASE_URI))){
    A2_LOG_NOTICE("Se reconoce la URL de youtube");
    std::string systemQuery="sh src/youtube-dl-aria.sh ";
    systemQuery+=op->get(PREF_CDNVIDEO_BASE_URI);
    system(systemQuery.c_str());
  }
  else{
    A2_LOG_DEBUG("la URL no es de youtube");
  }
#endif
  LogFactory::setLogFile(op->get(PREF_LOG));
  LogFactory::setLogLevel(op->get(PREF_LOG_LEVEL));
  LogFactory::setConsoleLogLevel(op->get(PREF_CONSOLE_LOG_LEVEL));
  if(op->getAsBool(PREF_QUIET)) {
    LogFactory::setConsoleOutput(false);
  }
  LogFactory::reconfigure();
  A2_LOG_INFO("<<--- --- --- ---");
  A2_LOG_INFO("  --- --- --- ---");
  A2_LOG_INFO("  --- --- --- --->>");
  A2_LOG_INFO(fmt("%s %s %s", PACKAGE, PACKAGE_VERSION, TARGET));
  A2_LOG_INFO(MSG_LOGGING_STARTED);

  if(op->getAsBool(PREF_DISABLE_IPV6)) {
    SocketCore::setProtocolFamily(AF_INET);
    // Get rid of AI_ADDRCONFIG. It causes name resolution error
    // when none of network interface has IPv4 address.
    setDefaultAIFlags(0);
  }
  net::checkAddrconfig();
  // Bind interface
  if(!op->get(PREF_INTERFACE).empty()) {
    std::string iface = op->get(PREF_INTERFACE);
    SocketCore::bindAddress(iface);
  }
  std::vector<std::shared_ptr<RequestGroup> > requestGroups;
  std::shared_ptr<UriListParser> uriListParser;
#ifdef ENABLE_BITTORRENT
  if(!op->blank(PREF_TORRENT_FILE)) {
    if(op->get(PREF_SHOW_FILES) == A2_V_TRUE) {
      showTorrentFile(op->get(PREF_TORRENT_FILE));
      return;
    } else {
      createRequestGroupForBitTorrent(requestGroups, op, args,
                                      op->get(PREF_TORRENT_FILE));
    }
  }
  else
#endif // ENABLE_BITTORRENT
#ifdef ENABLE_METALINK
    if(!op->blank(PREF_METALINK_FILE)) {
      if(op->get(PREF_SHOW_FILES) == A2_V_TRUE) {
        showMetalinkFile(op->get(PREF_METALINK_FILE), op);
        return;
      } else {
        createRequestGroupForMetalink(requestGroups, op);
      }
    }
    else
#endif // ENABLE_METALINK
      if(!op->blank(PREF_INPUT_FILE)) {
        if(op->getAsBool(PREF_DEFERRED_INPUT)) {
          uriListParser = openUriListParser(op->get(PREF_INPUT_FILE));
        } else {
          createRequestGroupForUriList(requestGroups, op);
        }
#if defined ENABLE_BITTORRENT || defined ENABLE_METALINK
      } else if(op->get(PREF_SHOW_FILES) == A2_V_TRUE) {
        showFiles(args, op);
        return;
#endif // ENABLE_METALINK || ENABLE_METALINK
      } else {
        createRequestGroupForUri(requestGroups, op, args, false, false, true);
      }

  // Remove option values which is only valid for URIs specified in
  // command-line. If they are left, because op is used as a template
  // for new RequestGroup(such as created in RPC command), they causes
  // unintentional effect.
  for(auto i = op; i; i = i->getParent()) {
    i->remove(PREF_OUT);
    i->remove(PREF_FORCE_SEQUENTIAL);
    i->remove(PREF_INPUT_FILE);
    i->remove(PREF_INDEX_OUT);
    i->remove(PREF_SELECT_FILE);
    i->remove(PREF_PAUSE);
    i->remove(PREF_CHECKSUM);
    i->remove(PREF_GID);
  }
  if(standalone &&
     !op->getAsBool(PREF_ENABLE_RPC) && requestGroups.empty() &&
     !uriListParser) {
    global::cout()->printf("%s\n", MSG_NO_FILES_TO_DOWNLOAD);
  } else {
    reqinfo.reset(new MultiUrlRequestInfo(std::move(requestGroups),
                                          op,
                                          uriListParser));
  }
}

Context::~Context()
{
}

} // namespace aria2
