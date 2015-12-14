//
//  SuperTuxKart - a fun racing game with go-kart
//  Copyright (C) 2013-2015 SuperTuxKart-Team
//
//  This program is free software; you can redistribute it and/or
//  modify it under the terms of the GNU General Public License
//  as published by the Free Software Foundation; either version 3
//  of the License, or (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

/*! \file protocol_manager.hpp
 *  \brief Contains structures and enumerations related to protocol management.
 */

#ifndef PROTOCOL_MANAGER_HPP
#define PROTOCOL_MANAGER_HPP

#include "network/network_string.hpp"
#include "network/protocol.hpp"
#include "utils/no_copy.hpp"
#include "utils/singleton.hpp"
#include "utils/synchronised.hpp"
#include "utils/types.hpp"

#include <vector>

class Event;
class STKPeer;

#define TIME_TO_KEEP_EVENTS 1.0

/** \enum PROTOCOL_STATE
 *  \brief Defines the three states that a protocol can have.
 */
enum ProtocolState
{
    PROTOCOL_STATE_INITIALISING, //!< The protocol is waiting to be started
    PROTOCOL_STATE_RUNNING,      //!< The protocol is being updated everytime.
    PROTOCOL_STATE_PAUSED,       //!< The protocol is paused.
    PROTOCOL_STATE_TERMINATED    //!< The protocol is terminated/does not exist.
};   // ProtocolState

// ----------------------------------------------------------------------------
/** \enum ProtocolRequestType
 *  \brief Defines actions that can be done about protocols.
 *  This enum is used essentially to keep the manager thread-safe and
 *  to avoid protocols modifying directly their state.
 */
enum ProtocolRequestType
{
    PROTOCOL_REQUEST_START,     //!< Start a protocol
    PROTOCOL_REQUEST_STOP,      //!< Stop a protocol
    PROTOCOL_REQUEST_PAUSE,     //!< Pause a protocol
    PROTOCOL_REQUEST_UNPAUSE,   //!< Unpause a protocol
    PROTOCOL_REQUEST_TERMINATE  //!< Terminate a protocol
};   // ProtocolRequestType

// ----------------------------------------------------------------------------
/** \struct ProtocolInfo
 *  \brief Stores the information needed to manage protocols
 */
typedef struct ProtocolInfo
{
    ProtocolState   m_state;      //!< The state of the protocol
    Protocol*       m_protocol;   //!< A pointer to the protocol
    uint32_t        m_id;         //!< The unique id of the protocol
} ProtocolInfo;

// ----------------------------------------------------------------------------
/** \struct ProtocolRequest
 *  \brief Represents a request to do an action about a protocol.
 */
typedef struct ProtocolRequest
{
    /** The type of request. */
    ProtocolRequestType m_type;

    /** The concerned protocol information. */
    ProtocolInfo m_protocol_info; 
} ProtocolRequest;

// ----------------------------------------------------------------------------
/** \struct ProtocolRequest
 *  \brief Used to pass the event to protocols that need it
 */
typedef struct EventProcessingInfo
{
    Event* m_event;
    double m_arrival_time;
    std::vector<unsigned int> m_protocols_ids;
} EventProcessingInfo;

// ----------------------------------------------------------------------------
/** \class ProtocolManager
 *  \brief Manages the protocols at runtime.
 *
 *  This class is in charge of storing and managing protocols.
 *  It is a singleton as there can be only one protocol manager per game
 *  instance. Any game object that wants to start a protocol must create a
 *  protocol and give it to this singleton. The protocols are updated in a
 *  special thread, to ensure that they are processed independently from the
 *  frames per second. Then, the management of protocols is thread-safe: any
 *  object can start/pause/stop protocols whithout problems.
 */ 
class ProtocolManager : public AbstractSingleton<ProtocolManager>,
                        public NoCopy
{
    friend class AbstractSingleton<ProtocolManager>;
    static void* mainLoop(void *data);
    public:
        
        virtual void abort();    
        virtual void propagateEvent(Event* event);
        virtual void sendMessage(Protocol* sender,
                                 const NetworkString& message,
                                 bool reliable = true);
        virtual void sendMessage(Protocol* sender, STKPeer* peer,
                                 const NetworkString& message,
                                 bool reliable = true);
        virtual void sendMessageExcept(Protocol* sender, STKPeer* peer,
                                       const NetworkString& message,
                                       bool reliable = true);
        virtual uint32_t requestStart(Protocol* protocol);
        virtual void requestStop(Protocol* protocol);
        virtual void requestPause(Protocol* protocol);
        virtual void requestUnpause(Protocol* protocol);
        virtual void requestTerminate(Protocol* protocol);
        virtual void update();
        virtual void asynchronousUpdate();
        virtual ProtocolState getProtocolState(uint32_t id);
        virtual ProtocolState getProtocolState(Protocol* protocol);
        virtual uint32_t  getProtocolID(Protocol* protocol);
        virtual Protocol* getProtocol(uint32_t id);
        virtual Protocol* getProtocol(ProtocolType type);
        bool isServer();
        int exit();

    protected:
        // protected functions
        /*!
         * \brief Constructor
         */
        ProtocolManager();
        /*!
         * \brief Destructor
         */
        virtual ~ProtocolManager();
        /*!
         * \brief Assign an id to a protocol.
         * This function will assign m_next_protocol_id as the protocol id.
         * This id starts at 0 at the beginning and is increased by 1 each time
         * a protocol starts.
         * \param protocol_info : The protocol info that needs an id.
         */
        void                    assignProtocolId(ProtocolInfo* protocol_info);

        virtual void startProtocol(ProtocolInfo &protocol);
        /*!
         * \brief Stops a protocol.
         * Coes nothing. Noone can stop running protocols for now.
         * \param protocol : ProtocolInfo to stop.
         */
        virtual void            stopProtocol(ProtocolInfo protocol);
        /*!
         * \brief Pauses a protocol.
         * Pauses a protocol and tells it that it's being paused.
         * \param protocol : ProtocolInfo to pause.
         */
        virtual void            pauseProtocol(ProtocolInfo protocol);
        /*!
         * \brief Unpauses a protocol.
         * Unpauses a protocol and notifies it.
         * \param protocol : ProtocolInfo to unpause.
         */
        virtual void            unpauseProtocol(ProtocolInfo protocol);
        /*!
         * \brief Notes that a protocol is terminated.
         * Remove a protocol from the protocols vector.
         * \param protocol : ProtocolInfo concerned.
         */
        virtual void            protocolTerminated(ProtocolInfo protocol);

        bool sendEvent(EventProcessingInfo* event, bool synchronous);

        // protected members
        /** Contains the running protocols.
         *  This stores the protocols that are either running or paused, their
         * state and their unique id. */
        Synchronised<std::vector<ProtocolInfo> >m_protocols;

        /** Contains the network events to pass to protocols. */
        Synchronised<std::vector<EventProcessingInfo> > m_events_to_process;

        /** Contains the requests to start/stop etc... protocols. */
        std::vector<ProtocolRequest>    m_requests;
        /*! \brief The next id to assign to a protocol.
         * This value is incremented by 1 each time a protocol is started.
         * If a protocol has an id lower than this value, it means that it have
         * been formerly started.
         */
        uint32_t                        m_next_protocol_id;

        // mutexes:
        /*! Used to ensure that the protocol vector is used thread-safely.   */
        pthread_mutex_t                 m_asynchronous_protocols_mutex;
        /*! Used to ensure that the request vector is used thread-safely.    */
        pthread_mutex_t                 m_requests_mutex;
        /*! Used to ensure that the protocol id is used in a thread-safe way.*/
        pthread_mutex_t                 m_id_mutex;
        /*! Used when need to quit.*/
        pthread_mutex_t                 m_exit_mutex;

        /*! Update thread.*/
        pthread_t* m_update_thread;
        /*! Asynchronous update thread.*/
        pthread_t* m_asynchronous_update_thread;
        /*! True if the thread is running. */
        bool m_asynchronous_thread_running;

};

#endif // PROTOCOL_MANAGER_HPP
