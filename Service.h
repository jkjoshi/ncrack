
 /***************************************************************************
 * Service.h -- The "Service" class encapsulates every bit of information  *
 * about the associated target's (Target class) service. Service-specific  *
 * options, statistical and timing information as well as functions for    *
 * handling username/password list iteration all belong to this class.     *
 *                                                                         *
 ***********************IMPORTANT NMAP LICENSE TERMS************************
 *                                                                         *
 * The Nmap Security Scanner is (C) 1996-2009 Insecure.Com LLC. Nmap is    *
 * also a registered trademark of Insecure.Com LLC.  This program is free  *
 * software; you may redistribute and/or modify it under the terms of the  *
 * GNU General Public License as published by the Free Software            *
 * Foundation; Version 2 with the clarifications and exceptions described  *
 * below.  This guarantees your right to use, modify, and redistribute     *
 * this software under certain conditions.  If you wish to embed Nmap      *
 * technology into proprietary software, we sell alternative licenses      *
 * (contact sales@insecure.com).  Dozens of software vendors already       *
 * license Nmap technology such as host discovery, port scanning, OS       *
 * detection, and version detection.                                       *
 *                                                                         *
 * Note that the GPL places important restrictions on "derived works", yet *
 * it does not provide a detailed definition of that term.  To avoid       *
 * misunderstandings, we consider an application to constitute a           *
 * "derivative work" for the purpose of this license if it does any of the *
 * following:                                                              *
 * o Integrates source code from Nmap                                      *
 * o Reads or includes Nmap copyrighted data files, such as                *
 *   nmap-os-db or nmap-service-probes.                                    *
 * o Executes Nmap and parses the results (as opposed to typical shell or  *
 *   execution-menu apps, which simply display raw Nmap output and so are  *
 *   not derivative works.)                                                * 
 * o Integrates/includes/aggregates Nmap into a proprietary executable     *
 *   installer, such as those produced by InstallShield.                   *
 * o Links to a library or executes a program that does any of the above   *
 *                                                                         *
 * The term "Nmap" should be taken to also include any portions or derived *
 * works of Nmap.  This list is not exclusive, but is meant to clarify our *
 * interpretation of derived works with some common examples.  Our         *
 * interpretation applies only to Nmap--we don't speak for other people's  *
 * GPL works.                                                              *
 *                                                                         *
 * If you have any questions about the GPL licensing restrictions on using *
 * Nmap in non-GPL works, we would be happy to help.  As mentioned above,  *
 * we also offer alternative license to integrate Nmap into proprietary    *
 * applications and appliances.  These contracts have been sold to dozens  *
 * of software vendors, and generally include a perpetual license as well  *
 * as providing for priority support and updates as well as helping to     *
 * fund the continued development of Nmap technology.  Please email        *
 * sales@insecure.com for further information.                             *
 *                                                                         *
 * As a special exception to the GPL terms, Insecure.Com LLC grants        *
 * permission to link the code of this program with any version of the     *
 * OpenSSL library which is distributed under a license identical to that  *
 * listed in the included COPYING.OpenSSL file, and distribute linked      *
 * combinations including the two. You must obey the GNU GPL in all        *
 * respects for all of the code used other than OpenSSL.  If you modify    *
 * this file, you may extend this exception to your version of the file,   *
 * but you are not obligated to do so.                                     *
 *                                                                         *
 * If you received these files with a written license agreement or         *
 * contract stating terms other than the terms above, then that            *
 * alternative license agreement takes precedence over these comments.     *
 *                                                                         *
 * Source is provided to this software because we believe users have a     *
 * right to know exactly what a program is going to do before they run it. *
 * This also allows you to audit the software for security holes (none     *
 * have been found so far).                                                *
 *                                                                         *
 * Source code also allows you to port Nmap to new platforms, fix bugs,    *
 * and add new features.  You are highly encouraged to send your changes   *
 * to nmap-dev@insecure.org for possible incorporation into the main       *
 * distribution.  By sending these changes to Fyodor or one of the         *
 * Insecure.Org development mailing lists, it is assumed that you are      *
 * offering the Nmap Project (Insecure.Com LLC) the unlimited,             *
 * non-exclusive right to reuse, modify, and relicense the code.  Nmap     *
 * will always be available Open Source, but this is important because the *
 * inability to relicense code has caused devastating problems for other   *
 * Free Software projects (such as KDE and NASM).  We also occasionally    *
 * relicense the code to third parties as discussed above.  If you wish to *
 * specify special license conditions of your contributions, just say so   *
 * when you send them.                                                     *
 *                                                                         *
 * This program is distributed in the hope that it will be useful, but     *
 * WITHOUT ANY WARRANTY; without even the implied warranty of              *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 * General Public License v2.0 for more details at                         *
 * http://www.gnu.org/licenses/gpl-2.0.html , or in the COPYING file       *
 * included with Nmap.                                                     *
 *                                                                         *
 ***************************************************************************/


#ifndef SERVICE_H
#define SERVICE_H

#include "ncrack.h"
#include "utils.h"
#include "Target.h"
#include "Buffer.h"
#include "timing.h"
#include <list>


#define BUFSIZE 256
#define MAX_HOSTINFO_LEN 1024

class Connection;
class Service;


/* 
 * Active connection taking place for authenticating service.
 * Each connection may try to authenticatate more than once before closing,
 * depending on the service. For UDP 1 connection = 1 authentication session.
 */
class Connection
{
	public:
		Connection(Service *serv);
		~Connection();

		int time_started;
		int time_elapsed;

    char *user;
    char *pass;

    /* 
     * True when we peer might close connection at the near moment. 
     * Consider the case, when some services after reaching the maximum
     * authentication limit per connecton, just drop the connection without
     * specifically telling you that you failed at the last authentication
     * attempt. Thus, we use this, to be able to count the correct number of
     * maximum attempts the peer lets us do (stored in 'supported_attempts'
     * inside the Service class). A module should probably set it to true
     * after writing the password on the wire and before issuing the next
     * read call. Also if you use it, don't forget to set it to false, in the first
     * state of your module, because we might need it to differentiate between
     * normal server FINs and FINs/RSTs sent in the middle of an authentication
     * due to strange network conditions.
     */
    bool peer_might_close; 

    /* True if we have received a server reply, that indicated that it didn't
     * close the connection prematurely. This may used in cases, when the
     * server may close the connection after the maximum allowed auth attempts
     * are reached, but will also print a relative message saying we failed.
     */
    bool finished_normally; /* XXX not currently used anywhere */

    bool check_closed;  /* true -> check if peer closed connection on us */
    bool peer_alive;    /* true -> if peer is certain to be alive currently */
    bool auth_complete; /* true -> login pair tested */
    bool from_pool;     /* true -> login pair was extracted from pair_pool */
    bool closed;        /* true -> connection was closed */
    bool auth_success;  /* true -> we found a valid pair!!! */

    void *misc_info;    /* additional state information that might be needed */

    int close_reason;

		int state;          /* module state-machine's current state */

    Buf *iobuf;

		char *buf;          /* auxiliary buffer */
		int bufsize;        /* total buffer size in bytes */
		long login_attempts;/* login attempts up until now */
		nsock_iod niod;     /* I/O descriptor for this connection */
		Service *service;   /* service it belongs to */
};

enum close_reasons { READ_EOF, READ_TIMEOUT, CON_ERR, CON_TIMEOUT };



typedef struct loginpair
{
  char *user;
  char *pass;
} loginpair;



class Service
{
	public:
		Service();
		~Service();

    /* ********************* Functions ******************* */

		Service(const Service&); /* copy constructor */
    const char *HostInfo(void);

    /* Add discovered credential to 'credentials_found' */
    void addCredential(char *user, char *pass);

    int getNextPair(char **login, char **pass);
    void appendToPool(char *login, char *pass);
    void removeFromPool(char *login, char *pass);
    bool isMirrorPoolEmpty(void);
    bool isPoolEmpty(void);

    void setListActive(void) { list_active = true; };
    void unsetListActive(void) { list_active = false; };
    bool getListActive(void) { return list_active; };

    void setListWait(void) { list_wait = true; };
    void unsetListWait(void) { list_wait = false; };
    bool getListWait(void) { return list_wait; };

    void setListPairfini(void) { list_pairfini = true; };
    void unsetListPairfini(void) { list_pairfini = false; };
    bool getListPairfini(void) { return list_pairfini; };

    void setListFull(void) { list_full = true; };
    void unsetListFull(void) { list_full = false; };
    bool getListFull(void) { return list_full; };

    void setListFinishing(void) { list_finishing = true; };
    void unsetListFinishing(void) { list_finishing = false; };
    bool getListFinishing(void) { return list_finishing; };

    void setListFinished(void) { list_finished = true; };
    bool getListFinished(void) { return list_finished; };


    /* ********************* Members ********************* */

		char *name;
		Target *target; /* service belongs to this host */
		u8 proto;
		u16 portno;

    /* list which holds discovered credentials if any */
    vector <loginpair> credentials_found;
  
		bool loginlist_fini;/* true if login list has been iterated through */ 

    vector <char *> *UserArray;
    vector <char *> *PassArray;

    bool just_started;
    unsigned int failed_connections;
    long active_connections;
    struct timeval last; /* time of last activated connection */

    /*
     * How many attempts the service supports per connection before
     * closing on us. This is used as a valuable piece of information
     * for many timing checks, and is gathered during the first connection
     * (since that probably is the most reliable one, because in the beginning
     * we haven't opened up too many connections yet)
     */
    unsigned int supported_attempts;

    /* total auth attempts, including failed ones */
    unsigned int total_attempts;
    
    /* auth attempts that have finished up to the point of completing
     * all authentication steps and getting the results */
    unsigned int finished_attempts; 

		/* timing options that override global ones */
		long min_connection_limit;  /* minimum number of concurrent parallel connections */
    long max_connection_limit;  /* maximum number of concurrent parallel connections */
    long ideal_parallelism;     /* ideal number of concurrent parallel connections */
		long auth_tries;            /* authentication attempts per connections */
		long connection_delay;      /* number of milliseconds to wait between each connection */
		long connection_retries;    /* number of connection retries after connection failure */
		/* misc options */
		bool ssl;
		void *module_data; /* service/module-specific data */

    RateMeter auth_rate_meter;
    struct last_auth_rate { 
      double rate;
      struct timeval time;
    } last_auth_rate;

		list <Connection *> connections;

  private:

    /* 
     * hostinfo in form "<service_name>://<ip or hostname>:<port number>"
     * e.g ftp://scanme.nmap.org:21 - Will be returned by HostInfo()  
     */
    char *hostinfo;

    /* 
     * Login pair pool that holds pairs that didn't manage to be authenticated
     * due to an error (the host timed out, the connection was forcefully closed
     * etc). If this pool has elements, then the next returned pair from
     * NextPair() will come from this pool. NextPair() will remove the
     * element from the list.
     */
    list <loginpair> pair_pool;

    /* 
     * Mirror login pair pool, that holds pairs from login pair pool but which
     * aren't removed from the list at the time they are used.
     * By this way we can determine, if we are currently using pairs from the
     * pair_pool by checking if the mirror_pair_pool is non-empty.
     */
    list <loginpair> mirror_pair_pool;

    vector <char *>::iterator uservi;
    vector <char *>::iterator passvi;

    bool list_active;   /* service is now on 'services_active' list */
    bool list_wait;     /* service appended to 'services_wait' list */
    bool list_pairfini; /* service appended to 'services_pairfini' list */ 
    bool list_full;     /* service appended to 'services_full' list */
    bool list_finishing;/* service appended to 'services_finishing' list */
    bool list_finished; /* service is now on 'services_finished' list */


};



#endif 
