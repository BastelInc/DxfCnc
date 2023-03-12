#pragma once

#pragma pack(push, 1)


#define MULTICAST  0
enum { UNBOUND, SUBNET_NODE, NEURON_ID, BROADCAST,GROUPADD = 0x80};
extern const char * AddrType [4] ;
enum {
no_action = 0,
both_cs_recalc = 1,
cnfg_cs_recalc = 4,
only_reset = 8,
both_cs_recalc_reset = 9,
cnfg_cs_recalc_reset = 0xC,
};
#define NEURON_ID_LEN	 6
#define ID_STR_LEN	 8
#define DOMAIN_ID_LEN	 6
#define AUTH_KEY_LEN	 6
#define LOCATION_LEN 6
#define NUM_COMM_PARAMS 7

typedef struct NM_service_pin_msg {
    unsigned char    neuron_id[ NEURON_ID_LEN ];
    unsigned char    id_string[ ID_STR_LEN ];
} NM_service_pin_msg;

typedef unsigned char addr_type;

typedef struct group_struct {
	unsigned char size	  : 7;	// group size (0 => huge group)
	unsigned char type	  : 1;	// 1 => group
    // --- BYTE SWAP----
	unsigned char member	  : 7;	// member num (if huge group, only use 0)
	unsigned char domain	  : 1;	// domain index
    // --- BYTE SWAP----
	unsigned char retry	  : 4;	// retry count
	unsigned char rpt_timer: 4;	// unackd_rpt timer
    // --- BYTE SWAP----
	unsigned char tx_timer : 4;	// transmit timer index
	unsigned char rcv_timer: 4;	// receive timer index
    // --- BYTE ----
	unsigned char group;		// group ID
} group_struct;

typedef struct snode_struct {
	addr_type type;		// SUBNET_NODE
	unsigned char node	  : 7;	// node number
	unsigned char domain	  : 1;	// domain index
    // --- BYTE SWAP----
	unsigned char retry	  : 4;	// retry count
	unsigned char rpt_timer: 4;	// unackd_rpt timer
    // --- BYTE SWAP----
	unsigned char tx_timer : 4;	// transmit timer index
	unsigned char	  : 4;
    // --- BYTE ----
	unsigned char subnet;	// subnet ID
} snode_struct;

typedef struct bcast_struct {
	addr_type type;		// BROADCAST
	unsigned char backlog  : 6;	// backlog override value
	unsigned char	       : 1;
	unsigned char domain   : 1;	// domain index
    // --- BYTE SWAP----
	unsigned char retry	  : 4;	// retry count
	unsigned char rpt_timer: 4;	// unackd_rpt timer
    // --- BYTE SWAP----
	unsigned char tx_timer: 4;	// transmit timer index
	unsigned char	      : 4;
    // --- BYTE ----
	unsigned char subnet;	// subnet ID (0 => domain broadcast)
} bcast_struct;







typedef struct nrnid_struct {
	addr_type	type;
	unsigned char		  : 7;
	unsigned char	domain	  : 1;
    // --- BYTE SWAP----
	unsigned char	retry	  : 4;
	unsigned char	rpt_timer : 4;
    // --- BYTE SWAP----
	unsigned char	tx_timer  : 4;
	unsigned char		  : 4;
    // --- BYTE ----
	unsigned char	subnet;
	unsigned char	nid [NEURON_ID_LEN];
} nrnid_struct;


typedef	union msg_out_addr {
	addr_type	no_address;	// UNBOUND (i.e. 0) if no address */
			// UNBOUND & addr_type are defined in <ADDRDEFS.H>
	group_struct	group;	// Defined in <ADDRDEFS.H>
	snode_struct	snode;	// Defined in <ADDRDEFS.H>
	nrnid_struct	nrnid;
	bcast_struct	bcast;	// Defined in <ADDRDEFS.H>
} msg_out_addr;

// Typedef for 'msg_in_addr', which is the type of the field 'msg_in.addr'.


typedef struct msg_in_addr {
    // --- BYTE SWAP----
	unsigned char	format		: 6;	// NOT the 'addr_type' enum.
	unsigned char	flex_domain	: 1;
	unsigned char	domain		: 1;
    // --- BYTE ----
			// INSTEAD:	0 => Broadcast,	   1 => Group
			//		2 => Subnet/Node,  3 => Neuron-ID
	struct {
		unsigned char subnet;
    // --- BYTE SWAP----
		unsigned char node	: 7;
		unsigned char 	: 1;
    // --- BYTE ----
	} src_addr;
	union {
		unsigned char bcast_subnet;
		unsigned char group;
		struct {
			unsigned char subnet;
    // --- BYTE SWAP----
			unsigned char node	: 7;
			unsigned char 	: 1;
    // --- BYTE ----
		} snode;
		struct {
			unsigned char subnet;
			unsigned char nid [NEURON_ID_LEN];
		} nrnid;
	} dest_addr;
} msg_in_addr;

// Typedef for 'resp_in_addr', which is the type of the field 'resp_in.addr'.


typedef struct resp_in_addr {
    // --- BYTE SWAP----
	unsigned char	: 6;
	unsigned char	flex_domain	: 1;
	unsigned char	domain		: 1;
    // --- ----
	struct {
		unsigned char subnet;
    // --- BYTE SWAP----
		unsigned char node		: 7;
		unsigned char is_snode	: 1; /* 0=>group resp, 1=>snode resp */
    // --- BYTE ----
	} src_addr;
	union {
		struct {
			unsigned char subnet;
    // --- BYTE SWAP----
			unsigned char node	: 7;
			unsigned char 	: 1;
    // --- BYTE ----
		} snode;
		struct {
			unsigned char subnet;
    // --- BYTE SWAP----
			unsigned char node	: 7;
			unsigned char	: 1;
    // --- BYTE SWAP----
			unsigned char group;
    // --- BYTE SWAP----
			unsigned char member	: 6;
			unsigned char	: 2;
    // --- BYTE ----
		} group;
	} dest_addr;
} resp_in_addr;



#ifndef NM_MSG_CODE
  #define NM_MSG_CODE
  typedef enum service_type { ACKD, UNACKD_RPT, UNACKD, REQUEST } service_type;


// Define network management message codes
extern const char * NM_MsgNames[32];
typedef enum NM_msg_code {
    NM_query_id		    = 0x1,
    NM_respond_to_query = 0x2,
    NM_update_domain	= 0x3,
    NM_leave_domain	    = 0x4,
    NM_update_key	    = 0x5,
    NM_update_addr	    = 0x6,
    NM_query_addr	    = 0x7,
    NM_query_nv_cnfg	= 0x8,
    NM_update_group_addr= 0x9,
    NM_query_domain	    = 0xA,
    NM_update_nv_cnfg	= 0xB,
    NM_set_node_mode	= 0xC,
    NM_read_memory	    = 0xD,
    NM_write_memory	    = 0xE,
    NM_checksum_recalc	= 0xF,
    NM_install		    = 0x10,
    NM_wink		        = 0x10, /* Alias for NM_install */
    NM_memory_refresh	= 0x11,
    NM_query_SNVT	    = 0x12,
    NM_nv_fetch		    = 0x13,
    NM_service_pin	    = 0x1F,
} NM_msg_code;

#endif
// Define network diagnostic message codes

typedef enum ND_msg_code {
    ND_query_status	= 0x1,
    ND_proxy		= 0x2,
    ND_clear_status	= 0x3,
    ND_query_xcvr	= 0x4,
} ND_msg_code;

// Define offsets and masks for constructing request and response codes

#define NM_opcode_base	    0x60
#define NM_opcode_mask	    0x1F
#define NM_resp_mask	    0xE0
#define NM_resp_success	    0x20
#define NM_resp_failed	    0x00

#define ND_opcode_base	    0x50
#define ND_opcode_mask	    0x0F
#define ND_resp_mask	    0xF0
#define ND_resp_success	    0x30
#define ND_resp_failed	    0x10


//-------------------------------------------------------------
#define MgSHaederSize  14
//#define MgSMsgSize     32
#define MgSMsgSize     48
//#define MgSMsgSize     16
#define MNG_NoError     0x00
#define MNG_NACK        0x80
#define MNG_ACK         0x81
#define MNG_MSGSucceds  0x82
#define MNG_MSGFails    0x83
#define MNG_NoMSGBuff   0x84
#define MNG_Undef       -1

typedef struct //ManagerStruct
{
  unsigned char code;
  unsigned char service;
  msg_out_addr  MSGaddr;
  unsigned char MSGLen;
  unsigned char MSG[MgSMsgSize];

}ManagerStruct;
//-------------------------------------------------------------
enum {
  absolute = 0,
  read_only_relative = 1,
  config_relative = 2,
  statistics_relative = 3,
};

//-------------------------------------------------------------
typedef struct
{
unsigned char  mode;
unsigned short offset;
unsigned char  count;
} NM_read_memory_request;

//-------------------------------------------------------------
typedef struct {
  unsigned char neuron_id[ NEURON_ID_LEN ]; // offset 0x00
  unsigned char model_num; // offset 0x06
  //---- BYTE -----
  unsigned char minor_model_num : 4; // offset 0x07
  unsigned char : 4;
  //---- BYTE -----
  unsigned short nv_fixed_struct; // offset 0x08
  //---- BYTE -----
  unsigned char nv_count : 6;
  unsigned char : 1;
  unsigned char read_write_protect : 1; // offset 0x0A
}NodeInfoT;

//-------------------------------------------------------------
typedef struct {
  unsigned char id_string[ ID_STR_LEN ]; // offset 0x0D
  //---- BYTE -----
  unsigned char : 5;
  unsigned char explicit_addr : 1;
  unsigned char two_domains : 1;
  unsigned char NV_processing_off : 1; // offset 0x15
  //---- BYTE -----
  unsigned char : 4;
  unsigned char address_count : 4; // offset 0x16
  //---- BYTE -----
  unsigned char receive_trans_count : 4;
  unsigned char : 4; // offset 0x17
  //---- BYTE -----
  unsigned char app_buf_in_size : 4;
  unsigned char app_buf_out_size : 4; // offset 0x18
  //---- BYTE -----
  unsigned char net_buf_in_size : 4;
  unsigned char net_buf_out_size : 4; // offset 0x19
  //---- BYTE -----
  unsigned char app_buf_out_priority_count : 4;
  unsigned char net_buf_out_priority_count : 4; // offset 0x1A
  //---- BYTE -----
  unsigned char app_buf_in_count : 4;
  unsigned char app_buf_out_count : 4; // offset 0x1B
  //---- BYTE -----
  unsigned char net_buf_in_count : 4;
  unsigned char net_buf_out_count : 4; // offset 0x1C
}NodeInfo2T;

//-------------------------------------------------------------
typedef struct
{
	//---- BYTE -----
	unsigned alias_count : 6;			// offset 0x25
	unsigned : 2;						// offset 0x24
	//---- BYTE -----
	unsigned : 4;
	unsigned msg_tag_count : 4;
	//---- BYTE -----
	char     reserved2[3];
}NodeInfo3T; //read_only_data_struct_2;
//-------------------------------------------------------------
typedef struct
{
  unsigned char id[ DOMAIN_ID_LEN ];
  //---- BYTE -----
  unsigned char subnet;
  //---- BYTE -----
  unsigned char node : 7;
  unsigned char : 1;
  //---- BYTE -----
  unsigned char len;
  //---- BYTE -----
  unsigned char key[ AUTH_KEY_LEN ];
} NM_query_domain_responseT;


//-------------------------------------------------------------
typedef struct {
  unsigned char type;             // addr_type or (0x80 |group_size)
  //---- BYTE -----
  unsigned char member_or_node : 7;
  unsigned char domain : 1;
  //---- BYTE -----
  unsigned char retry : 4;
  unsigned char rpt_timer : 4;
  //---- BYTE -----
  unsigned char tx_timer : 4;
  unsigned char rcv_timer : 4;
  //---- BYTE -----
  unsigned char group_or_subnet;
} NM_query_addr_responseT;
//-------------------------------------------------------------
typedef struct {
  unsigned char hysteresis : 3;
  unsigned char filter : 2;
  unsigned char bit_sync_threshold: 2;
  unsigned char collision_detect : 1; // offset 0x11
  //---- BYTE -----
  unsigned char cd_preamble : 1;
  unsigned char cd_tail : 1;
  unsigned char : 6; // offset 0x12
  //---- BYTE -----
} direct_param_struct;

typedef struct {
  unsigned short channel_id; // offset 0x00
  char location[ LOCATION_LEN ]; // offset 0x02
  //---- BYTE -----
  unsigned char input_clock : 3;
  unsigned char comm_clock : 5; // offset 0x08
  //---- BYTE -----
  unsigned char comm_pin_dir : 5;
  unsigned char comm_type : 3; // offset 0x09
  //---- BYTE -----
  unsigned char reserved[ 5 ]; // offset 0x0A
  unsigned char node_priority; // offset 0x0F
  unsigned char channel_priorities; // offset 0x10
  union xc{
    unsigned char xcvr_params[ NUM_COMM_PARAMS ];
    direct_param_struct dir_params; // offset 0x11
  }xcvr;
  //---- BYTE -----
  unsigned char preemption_timeout: 3;
  unsigned char nm_auth : 1;
  unsigned char non_group_timer : 4; // offset 0x18
  //---- BYTE -----
} config_data_struct;
//-------------------------------------------------------------
#define QUERY_STATUS_CODE        0x51
enum {
  appl_offline = 0, // soft offline state
    appl_online = 1,
    appl_reset = 2,
    change_state = 3,
};
//-------------------------------------------------------------
#ifndef ENUM_NODE_STATE
#define ENUM_NODE_STATE
enum {
  appl_uncnfg = 2,
    no_appl_uncnfg = 3,
    cnfg_online = 4,
    cnfg_offline = 6, // hard offline state
    soft_offline = 0xC,
    cnfg_bypass = 0x8C
};// enum_node_state;
#endif
typedef struct
{
unsigned short xmit_errors;          // offset 0x00
unsigned short transaction_timeouts; // offset 0x02
unsigned short rcv_transaction_full; // offset 0x04
unsigned short lost_msgs;            // offset 0x06
unsigned short missed_msgs;          // offset 0x08
unsigned char  reset_cause;          // offset 0x0A
unsigned char  node_state;           // offset 0x0B
unsigned char  version_number;       // offset 0x0C
unsigned char  error_log;            // offset 0x0D
unsigned char  model_number;         // offset rolf 0x0E
} ND_query_status_responseT;
//-------------------------------------------------------------
typedef struct
{
  unsigned short transmission_errors;
  unsigned short transmit_tx_failures;
  unsigned short receive_tx_full;
  unsigned short lost_messages;
  unsigned short missed_messages;
  unsigned short layer2_received;
  unsigned short layer3_received;
  unsigned short layer3_transmitted;
  unsigned short transmit_tx_retries;
  unsigned short backlog_overflows;
  unsigned short late_acknowledgements;
  unsigned short collisions;
  unsigned char  eeprom_lock;
} stats_structT;
//--------------------------------------------------------------

typedef struct
{
  NodeInfoT  I1;
  NodeInfo2T I2;
  NodeInfo3T I3;
  NM_query_domain_responseT NM_query_domain_response0;
  NM_query_domain_responseT NM_query_domain_response1;
  char Location[LOCATION_LEN+1];
  short int Channel_id;
  config_data_struct config_data_s;
  NM_query_addr_responseT NM_query_addr_response[15];
  ND_query_status_responseT ND_query_status_response;
  stats_structT stats_struct;
}tNodeInfo;
//-------------------------------------------------------------
typedef struct {
  unsigned char nv_index;
  //---- BYTE -----
  unsigned char nv_selector_hi : 6;
  unsigned char nv_direction : 1;
  unsigned char nv_priority : 1;
  //---- BYTE -----
  unsigned char nv_selector_lo : 8;
  //---- BYTE -----
  unsigned char nv_addr_index : 4;
  unsigned char nv_auth : 1;
  unsigned char nv_service : 2;
  unsigned char nv_turnaround : 1;
  //---- BYTE -----
} NM_update_nv_cnfg_request;
//-------------------------------------------------------------
typedef struct {
  NM_update_nv_cnfg_request nv_cnfg;
  //---- BYTE -----
  unsigned char primary;
} NM_update_nv_alias_cnfg_request;
//-------------------------------------------------------------
#pragma pack(pop)

extern tNodeInfo NodeInfo;
//extern NM_query_addr_responseT NM_query_addr_response;
