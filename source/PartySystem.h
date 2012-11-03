#ifndef __PARTYFACTORY_H__
#define __PARTYFACTORY_H__

#include "Prerequisites.h"
#include "Singleton.h"
#include "CDataList.h"

namespace UOX
{
	class PartyEntry
	{
	protected:
		CChar *				member;
		std::bitset< 8 >	settings;
	public:
		CChar *				Member( void ) const;
		bool				IsLeader( void ) const;
		bool				IsLootable( void ) const;

		void				Member( CChar *valid );
		void				IsLeader( bool value );
		void				IsLootable( bool value );

							PartyEntry();
							PartyEntry( CChar *m, bool isLeader = false, bool isLootable = false );
	};

	class Party
	{
	protected:
		std::vector< PartyEntry * >	members;
		CChar *						leader;
		bool						isNPC;
	public:
        /** Sends a generic packet to an individual or the entire party
        @remarks
		This allows for easier simplification of some code, such as 
		whether we intend to send updates or removals to an individual
		or the entire party.  It can also be used from outside to send
		packets like speech
        @par
		CPUOXBuffer *toSend		The packet we want to send
		CSocket *toSendTo		The socket, if any, we send to.  If this
								value is NULL, then we send to the entire
								party
        */
		void						SendPacket( CPUOXBuffer *toSend, CSocket *toSendTo = NULL );
        /** Adds a member to the party
        @remarks
		This adds a new member to the party.  It validates that the member
		is not already in the party, and that the object being added really
		is a valid character.  It does not validate if the character exists
		in a separate party
        @par
		CChar *i				The character we wish to add to the party
        */
		bool						AddMember( CChar *i );
        /** Removes a member from the party
        @remarks
		This removes a member from the party.  It validates that the member
		is already in the party, and that the object being removed really is 
		a valid character.
        @par
		CChar *i				The character we wish to remove from the party
        */
		bool						RemoveMember( CChar *i );
        /** Changes the party's leader
        @remarks
		This changes the party's leader to a new character.  In the event that
		an old leader actually exists, the function takes care to shuffle the
		data internally so that the new leader is in the right position for
		packets that get sent to the clients.  NOTE: It may be worth sending
		an update packet from here, but we could get into some odd dependency
		chains then (see constructor)
        @par
		CChar *member			The character who will become the new leader
        */
		void						Leader( CChar *member );
        /** Returns the character that is the party's leader
        @remarks
		This returns the character that is considered to be leader of the party
        @par
        */
		CChar *						Leader( void );
        /** Returns the list of party entries
        @remarks
		This returns the list of party entries as it is stored internally.  We
		should try and minimise the amount this gets used, if we can
        @par
        */
		std::vector< PartyEntry * > *	MemberList( void )		{ return &members; }
        /** Returns the party entry associated with a character
        @remarks
		This returns the party entry associated with a character, if they exist
		in the party.
        @par
		CChar *find				Character to find the party entry for
		int *location			A pointer to the storage of the location, if any,
								so that we can index quickly into the members[]
								to find the character again.  Used internally.
        */
		PartyEntry *				Find( CChar *find, int *location = NULL );
        /** Returns whether a member is in the party or not
        @remarks
		This returns true if the character exists in the party
        @par
		CChar *find				Character to find
        */
		bool						HasMember( CChar *find );
        /** Returns whether it's an NPC party
        @remarks
		This returns true if the party is an NPC party
        @par
        */
		bool						IsNPC( void ) const;
        /** Sets whether it's an NPC party
        @remarks
		This sets if the party is an NPC one or not.  Please note that this
		function does not do any internal manipulation of party members, and
		should not be used on parties that have PCs in them.  PCs and NPCs 
		cannot mix in the same party.
        @par
		bool value					Value to set to
        */
		void						IsNPC( bool value );
        /** Sends the current list of party members
        @remarks
		This will send the current list of party members to the socket in
		question, or the entire party.
        @par
		CSocket *toSendTo			The socket to send the list to, or if NULL,
									then the entire party
        */
		void						SendList( CSocket *toSendTo = NULL );
									Party( bool npc = false );
									Party( CChar *leader, bool npc = false );
	};


/** This class is responsible for the creation and destruction of parties
*/
	class PartyFactory : public Singleton< PartyFactory >
	{
	friend class Party;
	protected:
		CDataList< Party * >		parties;
		std::map< SERIAL, Party * >	partyQuickLook;

		void			AddLookup( Party *toQuickLook, CChar *toSave );
		void			RemoveLookup( CChar *toRemove );
	public:
						PartyFactory();
						~PartyFactory();
		Party *			Create( CChar *leader );
		void			Destroy( CChar *member );
		void			Destroy( Party *toRemove );
		Party *			Get( CChar *member );
		void			CreateInvite( CSocket *inviter );
		void			Kick( CSocket *inviter );
		bool			Load( void );
		bool			Save( void );

        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static PartyFactory& getSingleton( void );
        /** Override standard Singleton retrieval.
        @remarks
        Why do we do this? Well, it's because the Singleton
        implementation is in a .h file, which means it gets compiled
        into anybody who includes it. This is needed for the
        Singleton template to work, but we actually only want it
        compiled into the implementation of the class based on the
        Singleton, not all of them. If we don't change this, we get
        link errors when trying to use the Singleton-based class from
        an outside dll.
        @par
        This method just delegates to the template version anyway,
        but the implementation stays in this single compilation unit,
        preventing link errors.
        */
        static PartyFactory * getSingletonPtr( void );
	};

}

#endif

