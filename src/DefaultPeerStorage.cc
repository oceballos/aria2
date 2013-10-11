/* <!-- copyright */
/*
 * aria2 - The high speed download utility
 *
 * Copyright (C) 2006 Tatsuhiro Tsujikawa
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
#include "DefaultPeerStorage.h"

#include <algorithm>

#include "LogFactory.h"
#include "Logger.h"
#include "message.h"
#include "Peer.h"
#include "BtRuntime.h"
#include "BtSeederStateChoke.h"
#include "BtLeecherStateChoke.h"
#include "PieceStorage.h"
#include "wallclock.h"
#include "a2functional.h"
#include "fmt.h"
#include "SimpleRandomizer.h"
#include <string>
using std::string;
using namespace std;
#include <cstdlib>
#include <stdio.h>
#include <fstream>
#include <iostream>
using std::cout;
using std::endl;
#include <cstring>
#include <sstream>
using std::cout;
using std::cin;
using std::ifstream;
#include <vector>
#include "find.cpp"

namespace aria2 {

namespace {

const size_t MAX_PEER_LIST_SIZE = 512;
const size_t MAX_PEER_LIST_UPDATE = 100;

} // namespace

DefaultPeerStorage::DefaultPeerStorage()
  : maxPeerListSize_(MAX_PEER_LIST_SIZE),
    seederStateChoke_(new BtSeederStateChoke()),
    leecherStateChoke_(new BtLeecherStateChoke()),
    lastTransferStatMapUpdated_(0)
{}

DefaultPeerStorage::~DefaultPeerStorage()
{
  delete seederStateChoke_;
  delete leecherStateChoke_;
  assert(uniqPeers_.size() == unusedPeers_.size() + usedPeers_.size());
}

size_t DefaultPeerStorage::countAllPeer() const
{
  return unusedPeers_.size() + usedPeers_.size();
}

bool DefaultPeerStorage::isPeerAlreadyAdded(const std::shared_ptr<Peer>& peer)
{
  return uniqPeers_.count(std::make_pair(peer->getIPAddress(),
                                         peer->getOrigPort()));
}

void DefaultPeerStorage::addUniqPeer(const std::shared_ptr<Peer>& peer)
{
  uniqPeers_.insert(std::make_pair(peer->getIPAddress(), peer->getOrigPort()));
}




//Agregar un peer
bool DefaultPeerStorage::addPeer(const std::shared_ptr<Peer>& peer)
{
  if(isPeerAlreadyAdded(peer)) {
    A2_LOG_DEBUG(fmt("Adding %s:%u is rejected because it has been already"
                     " added.",
                     peer->getIPAddress().c_str(), peer->getPort()));
    return false;
  }
  if(isBadPeer(peer->getIPAddress())) {
    A2_LOG_DEBUG(fmt("Adding %s:%u is rejected because it is marked bad.",
                     peer->getIPAddress().c_str(), peer->getPort()));
    return false;
  }
  const size_t peerListSize = unusedPeers_.size();
  if(peerListSize >= maxPeerListSize_) {
    deleteUnusedPeer(peerListSize-maxPeerListSize_+1);
  }

  unusedPeers_.push_front(peer);
  addUniqPeer(peer);
  A2_LOG_DEBUG(fmt("Now unused peer list contains %lu peers",
                   static_cast<unsigned long>(unusedPeers_.size())));

  return true;

}


//Agregar lista de peers
void DefaultPeerStorage::addPeer(const std::vector<std::shared_ptr<Peer> >& peers)
{

//	  string anucio = "echo Nuevo Anucio del tracker >> sa.txt";
//	  system (anucio.c_str());
//	  A2_LOG_NOTICE(fmt(_("Nuevo Anucio del tracker")));

  size_t added = 0;
  size_t addMax = std::min(maxPeerListSize_, MAX_PEER_LIST_UPDATE);

  string arreglo1[(int)addMax];
  int t = 0;



//  doc = fopen("prueba.txt","w");
  for(std::vector<std::shared_ptr<Peer> >::const_iterator itr = peers.begin(),
        eoi = peers.end(); itr != eoi && added < addMax; ++itr) {
    const std::shared_ptr<Peer>& peer = *itr;
    if(isPeerAlreadyAdded(peer)) {
      A2_LOG_DEBUG(fmt("Adding %s:%u is rejected because it has been already"
                       " added.",
                       peer->getIPAddress().c_str(), peer->getPort()));
      arreglo1[t] = "Esta ip no sirve";
      t++;
      continue;
    } else if(isBadPeer(peer->getIPAddress())) {
      A2_LOG_DEBUG(fmt("Adding %s:%u is rejected because it is marked bad.",
                       peer->getIPAddress().c_str(), peer->getPort()));
      arreglo1[t] = "Esta ip no sirve";
      t++;
      continue;
    } else {


///////////////////////////EMPIEZA MI PROGRAMA!!! ///////////////

      arreglo1[t]=peer->getIPAddress();
      string ip = arreglo1[t];

    int sum =0;
    string arreglo[1000];
    int cont = 0; // para llevar el numero del arreglo

    string sub_str;
    string num;
    string temp;

 //   system("mkdir tracker");

    string hola2 = "traceroute -UnAm 10 "+ip+" | awk '{print $3}'  > trace3.txt"; //ping google.com -c 3
    system (hola2.c_str());

    ifstream myfile;
    myfile.open("trace3.txt");


    if(myfile.is_open())//si el archivo esta abierto
    {
    while(!myfile.eof()) // mientras NO sea el final del arc hivo
    {
   myfile >> temp;
   if(temp != "*" && temp != "[*" && temp != "[*]"){
   arreglo[cont] = temp;
   cont++;
              }
            }
        vector <string> aux;
        for(int i=0;i<cont;i++){
        if(arreglo[1]!=arreglo[i]){//CON QUE SOLO COMPARE UNO CON EL RESTO SE CUANTOS DIFERENTES HAY
		if(i==0){
			sum++;
			aux.push_back(arreglo[i]);//AGREGO EL NUMERO A LA LISTA DE VISTOS
		}
		else if( !find(aux , arreglo[i]) ){
                        sum++;
                        aux.push_back(arreglo[i]);//AGREGO EL NUMERO A LA LISTA DE VISTOS
		}
        }
                                }
	}

                stringstream stream; //Esto es para pasar de int a string //
                string suma;
                stream << sum;
                suma= stream.str();

                    string hola3 = "echo "+suma+"#"+ip+" >> sa.txt";
                    system (hola3.c_str());

////////////////AQUI CREO EL .TXT CON LAS IPS  Y LAS SUMAS DE SIS AUTONOMOS!!
                    t++;

      A2_LOG_DEBUG(fmt(MSG_ADDING_PEER,
                       peer->getIPAddress().c_str(), peer->getPort()));

    }

}

//////////////////////////////////////////// Esto es para ordenar

  ifstream myfile4;
  int c_lines = 0, i = 0, num, *nums;
  string line, *lines;
  char numero[1000], basura[1000];
  myfile4.open ("sa.txt");
  while(getline(myfile4, line))
  {
  	c_lines++;
  }
  myfile4.close();
  lines = new string[c_lines];
  nums = new int[c_lines];
  myfile4.open ("sa.txt");
  for(int i = 0; i < c_lines; i++)
  {
  	getline(myfile4, line);
  	lines[i] = line;
  	sscanf(line.c_str(), "%[^#]#%[^\n]\n", numero, basura);
  	nums[i] = atoi(numero);
  }
  myfile4.close();
  for(int i = 0; i < c_lines - 1; i++)
  	for(int j = i + 1; j < c_lines; j++)
  	{
  		if(nums[i] > nums[j])
  		{
  			int aux = nums[i];
  			nums[i] = nums[j];
  			nums[j] = aux;

  			string aux2 = lines[i];
  			lines[i] = lines[j];
  			lines[j] = aux2;
  		}
  	}

  ofstream file;
  file.open("sa3.txt");
  for(int i = 0; i < c_lines; i++)
  file << lines[i] << endl;
  file.close();

  system("awk -F# '{print $2}' sa3.txt > saordenadas.txt");


/////////////////////////////////////Aqui termina el ordenamiento/////////////////////

///////////////////////////Aqui corto por la mitad mi saordenadas/////////

int cont4 = 0; // para llevar el numero del arreglo
string temp4;
////int tam = sizeof arreglo/sizeof arreglo[0];

ifstream myfile;
myfile.open("saordenadas.txt");


system("wc -l saordenadas.txt | awk {'print $1'} > nlineas.txt");


ifstream nlineas;
string arreglo[1];
nlineas.open("nlineas.txt");
if(nlineas.is_open()) { //si el archivo esta abierto
while(!nlineas.eof()) // mientras NO sea el final del arc hivo
   nlineas >> temp4;
   arreglo[cont4] = temp4;
}
   string n = arreglo[0];
   int entero = atoi(n.c_str());
    int mitad = (entero/2);
    cout <<mitad<<endl;

stringstream trans;
string mit;
trans << mitad;
mit = trans.str();

string filtradas = "split -l "+mit+" saordenadas.txt filtradas";
system(filtradas.c_str());

  A2_LOG_NOTICE(fmt("cree fltradas"));

/////////////////////////////////////Aqui termina el cut de saordenadas/////////////////////

/////////////////////////AQUI EMPIEZA MI FOR

///////////////////***********************************************************************
  int u=0; ///////////////////OSVALDO PARA QUE ERA ESTE CONTADOR TE ACUERDAS?????????????????????
  for(std::vector<std::shared_ptr<Peer> >::const_iterator itr = peers.begin(),
  eoi = peers.end(); itr != eoi && added < addMax; ++itr) {
  const std::shared_ptr<Peer>& peer = *itr;

  A2_LOG_NOTICE(fmt("Acabo de entrar a mi for super loco"));

    string temp2;
    ifstream myfile2;
//    myfile2.open("filtradasaa.txt");

    myfile2.open("filtradasaa.txt");

    string arreglo2[(int)addMax];
//int n = (sizeof(arreglo2)/sizeof(arreglo2[0]))/2;
    int cont2 = 0; // para llevar el numero del arreglo


if(myfile2.is_open())//si el archivo esta abierto
{
while(myfile2.eof()) // mientras NO sea el final del arc hivo
{
   myfile2 >> temp2;
   arreglo2[cont2] = temp2;
   cont2++;
              }
            }

            for(int u = 0; u< (int)addMax ; u++){

              A2_LOG_NOTICE(fmt("Despues de entrar a mi for mi peer es %s", peer->getIPAddress().c_str()));
              A2_LOG_NOTICE(fmt("Despues de entrar a mi for mi peer es %s", arreglo2[u].c_str()));

            if(peer->getIPAddress().c_str() == arreglo2[u]){ //asi estaba antes y no funcionó
 //               if(arreglo1[t] == arreglo2[u]){
                //agrego al peer a la lista
                A2_LOG_DEBUG(fmt("Acabo de añadir the real un peer %s", arreglo2[u].c_str()));
                  unusedPeers_.push_front(peer);
                  addUniqPeer(peer);
                  ++added;
                   u++;  /////////////aQUI ESATA DE NUEVO ESTE CONTADORRR

            }
            } //cierra el ultimo for

    } //cerrar el foor
////////////////////// AQUI TERMINAR MI FOR

////////////////////////////////////////AQUI TERMINA MI PROGRAMA!! //////

  const size_t peerListSize = unusedPeers_.size();
  if(peerListSize > maxPeerListSize_) {
    deleteUnusedPeer(peerListSize-maxPeerListSize_);
  }
  A2_LOG_DEBUG(fmt("Now unused peer list contains %lu peers",
                   static_cast<unsigned long>(unusedPeers_.size())));
}


void DefaultPeerStorage::addDroppedPeer(const std::shared_ptr<Peer>& peer)
{
  // Make sure that duplicated peers exist in droppedPeers_. If
  // exists, erase older one.
  for(std::deque<std::shared_ptr<Peer> >::iterator i = droppedPeers_.begin(),
        eoi = droppedPeers_.end(); i != eoi; ++i) {
    if((*i)->getIPAddress() == peer->getIPAddress() &&
       (*i)->getPort() == peer->getPort()) {
      droppedPeers_.erase(i);
      break;
    }
  }
  droppedPeers_.push_front(peer);
  if(droppedPeers_.size() > 50) {
    droppedPeers_.pop_back();
  }
}

const std::deque<std::shared_ptr<Peer> >& DefaultPeerStorage::getUnusedPeers()
{
  return unusedPeers_;
}

const PeerSet& DefaultPeerStorage::getUsedPeers()
{
  return usedPeers_;
}

const std::deque<std::shared_ptr<Peer> >& DefaultPeerStorage::getDroppedPeers()
{
  return droppedPeers_;
}

bool DefaultPeerStorage::isPeerAvailable() {
  return !unusedPeers_.empty();
}

bool DefaultPeerStorage::isBadPeer(const std::string& ipaddr)
{
  std::map<std::string, time_t>::iterator i = badPeers_.find(ipaddr);
  if(i == badPeers_.end()) {
    return false;
  } else if(global::wallclock().getTime() >= (*i).second) {
    badPeers_.erase(i);
    return false;
  } else {
    return true;
  }
}

void DefaultPeerStorage::addBadPeer(const std::string& ipaddr)
{
  if(lastBadPeerCleaned_.difference(global::wallclock()) >= 3600) {
    for(std::map<std::string, time_t>::iterator i = badPeers_.begin(),
          eoi = badPeers_.end(); i != eoi;) {
      if(global::wallclock().getTime() >= (*i).second) {
        A2_LOG_DEBUG(fmt("Purge %s from bad peer", (*i).first.c_str()));
        badPeers_.erase(i++);
        // badPeers_.end() will not be invalidated.
      } else {
        ++i;
      }
    }
    lastBadPeerCleaned_ = global::wallclock();
  }
  A2_LOG_DEBUG(fmt("Added %s as bad peer", ipaddr.c_str()));
  // We use variable timeout to avoid many bad peers wake up at once.
  badPeers_[ipaddr] = global::wallclock().getTime()+
    std::max(SimpleRandomizer::getInstance()->getRandomNumber(601), 120L);
}

void DefaultPeerStorage::deleteUnusedPeer(size_t delSize) {
  for(; delSize > 0 && !unusedPeers_.empty(); --delSize) {
    onErasingPeer(unusedPeers_.back());
    unusedPeers_.pop_back();
  }
}

std::shared_ptr<Peer> DefaultPeerStorage::checkoutPeer(cuid_t cuid)
{
  if(!isPeerAvailable()) {
    return nullptr;
  }
  std::shared_ptr<Peer> peer = unusedPeers_.front();
  unusedPeers_.pop_front();
  peer->usedBy(cuid);
  usedPeers_.insert(peer);
  A2_LOG_DEBUG(fmt("Checkout peer %s:%u to CUID#%" PRId64,
                   peer->getIPAddress().c_str(), peer->getPort(),
                   peer->usedBy()));
  return peer;
}

void DefaultPeerStorage::onErasingPeer(const std::shared_ptr<Peer>& peer)
{
  uniqPeers_.erase(std::make_pair(peer->getIPAddress(),
                                  peer->getOrigPort()));
}

void DefaultPeerStorage::onReturningPeer(const std::shared_ptr<Peer>& peer)
{
  if(peer->isActive()) {
    if(peer->isDisconnectedGracefully() && !peer->isIncomingPeer()) {
      peer->startDrop();
      addDroppedPeer(peer);
    }
    // Execute choking algorithm if unchoked and interested peer is
    // disconnected.
    if(!peer->amChoking() && peer->peerInterested()) {
      executeChoke();
    }
  }
  peer->usedBy(0);
}

void DefaultPeerStorage::returnPeer(const std::shared_ptr<Peer>& peer)
{
  A2_LOG_DEBUG(fmt("Peer %s:%u returned from CUID#%" PRId64,
                   peer->getIPAddress().c_str(), peer->getPort(),
                   peer->usedBy()));
  if(usedPeers_.erase(peer)) {
    onReturningPeer(peer);
    onErasingPeer(peer);
  } else {
    A2_LOG_DEBUG(fmt("Cannot find peer %s:%u in usedPeers_",
                     peer->getIPAddress().c_str(), peer->getPort()));
  }
}

bool DefaultPeerStorage::chokeRoundIntervalElapsed()
{
  const time_t CHOKE_ROUND_INTERVAL = 10;
  if(pieceStorage_->downloadFinished()) {
    return seederStateChoke_->getLastRound().
      difference(global::wallclock()) >= CHOKE_ROUND_INTERVAL;
  } else {
    return leecherStateChoke_->getLastRound().
      difference(global::wallclock()) >= CHOKE_ROUND_INTERVAL;
  }
}

void DefaultPeerStorage::executeChoke()
{
  if(pieceStorage_->downloadFinished()) {
    return seederStateChoke_->executeChoke(usedPeers_);
  } else {
    return leecherStateChoke_->executeChoke(usedPeers_);
  }
}

void DefaultPeerStorage::setPieceStorage(const std::shared_ptr<PieceStorage>& ps)
{
  pieceStorage_ = ps;
}

void DefaultPeerStorage::setBtRuntime(const std::shared_ptr<BtRuntime>& btRuntime)
{
  btRuntime_ = btRuntime;
}

} // namespace aria2

