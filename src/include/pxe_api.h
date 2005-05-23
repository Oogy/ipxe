#ifndef PXE_API_H
#define PXE_API_H

/** @file
 *
 * Preboot eXecution Environment (PXE) API
 *
 */

#include "pxe_types.h"

/** @addtogroup pxe Preboot eXecution Environment (PXE) API
 *  @{
 */

/** @defgroup pxe_preboot_api PXE Preboot API
 *
 * General high-level functions: #PXENV_UNLOAD_STACK, #PXENV_START_UNDI etc.
 *
 * @{
 */

/** @defgroup pxenv_unload_stack PXENV_UNLOAD_STACK
 *
 *  UNLOAD BASE CODE STACK
 *
 *  @{
 */

/** PXE API function code for pxenv_unload_stack() */
#define	PXENV_UNLOAD_STACK		0x0070

/** Parameter block for pxenv_unload_stack() */
struct s_PXENV_UNLOAD_STACK {
	PXENV_STATUS_t Status;			/**< PXE status code */
	UINT8_t reserved[10];			/**< Must be zero */
} PACKED;

typedef struct s_PXENV_UNLOAD_STACK PXENV_UNLOAD_STACK_t;

extern PXENV_EXIT_t pxenv_unload_stack ( struct s_PXENV_UNLOAD_STACK
					 *unload_stack );

/** @} */ /* pxenv_unload_stack */

/** @defgroup pxenv_get_cached_info PXENV_GET_CACHED_INFO
 *
 *  GET CACHED INFO
 *
 *  @{
 */

/** PXE API function code for pxenv_get_cached_info() */
#define	PXENV_GET_CACHED_INFO		0x0071

/** The client's DHCPDISCOVER packet */
#define PXENV_PACKET_TYPE_DHCP_DISCOVER	1

/** The DHCP server's DHCPACK packet */
#define PXENV_PACKET_TYPE_DHCP_ACK	2

/** The Boot Server's Discover Reply packet
 *
 * This packet contains DHCP option 60 set to "PXEClient", a valid
 * boot file name, and may or may not contain MTFTP options.
 */
#define PXENV_PACKET_TYPE_CACHED_REPLY	3

/** Parameter block for pxenv_get_cached_info() */
struct s_PXENV_GET_CACHED_INFO {
	PXENV_STATUS_t Status;			/**< PXE status code */
	/** Packet type.
	 *
	 * Valid values are #PXENV_PACKET_TYPE_DHCP_DISCOVER,
	 * #PXENV_PACKET_TYPE_DHCP_ACK or #PXENV_PACKET_TYPE_CACHED_REPLY
	 */
	UINT16_t PacketType;
	UINT16_t BufferSize;			/**< Buffer size */
	SEGOFF16_t Buffer;			/**< Buffer address */
	UINT16_t BufferLimit;			/**< Maximum buffer size */
} PACKED;

typedef struct s_PXENV_GET_CACHED_INFO PXENV_GET_CACHED_INFO_t;

#define BOOTP_REQ	1	/**< A BOOTP request packet */
#define BOOTP_REP	2	/**< A BOOTP reply packet */

#define HWTYPE_ETHER		1	/**< Ethernet (10Mb) */
#define HWTYPE_EXP_ETHER	2	/**< Experimental Ethernet (3Mb) */
#define HWTYPE_AX25		3	/**< Amateur Radio AX.25 */
#define HWTYPE_TOKEN_RING	4	/**< Proteon ProNET Token Ring */
#define HWTYPE_CHAOS		5	/**< Chaos */
#define HWTYPE_IEEE		6	/**< IEEE 802 Networks */
#define HWTYPE_ARCNET		7	/**< ARCNET */

/** DHCP broadcast flag
 *
 * Request a broadcast response (DHCPOFFER or DHCPACK) from the DHCP
 * server.
 */
#define BOOTP_BCAST	0x8000

#define VM_RFC1048	0x63825363L	/**< DHCP magic cookie */

/** Format of buffer filled in by pxenv_get_cached_info()
 *
 * This somewhat convoluted data structure simply describes the layout
 * of a DHCP packet.  Refer to RFC2131 section 2 for a full
 * description.
 */
struct bootph {
	/** Message opcode.
	 *
	 * Valid values are #BOOTP_REQ and #BOOTP_REP.
	 */
	UINT8_t opcode;
	/** NIC hardware type.
	 *
	 * Valid values are defined in RFC1010 ("Assigned numbers"),
	 * and are #HWTYPE_ETHER, #HWTYPE_EXP_ETHER, #HWTYPE_AX25,
	 * #HWTYPE_TOKEN_RING, #HWTYPE_CHAOS, #HWTYPE_IEEE and
	 * #HWTYPE_ARCNET.
	 */
	UINT8_t Hardware;
	UINT8_t Hardlen;		/**< MAC address length */
	/** Gateway hops
	 *
	 * Zero in packets sent by the client.  May be non-zero in
	 * replies from the DHCP server, if the reply comes via a DHCP
	 * relay agent.
	 */
	UINT8_t Gatehops;
	UINT32_t ident;			/**< DHCP transaction id (xid) */
	/** Elapsed time
	 *
	 * Number of seconds since the client began the DHCP
	 * transaction.
	 */
	UINT16_t seconds;
	/** Flags
	 *
	 * This is the bitwise-OR of any of the following values:
	 * #BOOTP_BCAST.
	 */
	UINT16_t Flags;
	/** Client IP address
	 *
	 * Set only if the client already has an IP address.
	 */
	IP4_t cip;
	/** Your IP address
	 *
	 * This is the IP address that the server assigns to the
	 * client.
	 */
	IP4_t yip;
	/** Server IP address
	 *
	 * This is the IP address of the BOOTP/DHCP server.
	 */
	IP4_t sip;
	/** Gateway IP address
	 *
	 * This is the IP address of the BOOTP/DHCP relay agent, if
	 * any.  It is @b not (necessarily) the address of the default
	 * gateway for routing purposes.
	 */
	IP4_t gip;
	MAC_ADDR_t CAddr;		/**< Client MAC address */
	UINT8_t Sname[64];		/**< Server host name */
	UINT8_t bootfile[128];		/**< Boot file name */
	/** DHCP options
	 *
	 * Don't ask.  Just laugh.  Then burn a copy of the PXE
	 * specification and send Intel an e-mail asking them if
	 * they've figured out what a "union" does in C yet.
	 */
	union bootph_vendor {
		UINT8_t d[BOOTP_DHCPVEND]; /**< DHCP options */
		/** DHCP options */
		struct bootph_vendor_v {
			/** DHCP magic cookie
			 *
			 * Should have the value #VM_RFC1048.
			 */
			UINT8_t magic[4];
			UINT32_t flags;	/**< BOOTP flags/opcodes */
			/** "End of BOOTP vendor extensions"
			 *
			 * Abandon hope, all ye who consider the
			 * purpose of this field.
			 */
			UINT8_t pad[56];
		} v;
	} vendor;
} PACKED;

typedef struct bootph BOOTPLAYER_t;

extern PXENV_EXIT_t pxenv_get_cached_info ( struct s_PXENV_GET_CACHED_INFO
					    *get_cached_info );

/** @} */ /* pxenv_get_cached_info */

/** @defgroup pxenv_restart_tftp PXENV_RESTART_TFTP
 *
 *  RESTART TFTP
 *
 *  @{
 */

/** PXE API function code for pxenv_restart_tftp() */
#define	PXENV_RESTART_TFTP		0x0073

/** Parameter block for pxenv_restart_tftp() */
struct s_PXENV_TFTP_READ_FILE;

typedef struct s_PXENV_RESTART_TFTP PXENV_RESTART_TFTP_t;

extern PXENV_EXIT_t pxenv_restart_tftp ( struct s_PXENV_TFTP_READ_FILE
					 *restart_tftp );

/** @} */ /* pxenv_restart_tftp */

/** @defgroup pxenv_start_undi PXENV_START_UNDI
 *
 *  START UNDI
 *
 *  @{
 */

/** PXE API function code for pxenv_start_undi() */
#define	PXENV_START_UNDI		0x0000

/** Parameter block for pxenv_start_undi() */
struct s_PXENV_START_UNDI {
	PXENV_STATUS_t Status;			/**< PXE status code */
	/** %ax register as passed to the Option ROM initialisation routine.
	 *
	 * For a PCI device, this should contain the bus:dev:fn value
	 * that uniquely identifies the PCI device in the system.  For
	 * a non-PCI device, this field is not defined.
	 */
	UINT16_t AX;
	/** %bx register as passed to the Option ROM initialisation routine.
	 *
	 * For an ISAPnP device, this should contain the Card Select
	 * Number assigned to the ISAPnP card.  For non-ISAPnP
	 * devices, this should contain 0xffff.
	 */
	UINT16_t BX;
	/** %dx register as passed to the Option ROM initialisation routine.
	 *
	 * For an ISAPnP device, this should contain the ISAPnP Read
	 * Port address as currently set in all ISAPnP cards.  If
	 * there are no ISAPnP cards, this should contain 0xffff.  (If
	 * this is a non-ISAPnP device, but there are ISAPnP cards in
	 * the system, this value is not well defined.)
	 */
	UINT16_t DX;
	/** %di register as passed to the Option ROM initialisation routine.
	 *
	 * This contains the #OFF16_t portion of a struct #s_SEGOFF16
	 * that points to the System BIOS Plug and Play Installation
	 * Check Structure.  (Refer to section 4.4 of the Plug and
	 * Play BIOS specification for a description of this
	 * structure.)
	 *
	 * @note The PXE specification defines the type of this field
	 * as #UINT16_t.  For x86, #OFF16_t and #UINT16_t are
	 * equivalent anyway; for other architectures #OFF16_t makes
	 * more sense.
	 */
	OFF16_t DI;
	/** %es register as passed to the Option ROM initialisation routine.
	 *
	 * This contains the #SEGSEL_t portion of a struct #s_SEGOFF16
	 * that points to the System BIOS Plug and Play Installation
	 * Check Structure.  (Refer to section 4.4 of the Plug and
	 * Play BIOS specification for a description of this
	 * structure.)
	 *
	 * @note The PXE specification defines the type of this field
	 * as #UINT16_t.  For x86, #SEGSEL_t and #UINT16_t are
	 * equivalent anyway; for other architectures #SEGSEL_t makes
	 * more sense.
	 */
	SEGSEL_t ES;
} PACKED;

typedef struct s_PXENV_START_UNDI PXENV_START_UNDI_t;

extern PXENV_EXIT_t pxenv_start_undi ( struct s_PXENV_START_UNDI *start_undi );

/** @} */ /* pxenv_start_undi */

/** @defgroup pxenv_stop_undi PXENV_STOP_UNDI
 *
 *  STOP UNDI
 *
 *  @{
 */

/** PXE API function code for pxenv_stop_undi() */
#define	PXENV_STOP_UNDI			0x0015

/** Parameter block for pxenv_stop_undi() */
struct s_PXENV_STOP_UNDI {
	PXENV_STATUS_t Status;			/**< PXE status code */
} PACKED;

typedef struct s_PXENV_STOP_UNDI PXENV_STOP_UNDI_t;

extern PXENV_EXIT_t pxenv_stop_undi ( struct s_PXENV_STOP_UNDI *stop_undi );

/** @} */ /* pxenv_stop_undi */

/** @defgroup pxenv_start_base PXENV_START_BASE
 *
 *  START BASE
 *
 *  @{
 */

/** PXE API function code for pxenv_start_base() */
#define	PXENV_START_BASE		0x0075

/** Parameter block for pxenv_start_base() */
struct s_PXENV_START_BASE {
	PXENV_STATUS_t Status;			/**< PXE status code */
} PACKED;

typedef struct s_PXENV_START_BASE PXENV_START_BASE_t;

extern PXENV_EXIT_t pxenv_start_base ( struct s_PXENV_START_BASE *start_base );

/** @} */ /* pxenv_start_base */

/** @defgroup pxenv_stop_base PXENV_STOP_BASE
 *
 *  STOP BASE
 *
 *  @{
 */

/** PXE API function code for pxenv_stop_base() */
#define	PXENV_STOP_BASE			0x0076

/** Parameter block for pxenv_stop_base() */
struct s_PXENV_STOP_BASE {
	PXENV_STATUS_t Status;			/**< PXE status code */
} PACKED;

typedef struct s_PXENV_STOP_BASE PXENV_STOP_BASE_t;

extern PXENV_EXIT_t pxenv_stop_base ( struct s_PXENV_STOP_BASE *stop_base );

/** @} */ /* pxenv_stop_base */

/** @} */ /* pxe_preboot_api */

/** @defgroup pxe_tftp_api PXE TFTP API
 *
 * Download files via TFTP or MTFTP
 *
 * @{
 */

/** @defgroup pxenv_tftp_open PXENV_TFTP_OPEN
 *
 *  TFTP OPEN
 *
 *  @{
 */

/** PXE API function code for pxenv_tftp_open() */
#define	PXENV_TFTP_OPEN			0x0020

/** Parameter block for pxenv_tftp_open() */
struct s_PXENV_TFTP_OPEN {
	PXENV_STATUS_t Status;			/**< PXE status code */
	IP4_t ServerIPAddress;			/**< TFTP server IP address */
	IP4_t GatewayIPAddress;			/**< Relay agent IP address */
	UINT8_t FileName[128];			/**< File name */
	UDP_PORT_t TFTPPort;			/**< TFTP server UDP port */
	/** Requested size of TFTP packets
	 *
	 * This is the TFTP "blksize" option.  This must be at least
	 * 512, according to the PXE specification, though no reason
	 * is offered.
	 */
	UINT16_t PacketSize;
} PACKED;

typedef struct s_PXENV_TFTP_OPEN PXENV_TFTP_OPEN_t;

extern PXENV_EXIT_t pxenv_tftp_open ( struct s_PXENV_TFTP_OPEN *tftp_open );

/** @} */ /* pxenv_tftp_open */

/** @defgroup pxenv_tftp_close PXENV_TFTP_CLOSE
 *
 *  TFTP CLOSE
 *
 *  @{
 */

/** PXE API function code for pxenv_tftp_close() */
#define	PXENV_TFTP_CLOSE		0x0021

/** Parameter block for pxenv_tftp_close() */
struct s_PXENV_TFTP_CLOSE {
	PXENV_STATUS_t Status;			/**< PXE status code */
} PACKED;

typedef struct s_PXENV_TFTP_CLOSE PXENV_TFTP_CLOSE_t;

extern PXENV_EXIT_t pxenv_tftp_close ( struct s_PXENV_TFTP_CLOSE *tftp_close );

/** @} */ /* pxenv_tftp_close */

/** @defgroup pxenv_tftp_read PXENV_TFTP_READ
 *
 *  TFTP READ
 *
 *  @{
 */

/** PXE API function code for pxenv_tftp_read() */
#define	PXENV_TFTP_READ			0x0022

/** Parameter block for pxenv_tftp_read() */
struct s_PXENV_TFTP_READ {
	PXENV_STATUS_t Status;			/**< PXE status code */
	UINT16_t PacketNumber;			/**< TFTP packet number */
	UINT16_t BufferSize;			/**< Size of data buffer */
	SEGOFF16_t Buffer;			/**< Address of data buffer */
} PACKED;

typedef struct s_PXENV_TFTP_READ PXENV_TFTP_READ_t;

extern PXENV_EXIT_t pxenv_tftp_read ( struct s_PXENV_TFTP_READ *tftp_read );

/** @} */ /* pxenv_tftp_read */

/** @defgroup pxenv_tftp_read_file PXENV_TFTP_READ_FILE
 *
 *  TFTP/MTFTP READ FILE
 *
 *  @{
 */

/** PXE API function code for pxenv_tftp_read_file() */
#define	PXENV_TFTP_READ_FILE		0x0023

/** Parameter block for pxenv_tftp_read_file() */
struct s_PXENV_TFTP_READ_FILE {
	PXENV_STATUS_t Status;			/**< PXE status code */
	UINT8_t FileName[128];			/**< File name */
	UINT32_t BufferSize;			/**< Size of data buffer */
	ADDR32_t Buffer;			/**< Address of data buffer */
	IP4_t ServerIPAddress;			/**< TFTP server IP address */
	IP4_t GatewayIPAddress;			/**< Relay agent IP address */
	/** File multicast IP address */
	IP4_t McastIPAddress;
	/** Client multicast listening port */
	UDP_PORT_t TFTPClntPort;
	/** Server multicast listening port */
	UDP_PORT_t TFTPSrvPort;
	/** Timeout for receiving data or ACK packets */
	UINT16_t TFTPOpenTimeOut;
	/** Timeout before issuing MTFTP open */
	UINT16_t TFTPReopenDelay;
} PACKED;

typedef struct s_PXENV_TFTP_READ_FILE PXENV_TFTP_READ_FILE_t;

extern PXENV_EXIT_t pxenv_tftp_read_file ( struct s_PXENV_TFTP_READ_FILE
					   *tftp_read_file );

/** @} */ /* pxenv_tftp_read_file */

/** @defgroup pxenv_tftp_get_fsize PXENV_TFTP_GET_FSIZE
 *
 *  TFTP GET FILE SIZE
 *
 *  @{
 */

/** PXE API function code for pxenv_tftp_get_fsize() */
#define	PXENV_TFTP_GET_FSIZE		0x0025

/** Parameter block for pxenv_tftp_get_fsize() */
struct s_PXENV_TFTP_GET_FSIZE {
	PXENV_STATUS_t Status;			/**< PXE status code */
	IP4_t ServerIPAddress;			/**< TFTP server IP address */
	IP4_t GatewayIPAddress;			/**< Relay agent IP address */
	UINT8_t FileName[128];			/**< File name */
	UINT32_t FileSize;			/**< Size of the file */
} PACKED;

typedef struct s_PXENV_TFTP_GET_FSIZE PXENV_TFTP_GET_FSIZE_t;

extern PXENV_EXIT_t pxenv_tftp_get_fsize ( struct s_PXENV_TFTP_GET_FSIZE
					   *get_fsize );

/** @} */ /* pxenv_tftp_get_fsize */

/** @} */ /* pxe_tftp_api */

/** @defgroup pxe_udp_api PXE UDP API
 *
 * Transmit and receive UDP packets
 *
 * @{
 */

/** @defgroup pxenv_udp_open PXENV_UDP_OPEN
 *
 *  UDP OPEN
 *
 *  @{
 */

/** PXE API function code for pxenv_udp_open() */
#define	PXENV_UDP_OPEN			0x0030

/** Parameter block for pxenv_udp_open() */
struct s_PXENV_UDP_OPEN {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	IP4_t		src_ip;		/**< IP address of this station */
} PACKED;

typedef struct s_PXENV_UDP_OPEN PXENV_UDP_OPEN_t;

extern PXENV_EXIT_t pxenv_udp_open ( struct s_PXENV_UDP_OPEN *udp_open );

/** @} */ /* pxenv_udp_open */

/** @defgroup pxenv_udp_close PXENV_UDP_CLOSE
 *
 *  UDP CLOSE
 *
 *  @{
 */

/** PXE API function code for pxenv_udp_close() */
#define	PXENV_UDP_CLOSE			0x0031

/** Parameter block for pxenv_udp_close() */
struct s_PXENV_UDP_CLOSE {
	PXENV_STATUS_t	Status;		/**< PXE status code */
} PACKED;

typedef struct s_PXENV_UDP_CLOSE PXENV_UDP_CLOSE_t;

extern PXENV_EXIT_t pxenv_udp_close ( struct s_PXENV_UDP_CLOSE *udp_close );

/** @} */ /* pxenv_udp_close */

/** @defgroup pxenv_udp_write PXENV_UDP_WRITE
 *
 *  UDP WRITE
 *
 *  @{
 */

/** PXE API function code for pxenv_udp_write() */
#define	PXENV_UDP_WRITE			0x0033

/** Parameter block for pxenv_udp_write() */
struct s_PXENV_UDP_WRITE {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	IP4_t		ip;		/**< Destination IP address */
	IP4_t		gw;		/**< Relay agent IP address */
	UDP_PORT_t	src_port;	/**< Source UDP port */
	UDP_PORT_t	dst_port;	/**< Destination UDP port */
	UINT16_t	buffer_size;	/**< UDP payload buffer size */
	SEGOFF16_t	buffer;		/**< UDP payload buffer address */
} PACKED;

typedef struct s_PXENV_UDP_WRITE PXENV_UDP_WRITE_t;

extern PXENV_EXIT_t pxenv_udp_write ( struct s_PXENV_UDP_WRITE *udp_write );

/** @} */ /* pxenv_udp_write */

/** @defgroup pxenv_udp_read PXENV_UDP_READ
 *
 *  UDP READ
 *
 *  @{
 */

/** PXE API function code for pxenv_udp_read() */
#define	PXENV_UDP_READ			0x0032

/** Parameter block for pxenv_udp_read() */
struct s_PXENV_UDP_READ {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	IP4_t		src_ip;		/**< Source IP address */
	IP4_t		dest_ip;	/**< Destination IP address */
	UDP_PORT_t	s_port;		/**< Source UDP port */
	UDP_PORT_t	d_port;		/**< Destination UDP port */
	UINT16_t	buffer_size;	/**< UDP payload buffer size */
	SEGOFF16_t	buffer;		/**< UDP payload buffer address */
} PACKED;

typedef struct s_PXENV_UDP_READ PXENV_UDP_READ_t;

extern PXENV_EXIT_t pxenv_udp_read ( struct s_PXENV_UDP_READ *udp_read );

/** @} */ /* pxenv_udp_read */

/** @} */ /* pxe_udp_api */

/** @defgroup pxe_undi_api PXE UNDI API
 *
 * Direct control of the network interface card
 *
 * @{
 */

/** @defgroup pxenv_undi_startup PXENV_UNDI_STARTUP
 *
 *  UNDI STARTUP
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_startup() */
#define	PXENV_UNDI_STARTUP		0x0001

/** Parameter block for pxenv_undi_startup() */
struct s_PXENV_UNDI_STARTUP {
	PXENV_STATUS_t	Status;		/**< PXE status code */
} PACKED;

typedef struct s_PXENV_UNDI_STARTUP PXENV_UNDI_STARTUP_t;

extern PXENV_EXIT_t pxenv_undi_startup ( struct s_PXENV_UNDI_STARTUP
					 *undi_startup );

/** @} */ /* pxenv_undi_startup */

/** @defgroup pxenv_undi_cleanup PXENV_UNDI_CLEANUP
 *
 *  UNDI CLEANUP
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_cleanup() */
#define	PXENV_UNDI_CLEANUP		0x0002

/** Parameter block for pxenv_undi_cleanup() */
struct s_PXENV_UNDI_CLEANUP {
	PXENV_STATUS_t	Status;		/**< PXE status code */
} PACKED;

typedef struct s_PXENV_UNDI_CLEANUP PXENV_UNDI_CLEANUP_t;

extern PXENV_EXIT_t pxenv_undi_cleanup ( struct s_PXENV_UNDI_CLEANUP
					 *undi_cleanup );

/** @} */ /* pxenv_undi_cleanup */

/** @defgroup pxenv_undi_initialize PXENV_UNDI_INITIALIZE
 *
 *  UNDI INITIALIZE
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_initialize() */
#define	PXENV_UNDI_INITIALIZE		0x0003

/** Parameter block for pxenv_undi_initialize() */
struct s_PXENV_UNDI_INITIALIZE {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	/** NDIS 2.0 configuration information, or NULL
	 *
	 * This is a pointer to the data structure returned by the
	 * NDIS 2.0 GetProtocolManagerInfo() API call.  The data
	 * structure is documented, in a rather haphazard way, in
	 * section 4-17 of the NDIS 2.0 specification.
	 */
	ADDR32_t ProtocolIni;
	UINT8_t reserved[8];		/**< Must be zero */
} PACKED;

typedef struct s_PXENV_UNDI_INITIALIZE PXENV_UNDI_INITIALIZE_t;

extern PXENV_EXIT_t pxenv_undi_initialize ( struct s_PXENV_UNDI_INITIALIZE
					    *undi_initialize );

/** @} */ /* pxenv_undi_initialize */

/** @defgroup pxenv_undi_reset_adapter PXENV_UNDI_RESET_ADAPTER
 *
 *  UNDI RESET ADAPTER
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_reset_adapter() */
#define	PXENV_UNDI_RESET_ADAPTER	0x0004

/** Maximum number of multicast MAC addresses */
#define MAXNUM_MCADDR	8

/** List of multicast MAC addresses */
struct s_PXENV_UNDI_MCAST_ADDRESS {
	/** Number of multicast MAC addresses */
	UINT16_t MCastAddrCount;
	/** List of up to #MAXNUM_MCADDR multicast MAC addresses */
	MAC_ADDR_t McastAddr[MAXNUM_MCADDR];
} PACKED;

typedef struct s_PXENV_UNDI_MCAST_ADDRESS PXENV_UNDI_MCAST_ADDRESS_t;

/** Parameter block for pxenv_undi_reset_adapter() */
struct s_PXENV_UNDI_RESET_ADAPTER {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	/** Multicast MAC addresses */
	struct s_PXENV_UNDI_MCAST_ADDRESS R_Mcast_Buf;
} PACKED;

typedef struct s_PXENV_UNDI_RESET_ADAPTER PXENV_UNDI_RESET_ADAPTER_t;

extern PXENV_EXIT_t pxenv_undi_reset_adapter ( struct s_PXENV_UNDI_RESET
					       *undi_reset_adapter );

/** @} */ /* pxenv_undi_reset_adapter */

/** @defgroup pxenv_undi_shutdown PXENV_UNDI_SHUTDOWN
 *
 *  UNDI SHUTDOWN
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_shutdown() */
#define	PXENV_UNDI_SHUTDOWN		0x0005

/** Parameter block for pxenv_undi_shutdown() */
struct s_PXENV_UNDI_SHUTDOWN {
	PXENV_STATUS_t	Status;		/**< PXE status code */
} PACKED;

typedef struct s_PXENV_UNDI_SHUTDOWN PXENV_UNDI_SHUTDOWN_t;

extern PXENV_EXIT_t pxenv_undi_shutdown ( struct s_PXENV_UNDI_SHUTDOWN
					  *undi_shutdown );

/** @} */ /* pxenv_undi_shutdown */

/** @defgroup pxenv_undi_open PXENV_UNDI_OPEN
 *
 *  UNDI OPEN
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_open() */
#define	PXENV_UNDI_OPEN			0x0006

/** Accept "directed" packets
 *
 * These are packets addresses to either this adapter's MAC address or
 * to any of the configured multicast MAC addresses (see
 * #s_PXENV_UNDI_MCAST_ADDRESS).
 */
#define FLTR_DIRECTED	0x0001
/** Accept broadcast packets */
#define FLTR_BRDCST	0x0002
/** Accept all packets; listen in promiscuous mode */
#define FLTR_PRMSCS	0x0004
/** Accept source-routed packets */
#define FLTR_SRC_RTG	0x0008

/** Parameter block for pxenv_undi_open() */
struct s_PXENV_UNDI_OPEN {
	PXENV_STATUS_t	Status;		/**< PXE status code */
	/** Open flags as defined in NDIS 2.0
	 *
	 * This is the OpenOptions field as passed to the NDIS 2.0
	 * OpenAdapter() API call.  It is defined to be "adapter
	 * specific", though 0 is guaranteed to be a valid value.
	 */
	UINT16_t OpenFlag;
	/** Receive packet filter
	 *
	 * This is the bitwise-OR of any of the following flags:
	 * #FLTR_DIRECTED, #FLTR_BRDCST, #FLTR_PRMSCS and
	 * #FLTR_SRC_RTG.
	 */
	UINT16_t PktFilter;
	/** Multicast MAC addresses */
	struct s_PXENV_UNDI_MCAST_ADDRESS R_Mcast_Buf;
} PACKED;

typedef struct s_PXENV_UNDI_OPEN PXENV_UNDI_OPEN_t;

extern PXENV_EXIT_t pxenv_undi_open ( struct s_PXENV_UNDI_OPEN *undi_open );

/** @} */ /* pxenv_undi_open */

/** @defgroup pxenv_undi_close PXENV_UNDI_CLOSE
 *
 *  UNDI CLOSE
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_close() */
#define	PXENV_UNDI_CLOSE		0x0007

/** Parameter block for pxenv_undi_close() */
struct s_PXENV_UNDI_CLOSE {
	PXENV_STATUS_t	Status;		/**< PXE status code */
} PACKED;

typedef struct s_PXENV_UNDI_CLOSE PXENV_UNDI_CLOSE_t;

extern PXENV_EXIT_t pxenv_undi_close ( struct s_PXENV_UNDI_CLOSE *undi_close );

/** @} */ /* pxenv_undi_close */

/** @defgroup pxenv_undi_transmit PXENV_UNDI_TRANSMIT
 *
 *  UNDI TRANSMIT PACKET
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_transmit() */
#define	PXENV_UNDI_TRANSMIT		0x0008

/** Parameter block for pxenv_undi_transmit() */
struct s_PXENV_UNDI_TRANSMIT {
} PACKED;

typedef struct s_PXENV_UNDI_TRANSMIT PXENV_UNDI_TRANSMIT_t;

extern PXENV_EXIT_t pxenv_undi_transmit ( struct s_PXENV_UNDI_TRANSMIT
					  *undi_transmit );

/** @} */ /* pxenv_undi_transmit */

/** @defgroup pxenv_undi_set_mcast_address PXENV_UNDI_SET_MCAST_ADDRESS
 *
 *  UNDI SET MULTICAST ADDRESS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_set_mcast_address() */
#define	PXENV_UNDI_SET_MCAST_ADDRESS	0x0009

/** Parameter block for pxenv_undi_set_mcast_address() */
struct s_PXENV_UNDI_SET_MCAST_ADDRESS {
} PACKED;

typedef struct s_PXENV_UNDI_SET_MCAST_ADDRESS PXENV_UNDI_SET_MCAST_ADDRESS_t;

extern PXENV_EXIT_t pxenv_undi_set_mcast_address (
	       struct s_PXENV_UNDI_SET_MCAST_ADDRESS *undi_set_mcast_address );

/** @} */ /* pxenv_undi_set_mcast_address */

/** @defgroup pxenv_undi_set_station_address PXENV_UNDI_SET_STATION_ADDRESS
 *
 *  UNDI SET STATION ADDRESS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_set_station_address() */
#define	PXENV_UNDI_SET_STATION_ADDRESS	0x000a

/** Parameter block for pxenv_undi_set_station_address() */
struct s_PXENV_UNDI_SET_STATION_ADDRESS {
} PACKED;

typedef struct s_PXENV_UNDI_SET_STATION_ADDRESS PXENV_UNDI_SET_STATION_ADDRESS_t;

extern PXENV_EXIT_t pxenv_undi_set_station_address (
	   struct s_PXENV_UNDI_SET_STATION_ADDRESS *undi_set_station_address );

/** @} */ /* pxenv_undi_set_station_address */

/** @defgroup pxenv_undi_set_packet_filter PXENV_UNDI_SET_PACKET_FILTER
 *
 *  UNDI SET PACKET FILTER
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_set_packet_filter() */
#define	PXENV_UNDI_SET_PACKET_FILTER	0x000b

/** Parameter block for pxenv_undi_set_packet_filter() */
struct s_PXENV_UNDI_SET_PACKET_FILTER {
} PACKED;

typedef struct s_PXENV_UNDI_SET_PACKET_FILTER PXENV_UNDI_SET_PACKET_FILTER_t;

extern PXENV_EXIT_t pxenv_undi_set_packet_filter (
	       struct s_PXENV_UNDI_SET_PACKET_FILTER *undi_set_packet_filter );

/** @} */ /* pxenv_undi_set_packet_filter */

/** @defgroup pxenv_undi_get_information PXENV_UNDI_GET_INFORMATION
 *
 *  UNDI GET INFORMATION
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_information() */
#define	PXENV_UNDI_GET_INFORMATION	0x000c

/** Parameter block for pxenv_undi_get_information() */
struct s_PXENV_UNDI_GET_INFORMATION {
} PACKED;

typedef struct s_PXENV_UNDI_GET_INFORMATION PXENV_UNDI_GET_INFORMATION_t;

extern PXENV_EXIT_t pxenv_undi_get_information (
		   struct s_PXENV_UNDI_GET_INFORMATION *undi_get_information );

/** @} */ /* pxenv_undi_get_information */

/** @defgroup pxenv_undi_get_statistics PXENV_UNDI_GET_STATISTICS
 *
 *  UNDI GET STATISTICS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_statistics() */
#define	PXENV_UNDI_GET_STATISTICS	0x000d

/** Parameter block for pxenv_undi_get_statistics() */
struct s_PXENV_UNDI_GET_STATISTICS {
} PACKED;

typedef struct s_PXENV_UNDI_GET_STATISTICS PXENV_UNDI_GET_STATISTICS_t;

extern PXENV_EXIT_t pxenv_undi_get_statistics (
		     struct s_PXENV_UNDI_GET_STATISTICS *undi_get_statistics );

/** @} */ /* pxenv_undi_get_statistics */

/** @defgroup pxenv_undi_clear_statistics PXENV_UNDI_CLEAR_STATISTICS
 *
 *  UNDI CLEAR STATISTICS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_clear_statistics() */
#define	PXENV_UNDI_CLEAR_STATISTICS	0x000e

/** Parameter block for pxenv_undi_clear_statistics() */
struct s_PXENV_UNDI_CLEAR_STATISTICS {
} PACKED;

typedef struct s_PXENV_UNDI_CLEAR_STATISTICS PXENV_UNDI_CLEAR_STATISTICS_t;

extern PXENV_EXIT_t pxenv_undi_clear_statistics (
		 struct s_PXENV_UNDI_CLEAR_STATISTICS *undi_clear_statistics );

/** @} */ /* pxenv_undi_clear_statistics */

/** @defgroup pxenv_undi_initiate_diags PXENV_UNDI_INITIATE_DIAGS
 *
 *  UNDI INITIATE DIAGS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_initiate_diags() */
#define	PXENV_UNDI_INITIATE_DIAGS	0x000f

/** Parameter block for pxenv_undi_initiate_diags() */
struct s_PXENV_UNDI_INITIATE_DIAGS {
} PACKED;

typedef struct s_PXENV_UNDI_INITIATE_DIAGS PXENV_UNDI_INITIATE_DIAGS_t;

extern PXENV_EXIT_t pxenv_undi_initiate_diags (
		     struct s_PXENV_UNDI_INITIATE_DIAGS *undi_initiate_diags );

/** @} */ /* pxenv_undi_initiate_diags */

/** @defgroup pxenv_undi_force_interrupt PXENV_UNDI_FORCE_INTERRUPT
 *
 *  UNDI FORCE INTERRUPT
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_force_interrupt() */
#define	PXENV_UNDI_FORCE_INTERRUPT	0x0010

/** Parameter block for pxenv_undi_force_interrupt() */
struct s_PXENV_UNDI_FORCE_INTERRUPT {
} PACKED;

typedef struct s_PXENV_UNDI_FORCE_INTERRUPT PXENV_UNDI_FORCE_INTERRUPT_t;

extern PXENV_EXIT_t pxenv_undi_force_interrupt (
		   struct s_PXENV_UNDI_FORCE_INTERRUPT *undi_force_interrupt );

/** @} */ /* pxenv_undi_force_interrupt */

/** @defgroup pxenv_undi_get_mcast_address PXENV_UNDI_GET_MCAST_ADDRESS
 *
 *  UNDI GET MULTICAST ADDRESS
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_mcast_address() */
#define	PXENV_UNDI_GET_MCAST_ADDRESS	0x0011

/** Parameter block for pxenv_undi_get_mcast_address() */
struct s_PXENV_UNDI_GET_MCAST_ADDRESS {
} PACKED;

typedef struct s_PXENV_UNDI_GET_MCAST_ADDRESS PXENV_UNDI_GET_MCAST_ADDRESS_t;

extern PXENV_EXIT_t pxenv_undi_get_mcast_address (
	       struct s_PXENV_UNDI_GET_MCAST_ADDRESS *undi_get_mcast_address );

/** @} */ /* pxenv_undi_get_mcast_address */

/** @defgroup pxenv_undi_get_nic_type PXENV_UNDI_GET_NIC_TYPE
 *
 *  UNDI GET NIC TYPE
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_nic_type() */
#define	PXENV_UNDI_GET_NIC_TYPE		0x0012

/** Parameter block for pxenv_undi_get_nic_type() */
struct s_PXENV_UNDI_GET_NIC_TYPE {
} PACKED;

typedef struct s_PXENV_UNDI_GET_NIC_TYPE PXENV_UNDI_GET_NIC_TYPE_t;

extern PXENV_EXIT_t pxenv_undi_get_nic_type ( 
			 struct s_PXENV_UNDI_GET_NIC_TYPE *undi_get_nic_type );

/** @} */ /* pxenv_undi_get_nic_type */

/** @defgroup pxenv_undi_get_iface_info PXENV_UNDI_GET_IFACE_INFO
 *
 *  UNDI GET IFACE INFO
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_iface_info() */
#define	PXENV_UNDI_GET_IFACE_INFO	0x0013

/** Parameter block for pxenv_undi_get_iface_info() */
struct s_PXENV_UNDI_GET_IFACE_INFO {
} PACKED;

typedef struct s_PXENV_UNDI_GET_IFACE_INFO PXENV_UNDI_GET_IFACE_INFO_t;

extern PXENV_EXIT_t pxenv_undi_get_iface_info (
		     struct s_PXENV_UNDI_GET_IFACE_INFO *undi_get_iface_info );

/** @} */ /* pxenv_undi_get_iface_info */

/** @defgroup pxenv_undi_get_state PXENV_UNDI_GET_STATE
 *
 *  UNDI GET STATE
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_get_state() */
#define PXENV_UNDI_GET_STATE		0x0015

/** Parameter block for pxenv_undi_get_state() */
struct s_PXENV_UNDI_GET_STATE {
} PACKED;

typedef struct s_PXENV_UNDI_GET_STATE PXENV_UNDI_GET_STATE_t;

extern PXENV_EXIT_t pxenv_undi_get_state ( struct s_PXENV_UNDI_GET_STATE
					   *undi_get_state );

/** @} */ /* pxenv_undi_get_state */

/** @defgroup pxenv_undi_isr PXENV_UNDI_ISR
 *
 *  UNDI ISR
 *
 *  @{
 */

/** PXE API function code for pxenv_undi_isr() */
#define	PXENV_UNDI_ISR			0x0014

/** Parameter block for pxenv_undi_isr() */
struct s_PXENV_UNDI_ISR {
} PACKED;

typedef struct s_PXENV_UNDI_ISR PXENV_UNDI_ISR_t;

extern PXENV_EXIT_t pxenv_undi_isr ( struct s_PXENV_UNDI_ISR *undi_isr );

/** @} */ /* pxenv_undi_isr */

/** @} */ /* pxe_undi_api */

/** @} */ /* pxe */

#endif /* PXE_API_H */
