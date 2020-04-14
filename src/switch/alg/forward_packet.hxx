// Copyright (C) 2003-2009 Laboratoire de Recherche en Informatique

// This file is part of Qolyester.

// Qolyester is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.

// Qolyester is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HXX
# define QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HXX 1

# include "gra/graph.hh"
# include "set/client.hh"

# include "forward_packet.hh"

namespace olsr {

  extern pathnet_t	path_net;
  extern clientmap_t	client_map;
  extern timeval_t	start_time;

  namespace alg {

    void forward_packet(const address_t& from, utl::ConstData p)
    {
      pathnet_t::nset_t::const_iterator	x =
	path_net.nodes().find(gra::AdjNode::make_key(from));

      debug << "Forwarding packet {\n";
      for (pathnet_t::succ_iterator i = path_net.succ_begin(x);
	   i != path_net.succ_end(x); ++i) {
	if (i->endpoint1() != from)
	  continue;
	clientmap_t::iterator	cx = client_map.find(i->endpoint2());
	if (cx == client_map.end())
	  continue;
	timeval_t	elapsed = timeval_t::now() - start_time;
	if (i->life_begin() > elapsed or
	    (i->life_end() != 0 and i->life_end() < elapsed))
	  continue;
	float	loss = (float) ::rand() / RAND_MAX;
	if (loss <= i->loss()) {
	  debug << "  Dropping packet from " << i->endpoint1()
		<< " to " << i->endpoint2() << '\n';
	  continue;
	}
	if (i->delay() == timeval_t(0)) {
	  debug << "  Queuing packet from " << i->endpoint1()
		<< " to " << i->endpoint2() << " for emission" << std::endl;
	  cx->second->push(p);
	} else {
	  debug << "  Later transmission from " << i->endpoint1()
		<< " to " << i->endpoint2() << '\n';
	  scheduler.insert(new sch::PacketGenerator(timeval_t::in(i->delay()),
						    p, i->endpoint2()));
	}
      }
      debug << '}' << std::endl;
    }

  } // namespace alg

} // namespace olsr

#endif // !QOLYESTER_SWITCH_ALG_FORWARD_PACKET_HXX
